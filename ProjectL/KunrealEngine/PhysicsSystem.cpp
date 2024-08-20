#include <cassert>
#include "PhysicsSystem.h"
#include "TimeManager.h"
#include "GameObject.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "Transform.h"

#include "GraphicsSystem.h"

KunrealEngine::PhysicsSystem::PhysicsSystem()
	:_foundation(nullptr), _physics(nullptr), _dispatcher(nullptr), _pxScene(nullptr),
	_material(nullptr), _pvd(nullptr), _cylinderConvexMesh(nullptr), _swordAuraConvexMesh(nullptr),
	col1(nullptr), col2(nullptr)
{

}

void KunrealEngine::PhysicsSystem::Initialize()
{
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocator, _errorCallback);

	// visual debugger 세팅, 로컬에 연결
	_pvd = PxCreatePvd(*_foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// 버전, 세팅, 단위 등의 정보를 담은 물리						// _tolerance를 따로 정해주지 않았다면
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true, _pvd);

	PxInitExtensions(*_physics, _pvd);

	// 물리 씬을 생성
	CreatePhysXScene();


	// 머티리얼 생성(임의)	/// 이게 0.5라서?
	_material = _physics->createMaterial(0.5f, 0.5f, 0.5f);

	// FBX cylinder 모양의 collider mesh 생성
	CreateCylinderData();

	// custom mesh 데이터 생성
	CreateSwordAuraData();

	///												// halfHeight가 0이면 sphere가 된다
	//physx::PxCapsuleGeometry capsuleGeometry(3.0f, 0.0f); // Radius: 1.0, Half Height: 2.0 (total height = 4.0)
	//physx::PxQuat rotation(physx::PxPi / 2, physx::PxVec3(0.0f, 0.0f, 1.0f));
	//physx::PxTransform capsuleTransform(physx::PxVec3(0.0f, 5.0f, 0.0f), rotation); // Position of the capsule
	//
	//physx::PxRigidDynamic* capsuleActor = _physics->createRigidDynamic(capsuleTransform);
	//physx::PxShape* capsuleShape = _physics->createShape(capsuleGeometry, *_material);
	//capsuleActor->attachShape(*capsuleShape);
	//
	//// Add the collider to a rigid body
	//_pxScene->addActor(*capsuleActor);


	///


}

void KunrealEngine::PhysicsSystem::Release()
{
	/// actor들 release 하는 부분 필요

	for (physx::PxRigidDynamic* actor : _rigidDynamics)
	{
		actor->release();
	}

	_rigidDynamics.clear();
	_rigidStatics.clear();
	_dynamicMap.clear();
	_staticMap.clear();

	_pxScene->release();
	_dispatcher->release();
	_material->release();
	_pvd->release();
	_physics->release();
	_foundation->release();
}

void KunrealEngine::PhysicsSystem::FixedUpdate()
{
	UpdateDynamics();

	_pxScene->simulate(TimeManager::GetInstance().GetDeltaTime());
	_pxScene->fetchResults(true);

}

void KunrealEngine::PhysicsSystem::CreatePhysXScene()
{
	// 씬에 대한 설정		// SceneDescription
	physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, 0.f, 0.f);				// 중력 설정		// 중력이 없으니 0 0 0
	_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = _dispatcher;
	sceneDesc.simulationEventCallback = this;

	// 필터를 어떻게 넣어주느냐에 따라 이벤트를 callback 받을 때 다른 행동
	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = TriggerUsingFilterShader;

	_pxScene = _physics->createScene(sceneDesc);

	// Pvd에 정보 보내기
	physx::PxPvdSceneClient* pvdClient = _pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

}

physx::PxScene* KunrealEngine::PhysicsSystem::GetPxScene()
{
	return _pxScene;
}

void KunrealEngine::PhysicsSystem::CreateDynamicBoxCollider(Collider* collider)
{
	physx::PxTransform trans(physx::PxVec3(collider->GetColliderPos().x, collider->GetColliderPos().y, collider->GetColliderPos().z));
	physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));

	physx::PxRigidDynamic* boxActor = _physics->createRigidDynamic(trans);

	physx::PxShape* boxShape = _physics->createShape(physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *_material);

	boxActor->attachShape(*boxShape);

	// 우리 게임에서는 정교한 물리가 필요하지 않고 트리거 발동 여부만 필요
	// 충돌 시 아무런 물리적 변화가 없도록 설정
	physx::PxRigidDynamicLockFlags flag =
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	boxActor->setRigidDynamicLockFlags(flag);

	// UserData
	PhysicsUserData data(collider->GetOwnerObject(), boxShape);
	boxActor->userData = &data;

	collider->_shape = boxShape;

	_pxScene->addActor(*boxActor);
	_dynamicMap.insert(std::make_pair(collider, boxActor));
	_rigidDynamics.push_back(boxActor);
}

void KunrealEngine::PhysicsSystem::CreateCylinderCollider(Collider* collider)
{
	physx::PxMeshScale scale(physx::PxVec3(
		collider->GetColliderScale().x, collider->GetColliderScale().y, collider->GetColliderScale().z),
		physx::PxQuat(physx::PxPi / 2, physx::PxVec3(1.0f, 0.0f, 0.0f)));		// 실린더가 90도 누워서 출력되기 때문에 세워준다
	physx::PxConvexMeshGeometry geom(_cylinderConvexMesh, scale);
	physx::PxShape* shape = _physics->createShape(geom, *_material);

	physx::PxTransform trans(physx::PxVec3(collider->GetColliderPos().x, collider->GetColliderPos().y, collider->GetColliderPos().z));
	physx::PxRigidDynamic* customActor = _physics->createRigidDynamic(trans);

	// 충돌 관련 flag들
	physx::PxRigidDynamicLockFlags flag =
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	customActor->setRigidDynamicLockFlags(flag);

	// UserData
	PhysicsUserData data(collider->GetOwnerObject(), shape);
	customActor->userData = &data;

	// 다 만들었으면 shape 붙여주고
	customActor->attachShape(*shape);

	// collider 컴포넌트에 shape 전달
	collider->_shape = shape;

	// scene에 추가
	_pxScene->addActor(*customActor);

	// 관리를 위해 physicsSystem의 멤버로
	_dynamicMap.insert(std::make_pair(collider, customActor));
	_rigidDynamics.push_back(customActor);
}

void KunrealEngine::PhysicsSystem::CreateDynamicSphereCollider(SphereCollider* collider)
{

}


void KunrealEngine::PhysicsSystem::CreateMeshCollider(Collider* collider, std::string meshName)
{
	physx::PxMeshScale scale(physx::PxVec3(
		collider->GetColliderScale().x, collider->GetColliderScale().y, collider->GetColliderScale().z),
		physx::PxQuat(physx::PxPi / 2, physx::PxVec3(1.0f, 0.0f, 0.0f)));		// mesh가 90도 누워서 출력되기 때문에 세워준다

	physx::PxConvexMeshGeometry geom;

	if (meshName == "Blade")
	{
		geom = { _swordAuraConvexMesh, scale };
	}
	else
	{
		std::cout << "Cannot find FBX" << std::endl;
		return;
	}

	physx::PxShape* shape = _physics->createShape(geom, *_material);

	physx::PxTransform trans(physx::PxVec3(collider->GetColliderPos().x, collider->GetColliderPos().y, collider->GetColliderPos().z));
	physx::PxRigidDynamic* customActor = _physics->createRigidDynamic(trans);

	// 충돌 관련 flag들
	physx::PxRigidDynamicLockFlags flag =
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	customActor->setRigidDynamicLockFlags(flag);

	// UserData
	PhysicsUserData data(collider->GetOwnerObject(), shape);
	customActor->userData = &data;

	// 다 만들었으면 shape 붙여주고
	customActor->attachShape(*shape);

	// collider 컴포넌트에 shape 전달
	collider->_shape = shape;

	// scene에 추가
	_pxScene->addActor(*customActor);

	// 관리를 위해 physicsSystem의 멤버로
	_dynamicMap.insert(std::make_pair(collider, customActor));
	_rigidDynamics.push_back(customActor);
}

void KunrealEngine::PhysicsSystem::UpdateDynamics()
{
	// 포지션 관련
	for (const auto& pair : _dynamicMap)
	{
		pair.first->Clear();

		if (pair.first->GetOwnerObject()->GetObjectName() == "Player" || pair.first->GetOwnerObject()->GetTag() == "Boss")
		{
			pair.second->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
		}

		physx::PxTransform pxTrans;
		physx::PxVec3 pxPos(pair.first->GetColliderPos().x, pair.first->GetColliderPos().y, pair.first->GetColliderPos().z);
		physx::PxQuat pxRot = EulerToQuaternion(pair.first->GetOwnerObject()->GetComponent<Transform>()->GetRotation().x, pair.first->GetOwnerObject()->GetComponent<Transform>()->GetRotation().y, pair.first->GetOwnerObject()->GetComponent<Transform>()->GetRotation().z);

		pxTrans.p = pxPos;
		pxTrans.q = pxRot;

		if (!pxTrans.isValid())
		{
			std::cout << "PHYSX TRANSFORM ERROR" << std::endl;
		}

		if (!pair.first->GetOwnerObject()->GetActivated())
		{
			pxTrans.p = physx::PxVec3(4000.0f, 4000.0f, 4000.0f);

			pair.first->_isCollided = false;
			pair.first->_targetObj = nullptr;
		}

		pair.second->setGlobalPose(pxTrans, true);
	}
}

void KunrealEngine::PhysicsSystem::SetBoxSize(Collider* collider)
{
	/// attach된 shape의 크기를 직접 변경해주는 함수가 없어서 사이즈 변경 함수가 호출될 때마다 삭제/추가를 반복하도록 만들었음
	// 붙여줬던 shape를 떼주고
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);
	//
	// 메모리 해제
	// detachShape에서 delete까지 해주는 모양이다
	////static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape->release();
	//
	// 크기에 맞게 새로운 shape 생성
	physx::PxShape* boxShape = _physics->createShape(physx::PxBoxGeometry(
		collider->GetColliderScale().x / 2.f,
		collider->GetColliderScale().y / 2.f,
		collider->GetColliderScale().z / 2.f), *_material);

	// 새롭게 만든 shape 추가
	this->_dynamicMap.at(collider)->attachShape(*boxShape);
	//
	// userData 업데이트
	static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape = boxShape;
}


void KunrealEngine::PhysicsSystem::SetCylinderSize(Collider* collider)
{
	// 붙여줬던 shape를 떼주고
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);

	// 크기에 맞게 새로운 shape 생성
	// box의 scale에 맞추기 위해 전체적인 scale을 절반으로
	physx::PxMeshScale scale(physx::PxVec3(
		collider->GetColliderScale().x / 2.0f, collider->GetColliderScale().y / 2.0f, collider->GetColliderScale().z / 2.0f));
	physx::PxConvexMeshGeometry geom(_cylinderConvexMesh, scale);
	physx::PxShape* shape = _physics->createShape(geom, *_material);

	// 만들었던 shape를 다시 붙여준다
	this->_dynamicMap.at(collider)->attachShape(*shape);

	// userData 업데이트
	static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape = shape;

}


void KunrealEngine::PhysicsSystem::SetMeshSize(Collider* collider, std::string meshName)
{
	// 붙여줬던 shape를 떼주고
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);

	// 크기에 맞게 새로운 shape 생성
	// box의 scale에 맞추기 위해 전체적인 scale을 절반으로
	physx::PxMeshScale scale(physx::PxVec3(
		collider->GetColliderScale().x / 2.0f, collider->GetColliderScale().y / 2.0f, collider->GetColliderScale().z / 2.0f));

	physx::PxConvexMeshGeometry geom;

	if (meshName == "Blade")
	{
		geom = { _swordAuraConvexMesh, scale };
	}
	else
	{
		std::cout << "Cannot find FBX" << std::endl;
		return;
	}

	physx::PxShape* shape = _physics->createShape(geom, *_material);

	// 만들었던 shape를 다시 붙여준다
	this->_dynamicMap.at(collider)->attachShape(*shape);

	// userData 업데이트
	static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape = shape;
}

void KunrealEngine::PhysicsSystem::SetActorState(Collider* collider, bool active)
{
	if (!active)
	{
		physx::PxQuat pxRot = EulerToQuaternion(collider->GetOwnerObject()->GetComponent<Transform>()->GetRotation().x, collider->GetOwnerObject()->GetComponent<Transform>()->GetRotation().y, collider->GetOwnerObject()->GetComponent<Transform>()->GetRotation().z);

		physx::PxTransform pxTrans;
		pxTrans.p = physx::PxVec3(4000.0f, 4000.0f, 4000.0f);
		pxTrans.q = pxRot;

		this->_dynamicMap.at(collider)->setGlobalPose(pxTrans, false);
	}

	this->_dynamicMap.at(collider)->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);
	//collider->_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, active);
	//collider->_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, active);
}


void KunrealEngine::PhysicsSystem::ReleasePhysxObject(Collider* collider)
{
	this->_dynamicMap.erase(collider);
	this->_dynamicMap.at(collider)->release();
}

void KunrealEngine::PhysicsSystem::TestFunc()
{
	physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));		// 위치
	physx::PxBoxGeometry geometry(physx::PxVec3(2.0f, 2.0f, 2.0f));		// 크기

	physx::PxTransform transform2(physx::PxVec3(1.0f, 10.0f, 0.0f));
	physx::PxBoxGeometry geometry2(physx::PxVec3(2.0f, 2.0f, 2.0f));

	physx::PxRigidDynamic* boxActor = PxCreateDynamic(*_physics, transform, geometry, *_material, 1.0f);
	physx::PxRigidDynamic* boxActor2 = PxCreateDynamic(*_physics, transform2, geometry2, *_material, 1.0f);

	//physx::PxShape* shape = _physics->createShape;

	//boxActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);

	physx::PxRigidDynamicLockFlags flag =
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y |
		physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;

	//boxActor->setRigidDynamicLockFlags(flag);

	//shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	//shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	//boxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);		// 중력의 영향을 받지 않게
	//boxActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);	// simlation을 돌리지 않게 -> sleep 상태로 만들어서 아무것도 안하게 만듦

	//boxActor->userData = this;

	//_pxScene->addActor(*boxActor);
	//_pxScene->addActor(*boxActor2);
}


void KunrealEngine::PhysicsSystem::CreateCylinderData()
{
	// 실린더 데이터 
	std::vector<DirectX::XMFLOAT3> cylinderVertices = GRAPHICS->GetMeshVertexData("cylinder/cylinder");

	// 정점 정보
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = cylinderVertices.size();
	convexDesc.points.stride = sizeof(DirectX::XMFLOAT3);
	convexDesc.points.data = &cylinderVertices[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	const physx::PxCookingParams params(_physics->getTolerancesScale());

	// 정점 메모리 버퍼
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	const bool status = PxCookConvexMesh(params, convexDesc, buf, &result);

	// mesh 생성
	physx::PxDefaultMemoryInputData inputi(buf.getData(), buf.getSize());
	_cylinderConvexMesh = _physics->createConvexMesh(inputi);
}


void KunrealEngine::PhysicsSystem::CreateSwordAuraData()
{
	// 검기 데이터 
	std::vector<DirectX::XMFLOAT3> bladeVertices = GRAPHICS->GetMeshVertexData("Blade/Blade");

	// 정점 정보
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = bladeVertices.size();
	convexDesc.points.stride = sizeof(DirectX::XMFLOAT3);
	convexDesc.points.data = &bladeVertices[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	const physx::PxCookingParams params(_physics->getTolerancesScale());

	// 정점 메모리 버퍼
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	const bool status = PxCookConvexMesh(params, convexDesc, buf, &result);

	// mesh 생성
	physx::PxDefaultMemoryInputData inputi(buf.getData(), buf.getSize());
	_swordAuraConvexMesh = _physics->createConvexMesh(inputi);
}

bool KunrealEngine::PhysicsSystem::IsTrigger(const physx::PxFilterData& data)
{
	if (data.word0 != 0xffffffff)
		return false;
	if (data.word1 != 0xffffffff)
		return false;
	if (data.word2 != 0xffffffff)
		return false;
	if (data.word3 != 0xffffffff)
		return false;
	return true;
}

physx::PxFilterFlags KunrealEngine::PhysicsSystem::TriggerUsingFilterShader(physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/)
{
	pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;

	return physx::PxFilterFlag::eDEFAULT;
}

KunrealEngine::Collider* KunrealEngine::PhysicsSystem::GetColliderFromDynamic(const physx::PxRigidDynamic* dynamic)
{
	for (const auto& pair : _dynamicMap)
	{
		if (pair.second == dynamic)
		{
			return pair.first;
		}
	}
}


physx::PxQuat KunrealEngine::PhysicsSystem::EulerToQuaternion(float pitch, float yaw, float roll)
{
	pitch *= (physx::PxPi / 180.0f);
	yaw *= (physx::PxPi / 180.0f);
	roll *= (physx::PxPi / 180.0f);

	physx::PxQuat qx(pitch, physx::PxVec3(1.0f, 0.0f, 0.0f));
	physx::PxQuat qy(yaw, physx::PxVec3(0.0f, 1.0f, 0.0f));
	physx::PxQuat qz(roll, physx::PxVec3(0.0f, 0.0f, 1.0f));

	return qy * qx * qz;
}

void KunrealEngine::PhysicsSystem::PlayerForceUpdate()
{
	for (auto actor : _dynamicMap)
	{
		if (actor.first->GetOwnerObject()->GetObjectName() == "Player" || actor.first->GetOwnerObject()->GetTag() == "Boss")
		{
			actor.second->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
		}
	}
}

void KunrealEngine::PhysicsSystem::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{

}

void KunrealEngine::PhysicsSystem::onWake(physx::PxActor**, physx::PxU32)
{

}

void KunrealEngine::PhysicsSystem::onSleep(physx::PxActor** actors, physx::PxU32 count)
{

}

void KunrealEngine::PhysicsSystem::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	const physx::PxContactPair& current = *pairs++;

	/// collider를 여러개 더 추가해봐도 pairHeader의 actor들은 두개로 잡힌다
	/// 충돌한 두개만을 받아오는건 맞다는 이야기

	/// PxActor와 PxRigidDynamic과의 관계 때문에?
	/// PxBase -> PxActor -> PxRigidActor -> PxRigidBody -> PxRigidDynamic 순으로 상속 됌
	/// pairHeader가 들고 있는 멤버는 PxActor
									/// 다운 캐스팅이니까 dynamic_cast는 하지마라
	//physx::PxRigidActor* casted1 = static_cast<physx::PxRigidActor*>(pairHeader.actors[0]);
	//physx::PxRigidBody* casted2 = static_cast<physx::PxRigidBody*>(casted1);
	//physx::PxRigidDynamic* casted3 = static_cast<physx::PxRigidDynamic*>(casted2);

	//auto finalData = casted3->userData;
	//PhysicsUserData* finalDatata = static_cast<PhysicsUserData*>(casted3->userData);

	/// void*가 문제인걸까 결국 userData에는 nullptr만이 들어있을뿐이다
	/// 모르겠다

	/// cast는 안되는거 같다
	//physx::PxRigidDynamic* asfgd = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[1]);
	//physx::PxTransform ads = asfgd->getGlobalPose();

	/// PxActor의 userData라 안되는 모양이다		// 부모의 기능 같은데 왜 접근이 안될까
	/// PxRigidDynamic으로 만들고 addActor 함수로 추가했는데 왜 안되는지는 의문
	//PhysicsUserData* dataA = static_cast<PhysicsUserData*>(pairHeader.actors[0]->userData);
	//PhysicsUserData* dataB = static_cast<PhysicsUserData*>(pairHeader.actors[1]->userData);

	/// 결국 collider에 직접 접근해서 데이터를 넣어주기로함
	/// 나도 이쁘게 만들고 싶었어
	physx::PxRigidDynamic* casted1 = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[0]);
	physx::PxRigidDynamic* casted2 = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[1]);

	col1 = GetColliderFromDynamic(casted1);
	col2 = GetColliderFromDynamic(casted2);

	// collider 자체 혹은 둘의 부모중 하나가 비활성화라면 체크하지 않음
	if (!col1->GetOwnerObject()->GetActivated() || !col2->GetOwnerObject()->GetActivated() || !col1->_colliderActivated || !col2->_colliderActivated)
	{
		return;
	}

	// 충돌이 발생했을 때
	if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		| physx::PxPairFlag::eMODIFY_CONTACTS
		//&& col1->GetOwnerObject()->GetActivated() && col2->GetOwnerObject()->GetActivated())
		)
	{
		// 충돌 여부를 true로
		col1->_isCollided = true;
		col2->_isCollided = true;

		// 상대 오브젝트에 대한 정보를 넘겨줌
		col1->_targetObj = col2->GetOwnerObject();
		col2->_targetObj = col1->GetOwnerObject();
	}

	// 충돌에서 벗어났을 때
	if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		//&& col1->GetOwnerObject()->GetActivated() && col2->GetOwnerObject()->GetActivated() || (!col1->GetOwnerObject()->GetActivated() || !col2->GetOwnerObject()->GetActivated()))
		)
	{
		// 충돌 여부를 false로
		col1->_isCollided = false;
		col2->_isCollided = false;

		// 충돌에서 벗어났으니 nullptr로
		col1->_targetObj = nullptr;
		col2->_targetObj = nullptr;
	}
}

void KunrealEngine::PhysicsSystem::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{

}

void KunrealEngine::PhysicsSystem::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{

}
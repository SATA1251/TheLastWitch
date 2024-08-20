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

	// visual debugger ����, ���ÿ� ����
	_pvd = PxCreatePvd(*_foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// ����, ����, ���� ���� ������ ���� ����						// _tolerance�� ���� �������� �ʾҴٸ�
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), true, _pvd);

	PxInitExtensions(*_physics, _pvd);

	// ���� ���� ����
	CreatePhysXScene();


	// ��Ƽ���� ����(����)	/// �̰� 0.5��?
	_material = _physics->createMaterial(0.5f, 0.5f, 0.5f);

	// FBX cylinder ����� collider mesh ����
	CreateCylinderData();

	// custom mesh ������ ����
	CreateSwordAuraData();

	///												// halfHeight�� 0�̸� sphere�� �ȴ�
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
	/// actor�� release �ϴ� �κ� �ʿ�

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
	// ���� ���� ����		// SceneDescription
	physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.f, 0.f, 0.f);				// �߷� ����		// �߷��� ������ 0 0 0
	_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = _dispatcher;
	sceneDesc.simulationEventCallback = this;

	// ���͸� ��� �־��ִ��Ŀ� ���� �̺�Ʈ�� callback ���� �� �ٸ� �ൿ
	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = TriggerUsingFilterShader;

	_pxScene = _physics->createScene(sceneDesc);

	// Pvd�� ���� ������
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

	// �츮 ���ӿ����� ������ ������ �ʿ����� �ʰ� Ʈ���� �ߵ� ���θ� �ʿ�
	// �浹 �� �ƹ��� ������ ��ȭ�� ������ ����
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
		physx::PxQuat(physx::PxPi / 2, physx::PxVec3(1.0f, 0.0f, 0.0f)));		// �Ǹ����� 90�� ������ ��µǱ� ������ �����ش�
	physx::PxConvexMeshGeometry geom(_cylinderConvexMesh, scale);
	physx::PxShape* shape = _physics->createShape(geom, *_material);

	physx::PxTransform trans(physx::PxVec3(collider->GetColliderPos().x, collider->GetColliderPos().y, collider->GetColliderPos().z));
	physx::PxRigidDynamic* customActor = _physics->createRigidDynamic(trans);

	// �浹 ���� flag��
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

	// �� ��������� shape �ٿ��ְ�
	customActor->attachShape(*shape);

	// collider ������Ʈ�� shape ����
	collider->_shape = shape;

	// scene�� �߰�
	_pxScene->addActor(*customActor);

	// ������ ���� physicsSystem�� �����
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
		physx::PxQuat(physx::PxPi / 2, physx::PxVec3(1.0f, 0.0f, 0.0f)));		// mesh�� 90�� ������ ��µǱ� ������ �����ش�

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

	// �浹 ���� flag��
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

	// �� ��������� shape �ٿ��ְ�
	customActor->attachShape(*shape);

	// collider ������Ʈ�� shape ����
	collider->_shape = shape;

	// scene�� �߰�
	_pxScene->addActor(*customActor);

	// ������ ���� physicsSystem�� �����
	_dynamicMap.insert(std::make_pair(collider, customActor));
	_rigidDynamics.push_back(customActor);
}

void KunrealEngine::PhysicsSystem::UpdateDynamics()
{
	// ������ ����
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
	/// attach�� shape�� ũ�⸦ ���� �������ִ� �Լ��� ��� ������ ���� �Լ��� ȣ��� ������ ����/�߰��� �ݺ��ϵ��� �������
	// �ٿ���� shape�� ���ְ�
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);
	//
	// �޸� ����
	// detachShape���� delete���� ���ִ� ����̴�
	////static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape->release();
	//
	// ũ�⿡ �°� ���ο� shape ����
	physx::PxShape* boxShape = _physics->createShape(physx::PxBoxGeometry(
		collider->GetColliderScale().x / 2.f,
		collider->GetColliderScale().y / 2.f,
		collider->GetColliderScale().z / 2.f), *_material);

	// ���Ӱ� ���� shape �߰�
	this->_dynamicMap.at(collider)->attachShape(*boxShape);
	//
	// userData ������Ʈ
	static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape = boxShape;
}


void KunrealEngine::PhysicsSystem::SetCylinderSize(Collider* collider)
{
	// �ٿ���� shape�� ���ְ�
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);

	// ũ�⿡ �°� ���ο� shape ����
	// box�� scale�� ���߱� ���� ��ü���� scale�� ��������
	physx::PxMeshScale scale(physx::PxVec3(
		collider->GetColliderScale().x / 2.0f, collider->GetColliderScale().y / 2.0f, collider->GetColliderScale().z / 2.0f));
	physx::PxConvexMeshGeometry geom(_cylinderConvexMesh, scale);
	physx::PxShape* shape = _physics->createShape(geom, *_material);

	// ������� shape�� �ٽ� �ٿ��ش�
	this->_dynamicMap.at(collider)->attachShape(*shape);

	// userData ������Ʈ
	static_cast<PhysicsUserData*>(_dynamicMap.at(collider)->userData)->shape = shape;

}


void KunrealEngine::PhysicsSystem::SetMeshSize(Collider* collider, std::string meshName)
{
	// �ٿ���� shape�� ���ְ�
	this->_dynamicMap.at(collider)->detachShape(*collider->_shape);

	// ũ�⿡ �°� ���ο� shape ����
	// box�� scale�� ���߱� ���� ��ü���� scale�� ��������
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

	// ������� shape�� �ٽ� �ٿ��ش�
	this->_dynamicMap.at(collider)->attachShape(*shape);

	// userData ������Ʈ
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
	physx::PxTransform transform(physx::PxVec3(0.0f, 10.0f, 0.0f));		// ��ġ
	physx::PxBoxGeometry geometry(physx::PxVec3(2.0f, 2.0f, 2.0f));		// ũ��

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

	//boxActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);		// �߷��� ������ ���� �ʰ�
	//boxActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);	// simlation�� ������ �ʰ� -> sleep ���·� ���� �ƹ��͵� ���ϰ� ����

	//boxActor->userData = this;

	//_pxScene->addActor(*boxActor);
	//_pxScene->addActor(*boxActor2);
}


void KunrealEngine::PhysicsSystem::CreateCylinderData()
{
	// �Ǹ��� ������ 
	std::vector<DirectX::XMFLOAT3> cylinderVertices = GRAPHICS->GetMeshVertexData("cylinder/cylinder");

	// ���� ����
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = cylinderVertices.size();
	convexDesc.points.stride = sizeof(DirectX::XMFLOAT3);
	convexDesc.points.data = &cylinderVertices[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	const physx::PxCookingParams params(_physics->getTolerancesScale());

	// ���� �޸� ����
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	const bool status = PxCookConvexMesh(params, convexDesc, buf, &result);

	// mesh ����
	physx::PxDefaultMemoryInputData inputi(buf.getData(), buf.getSize());
	_cylinderConvexMesh = _physics->createConvexMesh(inputi);
}


void KunrealEngine::PhysicsSystem::CreateSwordAuraData()
{
	// �˱� ������ 
	std::vector<DirectX::XMFLOAT3> bladeVertices = GRAPHICS->GetMeshVertexData("Blade/Blade");

	// ���� ����
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = bladeVertices.size();
	convexDesc.points.stride = sizeof(DirectX::XMFLOAT3);
	convexDesc.points.data = &bladeVertices[0];
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	const physx::PxCookingParams params(_physics->getTolerancesScale());

	// ���� �޸� ����
	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	const bool status = PxCookConvexMesh(params, convexDesc, buf, &result);

	// mesh ����
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

	/// collider�� ������ �� �߰��غ��� pairHeader�� actor���� �ΰ��� ������
	/// �浹�� �ΰ����� �޾ƿ��°� �´ٴ� �̾߱�

	/// PxActor�� PxRigidDynamic���� ���� ������?
	/// PxBase -> PxActor -> PxRigidActor -> PxRigidBody -> PxRigidDynamic ������ ��� ��
	/// pairHeader�� ��� �ִ� ����� PxActor
									/// �ٿ� ĳ�����̴ϱ� dynamic_cast�� ��������
	//physx::PxRigidActor* casted1 = static_cast<physx::PxRigidActor*>(pairHeader.actors[0]);
	//physx::PxRigidBody* casted2 = static_cast<physx::PxRigidBody*>(casted1);
	//physx::PxRigidDynamic* casted3 = static_cast<physx::PxRigidDynamic*>(casted2);

	//auto finalData = casted3->userData;
	//PhysicsUserData* finalDatata = static_cast<PhysicsUserData*>(casted3->userData);

	/// void*�� �����ΰɱ� �ᱹ userData���� nullptr���� ����������̴�
	/// �𸣰ڴ�

	/// cast�� �ȵǴ°� ����
	//physx::PxRigidDynamic* asfgd = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[1]);
	//physx::PxTransform ads = asfgd->getGlobalPose();

	/// PxActor�� userData�� �ȵǴ� ����̴�		// �θ��� ��� ������ �� ������ �ȵɱ�
	/// PxRigidDynamic���� ����� addActor �Լ��� �߰��ߴµ� �� �ȵǴ����� �ǹ�
	//PhysicsUserData* dataA = static_cast<PhysicsUserData*>(pairHeader.actors[0]->userData);
	//PhysicsUserData* dataB = static_cast<PhysicsUserData*>(pairHeader.actors[1]->userData);

	/// �ᱹ collider�� ���� �����ؼ� �����͸� �־��ֱ����
	/// ���� �̻ڰ� ����� �;���
	physx::PxRigidDynamic* casted1 = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[0]);
	physx::PxRigidDynamic* casted2 = static_cast<physx::PxRigidDynamic*>(pairHeader.actors[1]);

	col1 = GetColliderFromDynamic(casted1);
	col2 = GetColliderFromDynamic(casted2);

	// collider ��ü Ȥ�� ���� �θ��� �ϳ��� ��Ȱ��ȭ��� üũ���� ����
	if (!col1->GetOwnerObject()->GetActivated() || !col2->GetOwnerObject()->GetActivated() || !col1->_colliderActivated || !col2->_colliderActivated)
	{
		return;
	}

	// �浹�� �߻����� ��
	if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_CCD | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		| physx::PxPairFlag::eMODIFY_CONTACTS
		//&& col1->GetOwnerObject()->GetActivated() && col2->GetOwnerObject()->GetActivated())
		)
	{
		// �浹 ���θ� true��
		col1->_isCollided = true;
		col2->_isCollided = true;

		// ��� ������Ʈ�� ���� ������ �Ѱ���
		col1->_targetObj = col2->GetOwnerObject();
		col2->_targetObj = col1->GetOwnerObject();
	}

	// �浹���� ����� ��
	if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		//&& col1->GetOwnerObject()->GetActivated() && col2->GetOwnerObject()->GetActivated() || (!col1->GetOwnerObject()->GetActivated() || !col2->GetOwnerObject()->GetActivated()))
		)
	{
		// �浹 ���θ� false��
		col1->_isCollided = false;
		col2->_isCollided = false;

		// �浹���� ������� nullptr��
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
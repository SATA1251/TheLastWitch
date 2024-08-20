#pragma once

#include <unordered_map>
#include <DirectXMath.h>
#include "CommonHeader.h"

#pragma warning(disable : 26451 26495 6297 33010)
#include <physx/PxPhysics.h>
#include <physx/PxPhysicsAPI.h>
#include <physx/extensions/PxDefaultAllocator.h>
#include <physx/extensions/PxDefaultErrorCallback.h>
#pragma warning(default : 26451 26495 6297 33010)

namespace KunrealEngine
{
	class GameObject;
	class Collider;
	class BoxCollider;
	class SphereCollider;
	class CylinderCollider;

	class _DECLSPEC PhysicsSystem : public physx::PxSimulationEventCallback
	{
		// actor의 userData에 추가할 구조체
		struct PhysicsUserData
		{
			GameObject* gameObject;
			physx::PxShape* shape;
		};

	private:
		PhysicsSystem();

		// 싱글톤
	public:
		static PhysicsSystem& GetInstance()
		{
			static PhysicsSystem _instance;
			return _instance;
		}

	public:
		void Initialize();
		void Release();
		void FixedUpdate();

	public:
		// eventcallback
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
		virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override;
		virtual void onWake(physx::PxActor**, physx::PxU32) override;
		virtual void onSleep(physx::PxActor**, physx::PxU32) override;
		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform * poseBuffer, const physx::PxU32 count) override;

	public:
		void CreatePhysXScene();
		physx::PxScene* GetPxScene();

	public:
		// 움직이는 객체에 부여할 box collider
		void CreateDynamicBoxCollider(Collider* collider);

		// cylinder collider
		void CreateCylinderCollider(Collider* collider);

		// sphere collider
		void CreateDynamicSphereCollider(SphereCollider* collider);

		// custom mesh collider
		void CreateMeshCollider(Collider* collider, std::string meshName);

		// physx 객체의 크기를 Collider의 크기에 맞게
		void SetBoxSize(Collider* collider);
		void SetCylinderSize(Collider* collider);
		void SetMeshSize(Collider* collider, std::string meshName);

		// physx 객체 on off
		void SetActorState(Collider* collider, bool active);

		// physx 객체 제거
		void ReleasePhysxObject(Collider* collider);

		// Dynamic actor들 업데이트
		void UpdateDynamics();

		void TestFunc();			/// 실험용 꼭 지워

	private:
		std::vector<physx::PxRigidDynamic*>	_rigidDynamics;
		std::vector<physx::PxRigidStatic*>	_rigidStatics;
		std::vector<physx::PxFixedJoint*>	_fixedJoints;

		// 컴포넌트에서 가지고 있는 Transform 정보를 지속적으로 넘겨주기 위해
		// actor의 userData 기능을 쓰면 될 것 같기도한데.. 일단 테스트중
		std::unordered_map<Collider*, physx::PxRigidDynamic*> _dynamicMap;
		std::unordered_map<Collider*, physx::PxRigidStatic*> _staticMap;

	private:
		physx::PxDefaultAllocator			_allocator;
		physx::PxDefaultErrorCallback		_errorCallback;
		physx::PxTolerancesScale			_torlerance;
		physx::PxFoundation*				_foundation;
		physx::PxPhysics*					_physics;
		physx::PxScene*						_pxScene;
		physx::PxDefaultCpuDispatcher*		_dispatcher;
		physx::PxMaterial*					_material;
		physx::PxPvd*						_pvd;

		physx::PxConvexMesh* _cylinderConvexMesh;
		physx::PxConvexMesh* _swordAuraConvexMesh;

	private:
		// FBX로부터 Cylinder 모양 추출
		void CreateCylinderData();

		// FBX로부터 검기 모양 추출
		void CreateSwordAuraData();

	// eventcallback 대응함수
	private:
		bool IsTrigger(const physx::PxFilterData& data);

		static physx::PxFilterFlags TriggerUsingFilterShader(physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/);
	
	private:
		// eventcallback으로부터 받은 actor가 어떤 collider에 대응하는지
		Collider* GetColliderFromDynamic(const physx::PxRigidDynamic* dynamic);

		// physx용 quaternion
		physx::PxQuat EulerToQuaternion(float pitch, float yaw, float roll);

		// 마지막에 호출
	public:
		void PlayerForceUpdate();
		///

	private:
		Collider* col1;
		Collider* col2;
	};
}
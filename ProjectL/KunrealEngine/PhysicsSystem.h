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
		// actor�� userData�� �߰��� ����ü
		struct PhysicsUserData
		{
			GameObject* gameObject;
			physx::PxShape* shape;
		};

	private:
		PhysicsSystem();

		// �̱���
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
		// �����̴� ��ü�� �ο��� box collider
		void CreateDynamicBoxCollider(Collider* collider);

		// cylinder collider
		void CreateCylinderCollider(Collider* collider);

		// sphere collider
		void CreateDynamicSphereCollider(SphereCollider* collider);

		// custom mesh collider
		void CreateMeshCollider(Collider* collider, std::string meshName);

		// physx ��ü�� ũ�⸦ Collider�� ũ�⿡ �°�
		void SetBoxSize(Collider* collider);
		void SetCylinderSize(Collider* collider);
		void SetMeshSize(Collider* collider, std::string meshName);

		// physx ��ü on off
		void SetActorState(Collider* collider, bool active);

		// physx ��ü ����
		void ReleasePhysxObject(Collider* collider);

		// Dynamic actor�� ������Ʈ
		void UpdateDynamics();

		void TestFunc();			/// ����� �� ����

	private:
		std::vector<physx::PxRigidDynamic*>	_rigidDynamics;
		std::vector<physx::PxRigidStatic*>	_rigidStatics;
		std::vector<physx::PxFixedJoint*>	_fixedJoints;

		// ������Ʈ���� ������ �ִ� Transform ������ ���������� �Ѱ��ֱ� ����
		// actor�� userData ����� ���� �� �� ���⵵�ѵ�.. �ϴ� �׽�Ʈ��
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
		// FBX�κ��� Cylinder ��� ����
		void CreateCylinderData();

		// FBX�κ��� �˱� ��� ����
		void CreateSwordAuraData();

	// eventcallback �����Լ�
	private:
		bool IsTrigger(const physx::PxFilterData& data);

		static physx::PxFilterFlags TriggerUsingFilterShader(physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/);
	
	private:
		// eventcallback���κ��� ���� actor�� � collider�� �����ϴ���
		Collider* GetColliderFromDynamic(const physx::PxRigidDynamic* dynamic);

		// physx�� quaternion
		physx::PxQuat EulerToQuaternion(float pitch, float yaw, float roll);

		// �������� ȣ��
	public:
		void PlayerForceUpdate();
		///

	private:
		Collider* col1;
		Collider* col2;
	};
}
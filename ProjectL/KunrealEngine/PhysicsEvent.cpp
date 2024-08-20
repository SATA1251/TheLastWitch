#include <cassert>
#include "PhysicsEvent.h"

void PhysicsEvent::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
{
	for (size_t i = 0; i < count; i++)
	{
		physx::PxRigidDynamic* rigid1;
		physx::PxRigidDynamic* rigid2;
		//constraints[i].constraint->getActors(rigid1, rigid2);

		//static_cast<RigidBody*>(rigid1->userData)->gameobject;
	}


	//throw std::logic_error("The method or operation is not implemented.");
}

void PhysicsEvent::onWake(PxActor** actors, PxU32 count)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

void PhysicsEvent::onSleep(PxActor** actors, PxU32 count)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

void PhysicsEvent::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	//assert("¥Î¿Á«–");
}

void PhysicsEvent::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

void PhysicsEvent::onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

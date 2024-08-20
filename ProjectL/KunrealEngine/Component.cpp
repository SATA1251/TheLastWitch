#include "Component.h"

KunrealEngine::Component::Component()
	:_ownerObject(nullptr), _isActivated(true), _isUnique(false), _componentName("")
{

}

KunrealEngine::Component::~Component()
{

}

void KunrealEngine::Component::SetOwner(GameObject* object)
{
	_ownerObject = object;
}

KunrealEngine::GameObject* KunrealEngine::Component::GetOwner()
{
	return this->_ownerObject;
}

bool KunrealEngine::Component::GetActivated()
{
	return this->_isActivated;
}

std::string KunrealEngine::Component::GetComponentName()
{
	return this->_componentName;
}

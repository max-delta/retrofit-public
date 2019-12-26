#include "stdafx.h"
#include "ComponentRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ObjectRef.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

bool ComponentRef::IsSet() const
{
	return mManager != nullptr;
}



ObjectRef ComponentRef::GetObject() const
{
	return ObjectManager::ConversionHelpers::CreateObjectRef( *this );
}



ResolvedComponentType ComponentRef::GetComponentType() const
{
	return mComponentType;
}

///////////////////////////////////////////////////////////////////////////////

bool MutableComponentRef::IsSet() const
{
	return mManager != nullptr;
}



MutableObjectRef MutableComponentRef::GetObject() const
{
	return ObjectManager::ConversionHelpers::CreateMutableObjectRef( *this );
}



ResolvedComponentType MutableComponentRef::GetComponentType() const
{
	return mComponentType;
}



MutableComponentRef::operator ComponentRef() const
{
	return ObjectManager::ConversionHelpers::CreateComponentRef( *this );
}

///////////////////////////////////////////////////////////////////////////////
}}

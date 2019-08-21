#include "stdafx.h"
#include "ComponentRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ObjectRef.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

ObjectRef ComponentRef::GetObject() const
{
	return ObjectManager::ConversionHelpers::CreateObjectRef( *this );
}

///////////////////////////////////////////////////////////////////////////////

MutableObjectRef MutableComponentRef::GetObject() const
{
	return ObjectManager::ConversionHelpers::CreateMutableObjectRef( *this );
}



MutableComponentRef::operator ComponentRef() const
{
	return ObjectManager::ConversionHelpers::CreateComponentRef( *this );
}

///////////////////////////////////////////////////////////////////////////////
}}

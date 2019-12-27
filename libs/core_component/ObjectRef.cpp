#include "stdafx.h"
#include "ObjectRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ComponentRef.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

ObjectIdentifier ObjectRef::GetIdentifier() const
{
	return mIdentifier;
}



ComponentRef ObjectRef::GetComponent( ResolvedComponentType componentType ) const
{
	return ObjectManager::ConversionHelpers::CreateComponentRef( *this, componentType );
}

///////////////////////////////////////////////////////////////////////////////

ObjectIdentifier MutableObjectRef::GetIdentifier() const
{
	return mIdentifier;
}



MutableComponentRef MutableObjectRef::GetComponent( ResolvedComponentType componentType ) const
{
	return ObjectManager::ConversionHelpers::CreateMutableComponentRef( *this, componentType );
}



MutableComponentRef MutableObjectRef::AddComponent( ResolvedComponentType componentType, ComponentInstance&& instance ) const
{
	return mManager->AddComponent( mIdentifier, componentType, rftl::move( instance ) );
}



MutableObjectRef::operator ObjectRef() const
{
	return ObjectManager::ConversionHelpers::CreateObjectRef( *this );
}

///////////////////////////////////////////////////////////////////////////////
}}

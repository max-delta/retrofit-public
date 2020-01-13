#include "stdafx.h"
#include "ObjectRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ComponentRef.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

bool ObjectRef::IsSet() const
{
	return mManager != nullptr;
}



ObjectIdentifier ObjectRef::GetIdentifier() const
{
	return mIdentifier;
}



ComponentRef ObjectRef::GetComponent( ResolvedComponentType componentType ) const
{
	return ObjectManager::ConversionHelpers::CreateComponentRef( *this, componentType );
}



ComponentInstanceRef ObjectRef::GetComponentInstance( ResolvedComponentType componentType ) const
{
	return mManager->GetComponentInstance( GetComponent( componentType ) );
}



bool ObjectRef::operator==( ObjectRef const& rhs ) const
{
	return mManager == rhs.mManager && mIdentifier == rhs.mIdentifier;
}

///////////////////////////////////////////////////////////////////////////////

bool MutableObjectRef::IsSet() const
{
	return mManager != nullptr;
}



ObjectIdentifier MutableObjectRef::GetIdentifier() const
{
	return mIdentifier;
}



MutableComponentRef MutableObjectRef::GetComponent( ResolvedComponentType componentType ) const
{
	return ObjectManager::ConversionHelpers::CreateMutableComponentRef( *this, componentType );
}



ComponentInstanceRef MutableObjectRef::GetComponentInstance( ResolvedComponentType componentType ) const
{
	return mManager->GetComponentInstance( GetComponent( componentType ) );
}



MutableComponentInstanceRef MutableObjectRef::GetMutableComponentInstance( ResolvedComponentType componentType ) const
{
	return mManager->GetMutableComponentInstance( GetComponent( componentType ) );
}



MutableComponentRef MutableObjectRef::AddComponent( ResolvedComponentType componentType, ComponentInstance&& instance ) const
{
	return mManager->AddComponent( mIdentifier, componentType, rftl::move( instance ) );
}



MutableObjectRef::operator ObjectRef() const
{
	return ObjectManager::ConversionHelpers::CreateObjectRef( *this );
}



bool MutableObjectRef::operator==( ObjectRef const& rhs ) const
{
	return operator ObjectRef().operator==( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "ComponentRef.h"

#include "core_component/ObjectManager.h"
#include "core_component/ObjectRef.h"

#include "core/ptr/weak_ptr.h"


namespace RF::component {
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



ComponentInstanceRef ComponentRef::GetComponentInstance() const
{
	return mManager->GetComponentInstance( *this );
}



bool ComponentRef::operator==( ComponentRef const& rhs ) const
{
	return mManager == rhs.mManager && mIdentifier == rhs.mIdentifier && mComponentType == rhs.mComponentType;
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



ComponentInstanceRef MutableComponentRef::GetComponentInstance() const
{
	return mManager->GetComponentInstance( *this );
}



MutableComponentInstanceRef MutableComponentRef::GetMutableComponentInstance() const
{
	return mManager->GetMutableComponentInstance( *this );
}



MutableComponentRef::operator ComponentRef() const
{
	return ObjectManager::ConversionHelpers::CreateComponentRef( *this );
}



bool MutableComponentRef::operator==( ComponentRef const& rhs ) const
{
	return operator ComponentRef().operator==( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}

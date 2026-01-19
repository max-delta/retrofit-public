#pragma once
#include "ContainerManager.h"

#include "core_rftype/VirtualPtrCast.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline WeakPtr<T> ContainerManager::GetMutableControllerAs( ContainerID containerID )
{
	WeakPtr<Controller> controller = GetMutableController( containerID );
	WeakPtr<T> const retVal = rftype::virtual_ptr_cast<T>( rftl::move( controller ) );
	return retVal;
}



template<typename T>
inline WeakPtr<T> ContainerManager::GetMutableControllerAs( rftl::string_view label )
{
	WeakPtr<Controller> controller = GetMutableController( label );
	WeakPtr<T> const retVal = rftype::virtual_ptr_cast<T>( rftl::move( controller ) );
	return retVal;
}



template<typename T>
inline WeakPtr<T> ContainerManager::AssignStrongController( ContainerID containerID, CreationPayload<T>&& controller )
{
	return AssignStrongController( GetMutableContainer( containerID ), rftl::move( controller ) );
}



template<typename T>
inline WeakPtr<T> ContainerManager::AssignStrongController( ContainerID containerID, UniquePtr<T>&& controller )
{
	return AssignStrongController( GetMutableContainer( containerID ), rftl::move( controller ) );
}



template<typename T>
inline WeakPtr<T> ContainerManager::AssignStrongController( Container& container, CreationPayload<T>&& controller )
{
	UniquePtr<T> temp = rftl::move( controller );
	WeakPtr<T> const retVal = AssignStrongController<T>( container, rftl::move( temp ) );
	return retVal;
}



template<typename T>
inline WeakPtr<T> ContainerManager::AssignStrongController( Container& container, UniquePtr<T>&& controller )
{
	WeakPtr<T> const retVal = controller;
	UniquePtr<Controller> temp = rftl::move( controller );
	WeakPtr<Controller> const result = AssignStrongControllerInternal( container, rftl::move( temp ) );
	RF_ASSERT( retVal == result );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

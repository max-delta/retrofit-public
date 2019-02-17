#pragma once
#include "ContainerManager.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

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
}}

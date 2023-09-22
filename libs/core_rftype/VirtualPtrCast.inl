#pragma once
#include "VirtualPtrCast.h"

#include "core_rftype/VirtualCast.h"

#include "core/ptr/ptr_transform.h"

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename TargetT, typename SourceT>
WeakPtr<TargetT> virtual_ptr_cast( WeakPtr<SourceT>&& source )
{
	SourceT* const rawSource = source;
	if( rawSource == nullptr )
	{
		// Source is null
		source = nullptr;
		return nullptr;
	}

	TargetT* const rawTarget = virtual_cast<TargetT*>( rawSource );
	if( rawTarget == nullptr )
	{
		// Unable to cast
		source = nullptr;
		return nullptr;
	}

	if( reinterpret_cast<void const*>( rawTarget ) != reinterpret_cast<void const*>( rawSource ) )
	{
		// Causes an offset, unsafe transormation
		RF_DBGFAIL_MSG( "Offset found during cast, review inheritance hierarchy" );
		source = nullptr;
		return nullptr;
	}

	WeakPtr<TargetT> retVal;
	PtrTransformer<TargetT>::PerformNonTypesafeTransformation( rftl::move( source ), retVal );

	return retVal;
}



template<typename TargetT, typename SourceT>
UniquePtr<TargetT> virtual_ptr_cast( UniquePtr<SourceT>&& source )
{
	SourceT* const rawSource = source;
	if( rawSource == nullptr )
	{
		// Source is null
		source = nullptr;
		return nullptr;
	}

	TargetT* const rawTarget = virtual_cast<TargetT*>( rawSource );
	if( rawTarget == nullptr )
	{
		// Unable to cast
		source = nullptr;
		return nullptr;
	}

	if( reinterpret_cast<void const*>( rawTarget ) != reinterpret_cast<void const*>( rawSource ) )
	{
		// Causes an offset, unsafe transormation
		RF_DBGFAIL_MSG( "Offset found during cast, review inheritance hierarchy" );
		source = nullptr;
		return nullptr;
	}

	UniquePtr<TargetT> retVal;
	PtrTransformer<TargetT>::PerformNonTypesafeTransformation( rftl::move( source ), retVal );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

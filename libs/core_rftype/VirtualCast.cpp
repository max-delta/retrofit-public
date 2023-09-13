#include "stdafx.h"
#include "VirtualCast.h"

#include "core/macros.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/ptr_transform.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

reflect::VirtualClassWithoutDestructor const* virtual_reflect_cast(
	reflect::ClassInfo const& classInfo,
	void const* classLocation )
{
	if( classLocation == nullptr )
	{
		// Trivial case
		// NOTE: Avoids cost of accessing inheritance data and hopping out to
		//  pointer transformation shims
		return nullptr;
	}

	reflect::VirtualRootInfo const& rootInfo = classInfo.mVirtualRootInfo;
	if( rootInfo.mDerivesFromVirtualClassWithoutDestructor == false )
	{
		// Doesn't have a virtual root
		RF_ASSERT( rootInfo.mDerivesFromVirtualClass == false );
		return nullptr;
	}

	if( rootInfo.mGetRootNonDestructingPointerFromCurrent == nullptr )
	{
		// Trivial, no transformation required
		RF_ASSERT( rootInfo.mGetRootPointerFromCurrent == nullptr );
		return reinterpret_cast<reflect::VirtualClassWithoutDestructor const*>( classLocation );
	}
	else
	{
		// Pointer transformation might be required
		return rootInfo.mGetRootNonDestructingPointerFromCurrent( classLocation );
	}
}



reflect::VirtualClassWithoutDestructor* virtual_reflect_cast(
	reflect::ClassInfo const& classInfo,
	void* classLocation )
{
	return const_cast<reflect::VirtualClassWithoutDestructor*>(
		virtual_reflect_cast(
			classInfo,
			const_cast<void const*>( classLocation ) ) );
}



UniquePtr<reflect::VirtualClassWithoutDestructor const> virtual_reflect_ptr_cast(
	reflect::ClassInfo const& classInfo,
	UniquePtr<void const>&& classLocation )
{
	if( classLocation == nullptr )
	{
		// Trivial case
		// NOTE: Avoids cost of accessing inheritance data and hopping out to
		//  pointer transformation shims
		return nullptr;
	}

	reflect::VirtualRootInfo const& rootInfo = classInfo.mVirtualRootInfo;
	if( rootInfo.mDerivesFromVirtualClassWithoutDestructor == false )
	{
		// Doesn't have a virtual root
		RF_ASSERT( rootInfo.mDerivesFromVirtualClass == false );
		return nullptr;
	}

	if( rootInfo.mGetRootNonDestructingPointerFromCurrent == nullptr )
	{
		// Trivial, no transformation required
		RF_ASSERT( rootInfo.mGetRootPointerFromCurrent == nullptr );
		UniquePtr<reflect::VirtualClassWithoutDestructor const> retVal;
		PtrTransformer<reflect::VirtualClassWithoutDestructor const>::PerformNonTypesafeTransformation( rftl::move( classLocation ), retVal );
		return retVal;
	}
	else
	{
		// Pointer transformation might be required

		// It might still be trivial after all, but the PtrTransformer can't
		//  figure that out on its own, as it explicitly ignores that
		void const* const initial = classLocation;
		reflect::VirtualClassWithoutDestructor const* const transformed =
			rootInfo.mGetRootNonDestructingPointerFromCurrent( initial );
		void const* const transformedTypeless = transformed;
		bool const seemsToBeTrvial = transformedTypeless == initial;

		if( seemsToBeTrvial )
		{
			UniquePtr<reflect::VirtualClassWithoutDestructor const> retVal;
			PtrTransformer<reflect::VirtualClassWithoutDestructor const>::PerformNonTypesafeTransformation( rftl::move( classLocation ), retVal );
			return retVal;
		}
		else
		{
			// At this point, you're asking to cause a pointer offset on the
			//  stored pointer value, which is strictly prohibited, as the
			//  address of the pointer needs to remain unchanged so that the
			//  deletion function can operate correctly, as the deletion
			//  functions get the pointers as void, and so can't tell if
			//  they've been messed with
			// NOTE: Theoretically, the pointer storage could also keep around
			//  an offset concept that would let them track that they've been
			//  messed with, and undo that before deletion, but that's a lot
			//  of work for what is a really stupid use case, since this really
			//  only shows up with really terrible multiple inheritance
			RF_TODO_BREAK_MSG( "Support doing really stupid things with really stupid types?" );
			return nullptr;
		}
	}
}



UniquePtr<reflect::VirtualClassWithoutDestructor> virtual_reflect_ptr_cast(
	reflect::ClassInfo const& classInfo,
	UniquePtr<void>&& classLocation )
{
	UniquePtr<reflect::VirtualClassWithoutDestructor> retVal;
	PtrTransformer<reflect::VirtualClassWithoutDestructor>::PerformConstDroppingTransformation(
		virtual_reflect_ptr_cast(
			classInfo,
			UniquePtr<void const>( rftl::move( classLocation ) ) ),
		retVal );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

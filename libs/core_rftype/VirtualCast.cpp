#include "stdafx.h"
#include "VirtualCast.h"

#include "core/macros.h"


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

///////////////////////////////////////////////////////////////////////////////
}

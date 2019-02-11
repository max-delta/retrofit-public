#pragma once
#include "core/macros.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Sometimes you want to do something when a scope ends, regardless of which
//  branches it may have taken, but you only want to write the code once
// EXAMPLE:
//  auto const onScopeEnd = RF::OnScopeEnd( [handle]()
//  {
//    handle->Cleanup();
//  } );
template<typename CALLABLE>
struct OnScopeEnd
{
	RF_NO_COPY( OnScopeEnd );
	RF_NO_MOVE( OnScopeEnd );

	OnScopeEnd( CALLABLE const& callable )
		: mCallable( callable )
	{
		//
	}

	OnScopeEnd( CALLABLE&& callable )
		: mCallable( rftl::move( callable ) )
	{
		//
	}

	~OnScopeEnd()
	{
		mCallable();
	}

	CALLABLE const mCallable;
};

///////////////////////////////////////////////////////////////////////////////
}

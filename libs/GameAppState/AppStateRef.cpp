#include "stdafx.h"
#include "AppStateRef.h"

#include "GameAppState/AppState.h"


namespace RF { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

void AppStateRef::OnEnter( AppStateChangeContext& context )
{
	RF_ASSERT( mAppState != nullptr );
	mAppState->OnEnter( context );
}



void AppStateRef::OnExit( AppStateChangeContext& context )
{
	RF_ASSERT( mAppState != nullptr );
	mAppState->OnExit( context );
}



void AppStateRef::OnTick( AppStateTickContext& context )
{
	RF_ASSERT( mAppState != nullptr );
	mAppState->OnTick( context );
}

///////////////////////////////////////////////////////////////////////////////
}}

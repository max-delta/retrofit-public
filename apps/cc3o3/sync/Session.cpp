#include "stdafx.h"
#include "Session.h"

#include "GameSync/SessionClientManager.h"
#include "GameSync/SessionHostManager.h"


namespace RF::cc::sync {
///////////////////////////////////////////////////////////////////////////////

// Session manager storage / access
WeakPtr<sync::SessionManager> gSessionManager;
WeakPtr<sync::SessionHostManager> gSessionHostManager;
WeakPtr<sync::SessionClientManager> gSessionClientManager;
static UniquePtr<sync::SessionHostManager> sSessionHostManager;
static UniquePtr<sync::SessionClientManager> sSessionClientManager;

///////////////////////////////////////////////////////////////////////////////

void SetSessionHostManager( UniquePtr<sync::SessionHostManager>&& manager )
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager == nullptr );
	RF_ASSERT( sSessionClientManager == nullptr );
	sSessionHostManager = rftl::move( manager );
	gSessionHostManager = sSessionHostManager;
	gSessionManager = sSessionHostManager;
}



void SetSessionClientManager( UniquePtr<sync::SessionClientManager>&& manager )
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager == nullptr );
	RF_ASSERT( sSessionClientManager == nullptr );
	sSessionClientManager = rftl::move( manager );
	gSessionClientManager = sSessionClientManager;
	gSessionManager = sSessionClientManager;
}



void ClearSessionManager()
{
	RF_ASSERT( gSessionManager == sSessionHostManager || gSessionManager == sSessionClientManager );
	RF_ASSERT( gSessionHostManager == sSessionHostManager );
	RF_ASSERT( gSessionClientManager == sSessionClientManager );
	RF_ASSERT( sSessionHostManager != nullptr || sSessionClientManager != nullptr );
	gSessionManager = nullptr;
	gSessionHostManager = nullptr;
	gSessionClientManager = nullptr;
	sSessionHostManager = nullptr;
	sSessionClientManager = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

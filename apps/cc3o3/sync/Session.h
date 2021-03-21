#pragma once
#include "project.h"

#include "cc3o3/sync/SyncFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"


// Forwards
namespace RF::sync {
class SessionManager;
class SessionHostManager;
class SessionClientManager;
}

namespace RF::cc::sync {
///////////////////////////////////////////////////////////////////////////////

// Session manager access
// NOTE: May not always be set
// NOTE: Inheritance and aliasing used for various pointer types
extern WeakPtr<sync::SessionManager> gSessionManager;
extern WeakPtr<sync::SessionHostManager> gSessionHostManager;
extern WeakPtr<sync::SessionClientManager> gSessionClientManager;

void SetSessionHostManager( UniquePtr<sync::SessionHostManager>&& manager );
void SetSessionClientManager( UniquePtr<sync::SessionClientManager>&& manager );
void ClearSessionManager();

///////////////////////////////////////////////////////////////////////////////
}

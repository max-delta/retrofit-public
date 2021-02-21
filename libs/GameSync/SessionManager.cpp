#include "stdafx.h"
#include "SessionManager.h"

#include "Communication/EndpointManager.h"

#include "core/ptr/default_creator.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

SessionManager::SessionManager()
	: mEndpointManager( DefaultCreator<comm::EndpointManager>::Create() )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}

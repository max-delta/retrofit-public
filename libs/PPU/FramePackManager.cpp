#include "stdafx.h"
#include "FramePackManager.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

FramePackManager::FramePackManager()
	: ResourceManagerType()
{
	//
}



FramePackManager::~FramePackManager()
{
	InternalShutdown();
}



UniquePtr<FramePackManager::ResourceType> FramePackManager::AllocateResourceFromFile( Filename const & filename )
{
	RF_DBGFAIL_MSG( "TODO" );
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

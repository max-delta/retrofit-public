#include "stdafx.h"
#include "FramePackManager.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

FramePackManager::FramePackManager()
	: ResourceManagerType()
{
	//
}



UniquePtr<FramePackManager::ResourceType> FramePackManager::AllocateResourceFromFile( Filename const & filename )
{
	RF_ASSERT_MSG( false, "TODO" );
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

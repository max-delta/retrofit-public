#include "stdafx.h"
#include "cc3o3.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PlatformFilesystem/VFS.h"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	// VFS setup for game data
	file::VFSPath const mountFile = file::VFS::kRoot.GetChild( "config", "vfs_cc3o3.ini" );
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( mountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load cc3o3 mount file" );
	}

	// TODO
}



void ProcessFrame()
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}

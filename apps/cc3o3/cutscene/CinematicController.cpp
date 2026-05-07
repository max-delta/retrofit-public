#include "stdafx.h"
#include "CinematicController.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameDialogue/DialogueLoader.h"

#include "PlatformFilesystem/VFS.h"

#include "core_vfs/FileBuffer.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::cutscene {
///////////////////////////////////////////////////////////////////////////////

CinematicController::CinematicController() = default;



bool CinematicController::LoadDialogueSequence( file::VFSPath const& filePath )
{
	// Reset
	mDialog = {};

	file::VFS& vfs = *app::gVfs;

	// Read file
	file::SeekHandlePtr const handle = vfs.GetFileForRead( filePath );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( filePath, RFCAT_CC3O3, "Failed to find dialogue file" );
		return false;
	}
	file::FileBuffer const buffer{ *handle, false };

	// Load
	dialogue::DialogueSequence tempSeq = dialogue::DialogueLoader::Parse( buffer.GetChars() );
	if( tempSeq.mEntries.empty() )
	{
		RFLOG_NOTIFY( filePath, RFCAT_CC3O3, "Failed to load dialogue file, or it has no valid entries" );
		return false;
	}

	// Assign
	mDialog = DefaultCreator<dialogue::DialogueSequence>::Create( rftl::move( tempSeq ) );

	RFLOG_INFO( filePath, RFCAT_CC3O3, "Loaded dialogue file for cinematic" );
	return true;
}



WeakPtr<dialogue::DialogueSequence const> CinematicController::GetDialogue() const
{
	return mDialog;
}

///////////////////////////////////////////////////////////////////////////////
}

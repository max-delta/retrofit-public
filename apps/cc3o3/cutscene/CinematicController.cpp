#include "stdafx.h"
#include "CinematicController.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameDialogue/DialogueLoader.h"
#include "GameDialogue/DialogueSequence.h"
#include "GameNovel/CinematicDriver.h"

#include "PlatformFilesystem/VFS.h"

#include "core_vfs/FileBuffer.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::cutscene {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static dialogue::DialogueSequence CreateFallbackSequence()
{
	dialogue::DialogueSequence retVal = {};
	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

CinematicController::CinematicController()
	: mFallbackDialogue(
		  DefaultCreator<dialogue::DialogueSequence const>::Create(
			  details::CreateFallbackSequence() ) )
	, mDriver(
		  DefaultCreator<novel::CinematicDriver>::Create(
			  mFallbackDialogue ) )
{
	//
}



bool CinematicController::LoadDialogueSequence( file::VFSPath const& filePath )
{
	// Reset
	mDialogue = {};
	mDriver->ChangeSequence( mFallbackDialogue );

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
	mDialogue = DefaultCreator<dialogue::DialogueSequence>::Create( rftl::move( tempSeq ) );
	mDriver->ChangeSequence( mDialogue );

	RFLOG_INFO( filePath, RFCAT_CC3O3, "Loaded dialogue file for cinematic" );
	return true;
}



dialogue::DialogueSequence const& CinematicController::GetDialogue() const
{
	if( mDialogue != nullptr )
	{
		return *mDialogue;
	}
	return *mFallbackDialogue;
}



novel::CinematicDriver const& CinematicController::GetDriver() const
{
	return *mDriver;
}



novel::CinematicDriver& CinematicController::GetMutableDriver()
{
	return *mDriver;
}

///////////////////////////////////////////////////////////////////////////////
}

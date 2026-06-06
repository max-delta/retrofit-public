#include "stdafx.h"
#include "CinematicController.h"

// HACK: Used just for temp fallback frame packs
RF_TODO_ANNOTATION( "Remove this fallback" );
#include "cc3o3/ui/standard/StandardUIElements.h"

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



static novel::CinematicDriver::FramePacksByCharacter AssignFramePacksForRequiredCharacters(
	dialogue::DialogueSequence::StringViewMultiMap const& requiredExpressionsPerCharacter,
	int TODO )
{
	using ExpressionsPerCharacter = dialogue::DialogueSequence::StringViewMultiMap;
	using Expressions = dialogue::DialogueSequence::StringViewSet;

	novel::CinematicDriver::FramePacksByCharacter retVal = {};

	RF_TODO_ANNOTATION( "Use actual characters" );
	( (void)TODO );

	// For each required character...
	for( ExpressionsPerCharacter::value_type const& requiredExpressionsEntry : requiredExpressionsPerCharacter )
	{
		rftl::string_view const& character = requiredExpressionsEntry.first;
		Expressions const& expressions = requiredExpressionsEntry.second;

		// For each required expression...
		for( rftl::string_view const& expression : expressions )
		{
			// HACK: Set a fake entry
			gfx::ppu::PPUController const& ppu = *app::gGraphics;
			ui::FramePackDef const portraitFPack = ui::QueryFramePackDef( ppu, ui::mockup::kWiggle64 );
			retVal[character][expression] = portraitFPack.mRef;
		}
	}

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
			  novel::CinematicDriver::SequenceParams{
				  .mSequence = mFallbackDialogue,
				  .mFramePacksByCharacter = {} } ) )
{
	//
}



bool CinematicController::SetCharacterData( file::VFSPath const& characterRoot )
{
	RF_TODO_ANNOTATION( "Load characters from folder" );
	( (void)characterRoot );

	return true;
}



bool CinematicController::SetSceneData( file::VFSPath const& sceneRoot )
{
	RF_TODO_ANNOTATION( "Load scenes from folder" );
	( (void)sceneRoot );

	return true;
}



bool CinematicController::LoadDialogueSequence( file::VFSPath const& filePath )
{
	// Reset
	mDialogue = {};
	mDriver->ChangeSequence(
		novel::CinematicDriver::SequenceParams{
			.mSequence = mFallbackDialogue,
			.mFramePacksByCharacter = {} } );

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
	UniquePtr<dialogue::DialogueSequence const> loadedSeq;
	{
		dialogue::DialogueSequence tempSeq = dialogue::DialogueLoader::Parse( buffer.GetChars() );
		if( tempSeq.mEntries.empty() )
		{
			RFLOG_NOTIFY( filePath, RFCAT_CC3O3, "Failed to load dialogue file, or it has no valid entries" );
			return false;
		}
		loadedSeq = DefaultCreator<dialogue::DialogueSequence>::Create( rftl::move( tempSeq ) );
	}

	// Assign
	mDialogue = rftl::move( loadedSeq );
	mDriver->ChangeSequence(
		novel::CinematicDriver::SequenceParams{
			.mSequence = mDialogue,
			.mFramePacksByCharacter =
				details::AssignFramePacksForRequiredCharacters(
					mDialogue->mRequiredExpressionsPerCharacter,
					-5 ) } );

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

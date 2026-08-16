#include "stdafx.h"
#include "CinematicController.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameDialogue/DialogueLoader.h"
#include "GameDialogue/DialogueSequence.h"
#include "GameNovel/CinematicDriver.h"

#include "PPU/FramePackManager.h"
#include "PPU/PPUController.h"
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



static void UnloadAllFramePacks( rftl::deque<rftl::string>& resourceNames )
{
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	// HACK: Direct access to frame pack manager
	// TODO: Re-visit API surface
	gfx::ppu::FramePackManager& fpackMan = *ppu.DebugGetFramePackManager();

	for( rftl::string const& resourceName : resourceNames )
	{
		fpackMan.DestroyResource( resourceName );
	}

	resourceNames.clear();
}



static novel::CinematicDriver::FramePacksByCharacter AssignFramePacksForRequiredCharacters(
	dialogue::DialogueSequence::StringViewMultiMap const& requiredExpressionsPerCharacter,
	file::VFSPath const& characterRoot,
	rftl::deque<rftl::string>& resourceNames )
{
	RF_ASSERT( resourceNames.empty() );

	using ExpressionsPerCharacter = dialogue::DialogueSequence::StringViewMultiMap;
	using Expressions = dialogue::DialogueSequence::StringViewSet;

	novel::CinematicDriver::FramePacksByCharacter retVal = {};

	gfx::ppu::PPUController& ppu = *app::gGraphics;
	gfx::ppu::FramePackManager const& framePackMan = *ppu.GetFramePackManager();

	// For each required character...
	for( ExpressionsPerCharacter::value_type const& requiredExpressionsEntry : requiredExpressionsPerCharacter )
	{
		rftl::string_view const& character = requiredExpressionsEntry.first;
		Expressions const& expressions = requiredExpressionsEntry.second;

		// For each required expression...
		for( rftl::string_view const& expression : expressions )
		{
			if( expression == novel::kNullExpression )
			{
				// Expression unused
				// NOTE: Will still cause the character to be present, to
				//  indicate that they were atleast considered
				retVal[character];
				continue;
			}

			resourceNames.emplace_back( rftl::format( "CINEMATIC_EXPRESSION/{}/{}", character, expression ) );
			rftl::string_view const resourceName = resourceNames.back();
			file::VFSPath const filename = characterRoot.GetChild( rftl::string( character ), rftl::string( expression ) + ".fpack" );
			bool const success = ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, resourceName, filename );
			if( success == false )
			{
				RFLOG_NOTIFY( filename, RFCAT_CC3O3, "Failed to load an expression '{}' for character '{}'", expression, character );
				continue;
			}

			gfx::ppu::ManagedFramePackID const framePackID = framePackMan.GetManagedResourceIDFromResourceName( resourceName );
			WeakPtr<gfx::ppu::FramePackBase const> const framePack = framePackMan.GetResourceFromManagedResourceID( framePackID );
			RF_ASSERT( framePack != nullptr );

			retVal[character][expression] = gfx::ppu::FramePackRef::FromFramePack( framePackID, *framePack );
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
	mCharacterRoot = characterRoot;
	return true;
}



bool CinematicController::SetSceneData( file::VFSPath const& sceneRoot )
{
	mSceneRoot = sceneRoot;
	return true;
}



bool CinematicController::LoadDialogueSequence( file::VFSPath const& filePath )
{
	// Unload resources
	details::UnloadAllFramePacks( mLoadedExpressionFramePackResourceNames );

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
					mCharacterRoot,
					mLoadedExpressionFramePackResourceNames ) } );

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

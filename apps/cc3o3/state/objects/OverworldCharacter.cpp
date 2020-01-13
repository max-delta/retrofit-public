#include "stdafx.h"
#include "OverworldCharacter.h"

#include "cc3o3/Common.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/OverworldVisual.h"
#include "cc3o3/state/components/OverworldMovement.h"
#include "cc3o3/state/StateLogging.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"

namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateOverworldCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeOverworldCharacterFromDB( sharedWindow, privateWindow, ref, charID );
	return ref;
}



void MakeOverworldCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// Movement
	{
		MutableComponentInstanceRefT<comp::OverworldMovement> const move =
			ref.AddComponentInstanceT<comp::OverworldMovement>(
				DefaultCreator<comp::OverworldMovement>::Create() );
		RFLOG_TEST_AND_FATAL( move != nullptr, ref, RFCAT_CC3O3, "Failed to add overworld movement component" );
		move->BindToMeta( sharedWindow, privateWindow, ref );
	}

	// Visual
	{
		MutableComponentInstanceRefT<comp::OverworldVisual> const visual =
			ref.AddComponentInstanceT<comp::OverworldVisual>(
				DefaultCreator<comp::OverworldVisual>::Create() );
		RFLOG_TEST_AND_FATAL( visual != nullptr, ref, RFCAT_CC3O3, "Failed to add overworld visual component" );
		visual->BindToMeta( sharedWindow, privateWindow, ref );

		character::CharacterDatabase const& charDB = *gCharacterDatabase;
		sprite::CompositeCharacter const composite = charDB.FetchExistingComposite( charID );
		RFLOG_TEST_AND_FATAL( composite.mCharacterSequenceType == sprite::CharacterSequenceType::N3_E3_S3_W3, ref, RFCAT_CC3O3, "Unsupported sequence" );
		file::VFSPath const nWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::NWalk );
		file::VFSPath const eWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::EWalk );
		file::VFSPath const sWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::SWalk );
		file::VFSPath const wWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::WWalk );

		gfx::PPUController& ppu = *app::gGraphics;
		gfx::FramePackManager const& fPackMan = *ppu.GetFramePackManager();
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, nWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, eWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, sWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, wWalk );
		auto setAnim = [&fPackMan]( comp::OverworldVisual::Anim& anim, file::VFSPath const& source ) -> void //
		{
			anim.mFramePackID = fPackMan.GetManagedResourceIDFromResourceName( source );
			gfx::FramePackBase const& fPack = *fPackMan.GetResourceFromResourceName( source );
			anim.mSlowdownRate = fPack.mPreferredSlowdownRate;
			anim.mMaxTimeIndex = fPack.CalculateTimeIndexBoundary();
			RF_ASSERT_MSG( anim.mMaxTimeIndex == 4, "Unexpected format" );
		};
		setAnim( visual->mWalkNorth, nWalk );
		setAnim( visual->mWalkEast, eWalk );
		setAnim( visual->mWalkSouth, sWalk );
		setAnim( visual->mWalkWest, wWalk );

		// TODO: Different format, with dedicated idle framepacks
		setAnim( visual->mIdleNorth, nWalk );
		setAnim( visual->mIdleEast, eWalk );
		setAnim( visual->mIdleSouth, sWalk );
		setAnim( visual->mIdleWest, wWalk );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as overworld character" );
}

///////////////////////////////////////////////////////////////////////////////
}

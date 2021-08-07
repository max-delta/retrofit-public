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
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeOverworldCharacterFromDB( sharedWindow, privateWindow, ref, charID );
	return ref;
}



void MakeOverworldCharacterFromDB(
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	rftl::immutable_string const& charID )
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
		sprite::CompositeCharacter const composite = charDB.FetchExistingComposite( charID, "24" );
		RF_TODO_ANNOTATION( "Clean up this logic and make it better at checking data integrity" );
		file::VFSPath const nIdle = composite.mFramepacksByAnim.at( "idle_n" );
		file::VFSPath const eIdle = composite.mFramepacksByAnim.at( "idle_e" );
		file::VFSPath const sIdle = composite.mFramepacksByAnim.at( "idle_s" );
		file::VFSPath const wIdle = composite.mFramepacksByAnim.at( "idle_w" );
		file::VFSPath const nWalk = composite.mFramepacksByAnim.at( "walk_n" );
		file::VFSPath const eWalk = composite.mFramepacksByAnim.at( "walk_e" );
		file::VFSPath const sWalk = composite.mFramepacksByAnim.at( "walk_s" );
		file::VFSPath const wWalk = composite.mFramepacksByAnim.at( "walk_w" );

		gfx::ppu::PPUController& ppu = *app::gGraphics;
		gfx::ppu::FramePackManager const& fPackMan = *ppu.GetFramePackManager();
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, nIdle );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, eIdle );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, sIdle );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, wIdle );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, nWalk );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, eWalk );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, sWalk );
		ppu.ForceImmediateLoadRequest( gfx::ppu::PPUController::AssetType::FramePack, wWalk );
		auto setAnim = [&fPackMan]( comp::OverworldVisual::Anim& anim, file::VFSPath const& source ) -> void //
		{
			anim.mFramePackID = fPackMan.GetManagedResourceIDFromResourceName( source );
			gfx::ppu::FramePackBase const& fPack = *fPackMan.GetResourceFromResourceName( source );
			anim.mSlowdownRate = fPack.mPreferredSlowdownRate;
			anim.mMaxTimeIndex = fPack.CalculateTimeIndexBoundary();
			RF_ASSERT_MSG( anim.mMaxTimeIndex == 1 || anim.mMaxTimeIndex == 4, "Unexpected format" );
		};
		setAnim( visual->mIdleNorth, nIdle );
		setAnim( visual->mIdleEast, eIdle );
		setAnim( visual->mIdleSouth, sIdle );
		setAnim( visual->mIdleWest, wIdle );
		setAnim( visual->mWalkNorth, nWalk );
		setAnim( visual->mWalkEast, eWalk );
		setAnim( visual->mWalkSouth, sWalk );
		setAnim( visual->mWalkWest, wWalk );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as overworld character" );
}

///////////////////////////////////////////////////////////////////////////////
}

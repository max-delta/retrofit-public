#include "stdafx.h"
#include "SiteCharacter.h"

#include "cc3o3/Common.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/SiteVisual.h"
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

MutableObjectRef CreateSiteCharacterFromDB(
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeSiteCharacterFromDB( sharedWindow, privateWindow, ref, charID );
	return ref;
}



void MakeSiteCharacterFromDB(
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	rftl::immutable_string const& charID )
{
	// TODO: Movement

	// Visual
	{
		MutableComponentInstanceRefT<comp::SiteVisual> const visual =
			ref.AddComponentInstanceT<comp::SiteVisual>(
				DefaultCreator<comp::SiteVisual>::Create() );
		RFLOG_TEST_AND_FATAL( visual != nullptr, ref, RFCAT_CC3O3, "Failed to add overworld visual component" );
		visual->BindToMeta( sharedWindow, privateWindow, ref );

		character::CharacterDatabase const& charDB = *gCharacterDatabase;
		sprite::CompositeCharacter const composite = charDB.FetchExistingComposite( charID, "36" );
		RFLOG_TEST_AND_FATAL( composite.mCharacterSequenceType == sprite::CharacterSequenceType::P_NESW_i121, ref, RFCAT_CC3O3, "Unsupported sequence" );
		file::VFSPath const nIdle = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::NIdle );
		file::VFSPath const eIdle = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::EIdle );
		file::VFSPath const sIdle = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::SIdle );
		file::VFSPath const wIdle = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::WIdle );
		file::VFSPath const nWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::NWalk );
		file::VFSPath const eWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::EWalk );
		file::VFSPath const sWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::SWalk );
		file::VFSPath const wWalk = composite.mFramepacksByAnim.at( sprite::CharacterAnimKey::WWalk );

		gfx::PPUController& ppu = *app::gGraphics;
		gfx::FramePackManager const& fPackMan = *ppu.GetFramePackManager();
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, nIdle );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, eIdle );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, sIdle );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, wIdle );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, nWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, eWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, sWalk );
		ppu.ForceImmediateLoadRequest( gfx::PPUController::AssetType::FramePack, wWalk );
		auto setAnim = [&fPackMan]( comp::SiteVisual::Anim& anim, file::VFSPath const& source ) -> void //
		{
			anim.mFramePackID = fPackMan.GetManagedResourceIDFromResourceName( source );
			gfx::FramePackBase const& fPack = *fPackMan.GetResourceFromResourceName( source );
			anim.mSlowdownRate = fPack.mPreferredSlowdownRate;
			anim.mMaxTimeIndex = fPack.CalculateTimeIndexBoundary();
			RF_ASSERT_MSG( anim.mMaxTimeIndex == 4, "Unexpected format" );
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

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as site character" );
}

///////////////////////////////////////////////////////////////////////////////
}

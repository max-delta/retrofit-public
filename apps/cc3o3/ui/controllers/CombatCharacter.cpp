#include "stdafx.h"
#include "CombatCharacter.h"

#include "cc3o3/Common.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/combat/Fighter.h"
#include "cc3o3/combat/IdentifierUtils.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Character.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/BorderFrame.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::CombatCharacter )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::CombatCharacter );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

bool CombatCharacter::HasCharacter() const
{
	return mBorderFrame->IsChildRenderingBlocked() == false;
}



void CombatCharacter::ClearCharacter()
{
	mBorderFrame->SetChildRenderingBlocked( true );
}



void CombatCharacter::UpdateCharacter( combat::Fighter const& fighter, state::ObjectRef const& character, bool selected )
{
	RF_ASSERT( character.IsSet() );
	mBorderFrame->SetChildRenderingBlocked( false );

	// Text
	{
		state::comp::Character const& chara = *character.GetComponentInstanceT<state::comp::Character>();
		character::CharData const& charData = chara.mCharData;

		combat::EntityClass const entityClass = combat::ReadEntityClassFromString( charData.mEntityClass );

		combat::CombatEngine const& combatEngine = *gCombatEngine;

		combat::DisplayVal const maxHP = combatEngine.DisplayHealth( fighter.mMaxHealth, entityClass );
		combat::DisplayVal const curHP = combatEngine.DisplayHealth( fighter.mCurHealth, entityClass );
		combat::SignedDisplayVal const curStamina = combatEngine.DisplayStamina( fighter.mCurStamina, entityClass );

		if( selected )
		{
			mBorderFrame->SwapPeerTileset( mActiveTileset );
		}
		else
		{
			mBorderFrame->SwapPeerTileset( mInactiveTileset );
		}

		static constexpr size_t kLineBufSize = 16;
		rftl::array<char, kLineBufSize> lineBuf;

		{
			mInfoRows.at( 0 )->SetText( charData.mDescription.mName );

			mInfoRows.at( 1 )->SetText( "TODO: GRID" );

			snprintf( lineBuf.data(), lineBuf.size(), "%3u/%-3u", curHP, maxHP );
			lineBuf.back() = '\0';
			mInfoRows.at( 2 )->SetText( lineBuf.data() );

			snprintf( lineBuf.data(), lineBuf.size(), "Sta.%01i.0", curStamina );
			lineBuf.back() = '\0';
			mInfoRows.at( 3 )->SetText( lineBuf.data() );
		}
	}
}



void CombatCharacter::OnInstanceAssign( UIContext& context, Container& container )
{
	ui::ContainerManager& uiManager = context.GetMutableContainerManager();
	gfx::ppu::PPUController const& renderer = GetRenderer( uiManager );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

	mActiveTileset = tsetMan.GetManagedResourceIDFromResourceName( "flat2_8_48" );
	mInactiveTileset = tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" );

	mChildContainerID = CreateChildContainer(
		uiManager,
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	// Frame
	mBorderFrame =
		uiManager.AssignStrongController(
			mChildContainerID,
			DefaultCreator<BorderFrame>::Create() );
	mBorderFrame->SetTileset( context, mInactiveTileset, { 8, 8 }, { 48, 48 }, { -4, -4 } );
	mBorderFrame->SetChildRenderingBlocked( true );

	// 4 info rows
	RowSlicer::Ratios const infoRowRatios = {
		{ 4.f / 18.f, true },
		{ 4.f / 18.f, true },
		{ 6.f / 18.f, true },
		{ 4.f / 18.f, true },
	};
	WeakPtr<RowSlicer> const infoRowSlicer =
		uiManager.AssignStrongController(
			mBorderFrame->GetChildContainerID(),
			DefaultCreator<RowSlicer>::Create(
				infoRowRatios ) );

	for( size_t i_info = 0; i_info < 4; i_info++ )
	{
		// Info
		WeakPtr<TextLabel>& info = mInfoRows.at( i_info );
		info =
			uiManager.AssignStrongController(
				infoRowSlicer->GetChildContainerID( i_info ),
				DefaultCreator<TextLabel>::Create() );
		info->SetText( "UNSET" );
		info->SetColor( math::Color3f::kWhite );
		info->SetBorder( true );
	}

	// Name
	{
		WeakPtr<TextLabel> const& info = mInfoRows.at( 0 );
		info->SetJustification( ui::Justification::MiddleLeft );
		info->SetFont( ui::font::BattleName );
	}

	// Grid
	{
		WeakPtr<TextLabel> const& info = mInfoRows.at( 1 );
		info->SetJustification( ui::Justification::MiddleCenter );
		info->SetFont( ui::font::BattleName );
	}

	// Health
	{
		WeakPtr<TextLabel> const& info = mInfoRows.at( 2 );
		info->SetJustification( ui::Justification::MiddleCenter );
		info->SetFont( ui::font::BattleHealth );

		// TODO: Remove this and solve the overlapping issues
		info->SetIgnoreOverflow( true );
	}

	// Stamina
	{
		WeakPtr<TextLabel> const& info = mInfoRows.at( 3 );
		info->SetJustification( ui::Justification::MiddleRight );
		info->SetFont( ui::font::BattleStamina );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

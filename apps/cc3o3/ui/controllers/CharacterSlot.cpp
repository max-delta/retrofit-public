#include "stdafx.h"
#include "CharacterSlot.h"

#include "cc3o3/Common.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/SiteVisual.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/combat/IdentifierUtils.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/FramePackDisplay.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstdio"


RFTYPE_CREATE_META( RF::cc::ui::controller::CharacterSlot )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::CharacterSlot );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

bool CharacterSlot::HasCharacter() const
{
	return mBorderFrame->IsChildRenderingBlocked() == false;
}



void CharacterSlot::ClearCharacter()
{
	mBorderFrame->SetChildRenderingBlocked( true );
}



void CharacterSlot::UpdateCharacter( state::ObjectRef const& character )
{
	RF_ASSERT( character.IsSet() );
	mBorderFrame->SetChildRenderingBlocked( false );

	// Visual
	state::comp::SiteVisual const& visual = *character.GetComponentInstanceT<state::comp::SiteVisual>();
	state::comp::SiteVisual::Anim const& anim = visual.mIdleSouth;
	mCharacterDisplay->SetFramePack(
		anim.mFramePackID,
		anim.mMaxTimeIndex,
		0,
		gfx::ppu::kTileSize / 2 );

	// Text
	{
		state::comp::Character const& chara = *character.GetComponentInstanceT<state::comp::Character>();
		character::CharData const& charData = chara.mCharData;

		combat::EntityClass const entityClass = combat::ReadEntityClassFromString( charData.mEntityClass );

		combat::CombatEngine const& combatEngine = *gCombatEngine;

		auto const dispStat = [&combatEngine, &entityClass]( character::StatValue stat ) -> combat::DisplayVal //
		{
			return combatEngine.DisplayStandardStat( math::integer_cast<combat::SimVal>( stat ), entityClass );
		};
		combat::DisplayVal const physAtk = dispStat( charData.mStats.mPhysAtk );
		combat::DisplayVal const physDef = dispStat( charData.mStats.mPhysDef );
		combat::DisplayVal const elemAtk = dispStat( charData.mStats.mElemAtk );
		combat::DisplayVal const elemDef = dispStat( charData.mStats.mElemDef );
		combat::DisplayVal const balance = dispStat( charData.mStats.mBalance );
		combat::DisplayVal const techniq = dispStat( charData.mStats.mTechniq );
		//combat::DisplayVal const elemPwr = dispStat( charData.mStats.mElemPwr );

		//combat::DisplayVal const hp = combatEngine.DisplayHealth(
		//	combatEngine.LoCalcMaxHealth(
		//		math::integer_cast<combat::SimVal>( charData.mStats.mMHealth ), entityClass ),
		//	entityClass );

		static constexpr size_t kLineBufSize = 16;
		rftl::array<char, kLineBufSize> lineBuf;

		// TODO: Small icons to make more readable room for more stuff
		{
			mInfoRows.at( 0 )->SetText( charData.mDescription.mName );

			//rftl::snprintf( lineBuf.data(), lineBuf.size(), "HP % 3u INN %s", hp, charData.mInnate.c_str() );
			rftl::snprintf( lineBuf.data(), lineBuf.size(), "PAt %2u EAt %2u", physAtk, elemAtk );
			lineBuf.back() = '\0';
			mInfoRows.at( 1 )->SetText( lineBuf.data() );

			rftl::snprintf( lineBuf.data(), lineBuf.size(), "PDf %2u EDf %2u", physDef, elemDef );
			lineBuf.back() = '\0';
			mInfoRows.at( 2 )->SetText( lineBuf.data() );

			rftl::snprintf( lineBuf.data(), lineBuf.size(), "Bal %2u Tec %2u", balance, techniq );
			lineBuf.back() = '\0';
			mInfoRows.at( 3 )->SetText( lineBuf.data() );
		}
	}
}



void CharacterSlot::OnInstanceAssign( UIContext& context, Container& container )
{
	ui::ContainerManager& uiManager = context.GetMutableContainerManager();
	gfx::ppu::PPUController const& renderer = GetRenderer( uiManager );
	gfx::TilesetManager const& tsetMan = *renderer.GetTilesetManager();

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
	mBorderFrame->SetTileset( context, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
	mBorderFrame->SetChildRenderingBlocked( true );

	// 2 subsections for info and display
	ColumnSlicer::Ratios const charSlotColumnRatios = {
		{ 7.f / 10.f, true },
		{ 3.f / 10.f, true },
	};
	WeakPtr<ColumnSlicer> const charSlotColumnSlicer =
		uiManager.AssignStrongController(
			mBorderFrame->GetChildContainerID(),
			DefaultCreator<ColumnSlicer>::Create(
				charSlotColumnRatios ) );

	// 4 info rows
	RowSlicer::Ratios const infoRowRatios = {
		{ 6.f / 18.f, true },
		{ 4.f / 18.f, true },
		{ 4.f / 18.f, true },
		{ 4.f / 18.f, true },
	};
	WeakPtr<RowSlicer> const infoRowSlicer =
		uiManager.AssignStrongController(
			charSlotColumnSlicer->GetChildContainerID( 0 ),
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
		info->SetJustification( ui::Justification::MiddleLeft );
		if( i_info == 0 )
		{
			info->SetFont( ui::font::LargeMenuText );
		}
		else
		{
			info->SetFont( ui::font::NarrowQuarterTileMono );
		}
		info->SetText( "UNSET" );
		info->SetColor( math::Color3f::kWhite );
		info->SetBorder( true );
	}

	// Display
	mCharacterDisplay =
		uiManager.AssignStrongController(
			charSlotColumnSlicer->GetChildContainerID( 1 ),
			DefaultCreator<FramePackDisplay>::Create() );
	mCharacterDisplay->SetJustification( ui::Justification::BottomCenter );
}

///////////////////////////////////////////////////////////////////////////////
}

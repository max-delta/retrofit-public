#include "stdafx.h"
#include "TitleScreen_CharCreate.h"

#include "cc3o3/appstates/titlescreen/TitleScreen.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/char/CharData.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameSprite/CharacterCreator.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/MessageBox.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/TextRows.h"
#include "GameUI/controllers/ListBox.h"
#include "GameUI/controllers/FramePackDisplay.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kPreviewFpackName[] = "charcreate_preview";

static constexpr char kSaveTag[] = "SAVE";

static constexpr char const* kLongTextTags[] = {
	"O_Charname",
	kSaveTag,
};

static constexpr char const* kLeftText[] = {
	"CHARACTERNAME [E]", // THis will be dymanically controlled
	"Innate",
	"Genetics",
	"Phys Atk",
	"Phys Def",
	"Elem Atk",
	"Elem Def",
	"Balance",
	"Techniq",
	"Elem Pwr",
	"Breadth",
	"",
	"",
};

static constexpr char const* kLeftTextTags[rftl::extent<decltype( kLeftText )>::value] = {
	"O_Charname",
	"O_Innate",
	"O_Genetics",
	"O_PhysAtk",
	"O_PhysDef",
	"O_ElemAtk",
	"O_ElemDef",
	"O_Balance",
	"O_Techniq",
	"O_ElemPwr",
	"O_Breadth",
	"O_UNUSED_L1",
	"O_UNUSED_L2",
};

static constexpr char const* kLeftStatusTags[rftl::extent<decltype( kLeftText )>::value] = {
	"S_UNUSED_L1",
	"S_Innate",
	"S_Genetics",
	"S_PhysAtk",
	"S_PhysDef",
	"S_ElemAtk",
	"S_ElemDef",
	"S_Balance",
	"S_Techniq",
	"S_ElemPwr",
	"S_Shape",
	"S_UNUSED_L2",
	"S_UNUSED_L3",
};

static constexpr char const* kRightText[] = {
	"S.Opaci", // Or F.Satur
	"S.PheoM", // Or F.PheoM
	"S.EuMel", // Or F.EuMel
	"H.Satur",
	"H.PheoM",
	"H.EuMel",
	"H.Dye",
	"Cloth",
	"C.Dye",
	"SAVE AND QUIT",
};

static constexpr char const* kRightTextTags[rftl::extent<decltype( kRightText )>::value] = {
	"O_Body1",
	"O_Body2",
	"O_Body3",
	"O_Hair1",
	"O_Hair2",
	"O_Hair3",
	"O_Hair4",
	"O_Cloth1",
	"O_Cloth2",
	kSaveTag,
};

static constexpr char const* kRightStatusTags[rftl::extent<decltype( kRightText )>::value] = {
	"S_Body1",
	"S_Body2",
	"S_Body3",
	"S_Hair1",
	"S_Hair2",
	"S_Hair3",
	"S_Hair4",
	"S_Cloth1",
	"S_Cloth2",
	"S_UNUSED_R1",
};

}
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_CharCreate::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState();

	void UpdateDisplay( ui::ContainerManager& uiManager );
	void HandleModification( ui::ContainerManager const& uiManager, ui::ContainerID const& focusContainerID, bool increase );
	void Recomposite();
	void Load();
	void Save();

	using PlayableGeneticsList = rftl::vector<character::CharacterValidator::GeneticsID>;
	PlayableGeneticsList mPlayableGenetics;

	character::CharData mChar = {};
};



TitleScreen_CharCreate::InternalState::InternalState()
{
	character::CharacterValidator const& charValidate = *gCharacterValidator;

	// Figure out genetics options
	mPlayableGenetics.clear();
	rftl::vector<character::CharacterValidator::GeneticsID> sortedGenetics;
	{
		rftl::unordered_set<character::CharacterValidator::GeneticsID> const allGenetics = charValidate.GetGeneticsIDs();
		sortedGenetics.reserve( allGenetics.size() );
		sortedGenetics.assign( allGenetics.begin(), allGenetics.end() );
		rftl::sort( sortedGenetics.begin(), sortedGenetics.end() );

		// HACK: Reverse
		RF_TODO_ANNOTATION( "Data-driven control of ordering, instead of by alphabetical" );
		// NOTE: Reverse causes 'portal' to be before 'awoken...', so humans
		//  are first in the list
		rftl::reverse( sortedGenetics.begin(), sortedGenetics.end() );
	}
	for( character::CharacterValidator::GeneticsID const& id : sortedGenetics )
	{
		character::GeneticsEntry const& entry = charValidate.GetGeneticEntry( id );
		if( entry.mPlayable )
		{
			mPlayableGenetics.emplace_back( id );
		}
	}
}



void TitleScreen_CharCreate::InternalState::UpdateDisplay( ui::ContainerManager& uiManager )
{
	character::CharacterValidator const& charValidate = *gCharacterValidator;

	rftl::string const& name = mChar.mDescription.mName;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "O_Charname" )->SetText( name + " [E]" );

	static constexpr auto format9 = []( rftl::string const& str ) -> rftl::string //
	{
		rftl::string retVal;
		retVal += "< ";
		retVal += str;
		retVal += " >";
		return retVal;
	};

	static constexpr auto format2 = []( uint8_t val ) -> rftl::string //
	{
		RF_ASSERT( val <= 99 );
		rftl::string retVal;
		retVal += "< ";
		retVal += '0' + ( val / 10 );
		retVal += '0' + ( val % 10 );
		retVal += " >";
		return retVal;
	};

	static constexpr auto format5Pips = []( uint8_t min, uint8_t cur, uint8_t avail ) -> rftl::string //
	{
		RF_ASSERT( min >= 0 );
		RF_ASSERT( min <= 5 );
		RF_ASSERT( min <= cur );
		RF_ASSERT( cur <= 5 );
		uint8_t const max = math::integer_cast<uint8_t>( cur + avail );
		RF_ASSERT( cur <= max );
		rftl::string retVal;
		retVal += "- ";
		for( uint8_t i = 0; i < 5; i++ )
		{
			if( i + 1 < min )
			{
				retVal += "@ ";
			}
			else if( i < min )
			{
				retVal += "@ ";
			}
			else if( i < cur )
			{
				retVal += "O ";
			}
			else if( i < max )
			{
				retVal += ". ";
			}
			else
			{
				retVal += "  ";
			}
		}
		retVal += "+";
		return retVal;
	};

	// TODO
	rftl::string innate = "YYYYYYY";
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Innate" )->SetText( format9( innate ) );

	// Genetics
	character::CharacterValidator::GeneticsID const geneticsID = charValidate.GetGeneticsID( mChar.mGenetics.mSpecies, mChar.mGenetics.mGender );
	rftl::string const& genetics = charValidate.GetGeneticEntry( geneticsID ).m9Char;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Genetics" )->SetText( format9( genetics ) );

	// Stats
	uint8_t const avail = math::integer_cast<uint8_t>(
		character::CharacterValidator::kMaxTotalStatPoints -
		character::CharacterValidator::CalculateTotalPoints( mChar.mStats ) );
	character::Stats const bonuses = charValidate.GetStatBonuses( mChar.mGenetics.mSpecies );
	character::Stats const& stats = mChar.mStats;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_PhysAtk" )->SetText( format5Pips( bonuses.mPhysAtk, stats.mPhysAtk, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_PhysDef" )->SetText( format5Pips( bonuses.mPhysDef, stats.mPhysDef, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemAtk" )->SetText( format5Pips( bonuses.mElemAtk, stats.mElemAtk, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemDef" )->SetText( format5Pips( bonuses.mElemDef, stats.mElemDef, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Balance" )->SetText( format5Pips( bonuses.mBalance, stats.mBalance, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Techniq" )->SetText( format5Pips( bonuses.mTechniq, stats.mTechniq, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemPwr" )->SetText( format5Pips( bonuses.mElemPwr, stats.mElemPwr, avail ) );

	// Breadth vs depth
	character::GridShape const shape = mChar.mStats.mGridShp;
	(void)shape; // TODO
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Shape" )->SetText( format9( "TODO" ) );

	// TODO
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Body1" )->SetText( format2( 5 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Body2" )->SetText( format2( 6 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Body3" )->SetText( format2( 7 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Hair1" )->SetText( format2( 8 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Hair2" )->SetText( format2( 9 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Hair3" )->SetText( format2( 10 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Hair4" )->SetText( format2( 11 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Cloth1" )->SetText( format2( 12 ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Cloth2" )->SetText( format2( 13 ) );
}



void TitleScreen_CharCreate::InternalState::HandleModification( ui::ContainerManager const& uiManager, ui::ContainerID const& focusContainerID, bool increase )
{
	character::CharacterValidator const& charValidate = *gCharacterValidator;

	static constexpr auto statChange = []( character::StatValue& stat, bool increase ) -> void //
	{
		if( stat < 5 && increase )
		{
			stat++;
		}
		else if( stat > 0 && increase == false )
		{
			stat--;
		}
	};

	if( focusContainerID == uiManager.GetContainerID( "O_Charname" ) )
	{
		// TODO
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_Innate" ) )
	{
		// TODO
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_Genetics" ) )
	{
		character::CharacterValidator::GeneticsID const current = charValidate.GetGeneticsID( mChar.mGenetics.mSpecies, mChar.mGenetics.mGender );
		character::CharacterValidator::GeneticsID modified;
		PlayableGeneticsList::const_iterator const iter = rftl::find( mPlayableGenetics.begin(), mPlayableGenetics.end(), current );
		RF_ASSERT( iter != mPlayableGenetics.end() );
		if( increase )
		{
			if( iter + 1 == mPlayableGenetics.end() )
			{
				modified = mPlayableGenetics.front();
			}
			else
			{
				modified = *( iter + 1 );
			}
		}
		else
		{
			if( iter == mPlayableGenetics.begin() )
			{
				modified = mPlayableGenetics.back();
			}
			else
			{
				modified = *( iter - 1 );
			}
		}
		character::GeneticsEntry const& entry = charValidate.GetGeneticEntry( modified );
		mChar.mGenetics.mSpecies = entry.mSpecies;
		mChar.mGenetics.mGender = entry.mGender;
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_PhysAtk" ) )
	{
		statChange( mChar.mStats.mPhysAtk, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_PhysDef" ) )
	{
		statChange( mChar.mStats.mPhysDef, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_ElemAtk" ) )
	{
		statChange( mChar.mStats.mElemAtk, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_ElemDef" ) )
	{
		statChange( mChar.mStats.mElemDef, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_Balance" ) )
	{
		statChange( mChar.mStats.mBalance, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_Techniq" ) )
	{
		statChange( mChar.mStats.mTechniq, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_ElemPwr" ) )
	{
		statChange( mChar.mStats.mElemPwr, increase );
	}
	else if( focusContainerID == uiManager.GetContainerID( "O_Breadth" ) )
	{
		// TODO
	}

	charValidate.SanitizeForCharacterCreation( mChar );

	// HACK: Always recomposite
	RF_TODO_ANNOTATION( "Only recomposite on changes that affect visuals" );
	Recomposite();
}



void TitleScreen_CharCreate::InternalState::Recomposite()
{
	gfx::ppu::PPUController& ppu = *app::gGraphics;
	gfx::ppu::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
	sprite::CharacterCreator& charCreate = *gCharacterCreator;

	static constexpr char const kId[] = "CHAR_CREATE_TEMP";

	sprite::CompositeCharacterParams params = {};
	params.mMode = "36";
	params.mCompositeWidth = 36;
	params.mCompositeHeight = 40;
	params.mBaseId = mChar.mVisuals.mBase;
	params.mTopId = mChar.mVisuals.mTop;
	params.mBottomId = mChar.mVisuals.mBottom;
	params.mHairId = mChar.mVisuals.mHair;
	params.mSpeciesId = mChar.mVisuals.mSpecies;
	params.mCharPiecesDir = paths::CharacterPieces();
	params.mOutputDir = paths::CompositeCharacters().GetChild( kId );
	sprite::CompositeCharacter const character = charCreate.CreateCompositeCharacter( params );

	gfx::ppu::ManagedFramePackID const previewID = framePackMan.GetManagedResourceIDFromResourceName( details::kPreviewFpackName );
	if( previewID == gfx::ppu::kInvalidManagedFramePackID )
	{
		ppu.ForceImmediateLoadRequest(
			gfx::ppu::PPUController::AssetType::FramePack,
			details::kPreviewFpackName,
			character.mFramepacksByAnim.at( "idle_n" ) );
	}
	else
	{
		ppu.QueueDeferredReloadRequest(
			gfx::ppu::PPUController::AssetType::FramePack,
			details::kPreviewFpackName );
	}
}



void TitleScreen_CharCreate::InternalState::Load()
{
	RF_TODO_ANNOTATION( "Do actual loading" );

	mChar = {};

	character::CharacterValidator const& charValidate = *gCharacterValidator;
	charValidate.SanitizeForCharacterCreation( mChar );

	Recomposite();
}



void TitleScreen_CharCreate::InternalState::Save()
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_CharCreate::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	// Load
	mInternalState->Load();

	gfx::ppu::PPUController const& ppu = *app::gGraphics;
	gfx::ppu::FramePackManager const& framePackMan = *ppu.GetFramePackManager();
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Cut the whole screen into rows
		ui::controller::RowSlicer::Ratios const rootRowRatios = {
			{ 5.f / 7.f, true },
			{ 2.f / 7.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const rootRowSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::RowSlicer>::Create(
					rootRowRatios ) );

		// Put a message box at the bottom
		WeakPtr<ui::controller::MessageBox> const messageBox =
			uiManager.AssignStrongController(
				rootRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::MessageBox>::Create(
					3u,
					ui::font::MessageBox,
					ui::Justification::MiddleLeft,
					math::Color3f::kWhite,
					ui::GetBreakableChars() ) );
		messageBox->SetFrameTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "retro1_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
		messageBox->SetAnimationSpeed( ui::kTextSpeed );
		messageBox->SetText(
			"This is some text. It may take up a fair amount of space, so it"
			" will likely get broken up by the text box. It may even get"
			" truncated due to its large size... Like right about here... Or"
			" maybe here? If you're using widescreen, you can fit a lot more"
			" in, but sooner or later you're going to run out, I hope...",
			false );

		// Cut the top in 4
		ui::controller::ColumnSlicer::Ratios const topColumnRatios = {
			{ 1.f / 10.f, false },
			{ 5.f / 10.f, true },
			{ 3.f / 10.f, true },
			{ 1.f / 10.f, false },
		};
		WeakPtr<ui::controller::ColumnSlicer> const topColumnSlicer =
			uiManager.AssignStrongController(
				rootRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					topColumnRatios ) );

		// Floating frame in middle left
		gfx::ppu::Coord const leftFrameDimensions = { 128 - ( 16 + 8 ), 128 + 16 };
		WeakPtr<ui::controller::Floater> const leftFrameFloater =
			uiManager.AssignStrongController(
				topColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::Floater>::Create(
					leftFrameDimensions.x,
					leftFrameDimensions.y,
					ui::Justification::MiddleCenter ) );
		WeakPtr<ui::controller::BorderFrame> const leftFrame =
			uiManager.AssignStrongController(
				leftFrameFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		leftFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "retro1_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

		// Floating frame in middle right
		gfx::ppu::Coord const rightFrameDimensions = { 64 + 8, 128 + ( 16 - 4 ) };
		WeakPtr<ui::controller::Floater> const rightFrameFloater =
			uiManager.AssignStrongController(
				topColumnSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::Floater>::Create(
					rightFrameDimensions.x,
					rightFrameDimensions.y,
					ui::Justification::MiddleCenter ) );
		WeakPtr<ui::controller::BorderFrame> const rightFrame =
			uiManager.AssignStrongController(
				rightFrameFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		rightFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "retro1_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

		// Left frame
		{
			// Cut in 2
			ui::controller::ColumnSlicer::Ratios const leftColumnRatios = {
				{ 2.f / 5.f, true },
				{ 3.f / 5.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const leftColumnSlicer =
				uiManager.AssignStrongController(
					leftFrame->GetChildContainerID(),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						leftColumnRatios ) );

			// Create selections in the left frame
			WeakPtr<ui::controller::ListBox> const leftOptions =
				uiManager.AssignStrongController(
					leftColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						rftl::extent<decltype( details::kLeftText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			leftOptions->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
			for( size_t i = 0; i < rftl::extent<decltype( details::kLeftText )>::value; i++ )
			{
				leftOptions->GetMutableSlotController( i )->SetText( details::kLeftText[i] );
				uiManager.AssignLabel( leftOptions->GetSlotController( i )->GetContainerID(), details::kLeftTextTags[i] );
			}

			// Create status in the left frame
			// HACK: Text as a mock-up for controls
			RF_TODO_ANNOTATION( "This shouldn't be text" );
			WeakPtr<ui::controller::TextRows> const leftStatus =
				uiManager.AssignStrongController(
					leftColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::TextRows>::Create(
						rftl::extent<decltype( details::kLeftText )>::value,
						ui::font::NarrowQuarterTileMono,
						ui::Justification::MiddleLeft,
						math::Color3f::kWhite ) );
			for( size_t i = 0; i < rftl::extent<decltype( details::kLeftText )>::value; i++ )
			{
				// This will be controlled dynamically by update logic
				leftStatus->GetMutableSlotController( i )->SetText( "" );
				uiManager.AssignLabel( leftStatus->GetSlotController( i )->GetContainerID(), details::kLeftStatusTags[i] );
			}
		}


		// Right frame
		{
			// Cut in 2 to make room for preview
			ui::controller::RowSlicer::Ratios const rightRowRatios = {
				{ 6.f / 20.f, true },
				{ 14.f / 20.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const rightRowSlicer =
				uiManager.AssignStrongController(
					rightFrame->GetChildContainerID(),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						rightRowRatios ) );

			// Preview on top
			WeakPtr<ui::controller::FramePackDisplay> const preview =
				uiManager.AssignStrongController(
					rightRowSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::FramePackDisplay>::Create() );
			preview->SetJustification( ui::Justification::MiddleCenter );
			gfx::ppu::ManagedFramePackID const previewFpackID = framePackMan.GetManagedResourceIDFromResourceName( details::kPreviewFpackName );
			gfx::ppu::FramePackBase const& fPack = *framePackMan.GetResourceFromManagedResourceID( previewFpackID );
			preview->SetFramePack(
				previewFpackID,
				fPack.CalculateTimeIndexBoundary(),
				36,
				40 );
			preview->SetSlowdown( fPack.mPreferredSlowdownRate );

			// Cut bottom in 2
			ui::controller::ColumnSlicer::Ratios const rightColumnRatios = {
				{ 11.f / 20.f, true },
				{ 9.f / 20.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const rightColumnSlicer =
				uiManager.AssignStrongController(
					rightRowSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						rightColumnRatios ) );

			// Create selections in the right frame
			WeakPtr<ui::controller::ListBox> const rightOptions =
				uiManager.AssignStrongController(
					rightColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						rftl::extent<decltype( details::kRightText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rightOptions->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode().GetMutableFavoredChild() );
			for( size_t i = 0; i < rftl::extent<decltype( details::kRightText )>::value; i++ )
			{
				rightOptions->GetMutableSlotController( i )->SetText( details::kRightText[i] );
				uiManager.AssignLabel( rightOptions->GetSlotController( i )->GetContainerID(), details::kRightTextTags[i] );
			}

			// Create status in the right frame
			// HACK: Text as a mock-up for controls
			RF_TODO_ANNOTATION( "This shouldn't be text" );
			WeakPtr<ui::controller::TextRows> const rightStatus =
				uiManager.AssignStrongController(
					rightColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::TextRows>::Create(
						rftl::extent<decltype( details::kRightText )>::value,
						ui::font::NarrowQuarterTileMono,
						ui::Justification::MiddleLeft,
						math::Color3f::kWhite ) );
			for( size_t i = 0; i < rftl::extent<decltype( details::kRightText )>::value; i++ )
			{
				// This will be controlled dynamically by update logic
				rightStatus->GetMutableSlotController( i )->SetText( "" );
				uiManager.AssignLabel( rightStatus->GetSlotController( i )->GetContainerID(), details::kRightStatusTags[i] );
			}
		}

		// Disable overflow warnings on some long labels that are intended to
		//  spill over into the empty label next to them
		for( char const* const& tag : details::kLongTextTags )
		{
			uiManager.GetMutableControllerAs<ui::controller::TextLabel>( tag )->SetIgnoreOverflow( true );
		}

		// Perform an initial update
		mInternalState->UpdateDisplay( uiManager );
	}
}



void TitleScreen_CharCreate::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_CharCreate::OnTick( AppStateTickContext& context )
{
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();

	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		if( handled == false )
		{
			// Wasn't handled by general UI

			// Figure out the useful focus information
			ui::ContainerID const currentFocusContainerID = focusMan.GetFocusTree().GetCurrentFocusContainerID();

			if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
			{
				if( currentFocusContainerID != ui::kInvalidContainerID )
				{
					if( currentFocusContainerID == uiManager.GetContainerID( details::kSaveTag ) )
					{
						mInternalState->Save();
						context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
					}
				}
			}
			else if( focusEvent == ui::focusevent::Command_NavigateUp )
			{
				// Assume we're at the edge of a pane, and swap to the edge of
				//  the other pane
				focusMan.GetMutableFocusTree().CycleRootFocusToPreviousChild( true );
				focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateToLast );
			}
			else if( focusEvent == ui::focusevent::Command_NavigateDown )
			{
				// Assume we're at the edge of a pane, and swap to the edge of
				//  the other pane
				focusMan.GetMutableFocusTree().CycleRootFocusToNextChild( true );
				focusMan.HandleEvent( uiContext, ui::focusevent::Command_NavigateToFirst );
			}
			else if( focusEvent == ui::focusevent::Command_NavigateLeft )
			{
				mInternalState->HandleModification( uiManager, currentFocusContainerID, false );
				mInternalState->UpdateDisplay( uiManager );
			}
			else if( focusEvent == ui::focusevent::Command_NavigateRight )
			{
				mInternalState->HandleModification( uiManager, currentFocusContainerID, true );
				mInternalState->UpdateDisplay( uiManager );
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

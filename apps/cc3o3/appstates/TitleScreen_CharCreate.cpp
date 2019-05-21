#include "stdafx.h"
#include "TitleScreen_CharCreate.h"

#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/char/Character.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/CommonPaths.h"

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

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/FramePackManager.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

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
	"S_Breadth",
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
	void Load();
	void Save();

	UniquePtr<sprite::CharacterCreator> mCharacterCreator;
	character::CharacterValidator mCharacterValidator;

	rftl::vector<character::GeneticsEntry> mPlayableGenetics;

	character::Character mChar = {};
};



TitleScreen_CharCreate::InternalState::InternalState()
	: mCharacterCreator( DefaultCreator<sprite::CharacterCreator>::Create( app::gVfs, app::gGraphics ) )
	, mCharacterValidator( app::gVfs, mCharacterCreator )
{
	// Init creator
	sprite::CharacterCreator& charCreate = *mCharacterCreator;
	charCreate.LoadPieceTables(
		paths::gCharacterTables.GetChild( "pieces.csv" ),
		paths::gCharacterTables.GetChild( "pieces" ) );
	charCreate.LoadCompositionTable(
		paths::gCharacterTables.GetChild( "composite.csv" ) );

	// Init validator
	character::CharacterValidator& charValidate = mCharacterValidator;
	charValidate.LoadGeneticsTable(
		paths::gCharacterTables.GetChild( "genetics.csv" ) );
	charValidate.LoadStatBonusesTable(
		paths::gCharacterTables.GetChild( "statbonuses.csv" ) );

	// Figure out genetics options
	rftl::vector<character::CharacterValidator::GeneticsID> sortedGenetics;
	{
		rftl::unordered_set<character::CharacterValidator::GeneticsID> const allGenetics = charValidate.GetGeneticsIDs();
		sortedGenetics.reserve( allGenetics.size() );
		sortedGenetics.assign( allGenetics.begin(), allGenetics.end() );
		rftl::sort( sortedGenetics.begin(), sortedGenetics.end() );

		// HACK: Reverse
		// TODO: Data-driven control of ordering
		// NOTE: Reverse causes 'portal' to be before 'awoken...', so humans
		//  are first in the list
		rftl::reverse( sortedGenetics.begin(), sortedGenetics.end() );
	}
	for( character::CharacterValidator::GeneticsID const& genetics : sortedGenetics )
	{
		character::GeneticsEntry const& entry = charValidate.GetGeneticEntry( genetics );
		if( entry.mPlayable )
		{
			mPlayableGenetics.emplace_back( entry );
		}
	}
}



void TitleScreen_CharCreate::InternalState::UpdateDisplay( ui::ContainerManager& uiManager )
{
	rftl::string const& name = mChar.mDescription.mName;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "O_Charname" )->SetText( name + " [E]" );

	static constexpr auto format9 = []( rftl::string const& str ) -> rftl::string
	{
		rftl::string retVal;
		retVal += "< ";
		retVal += str;
		retVal += " >";
		return retVal;
	};

	static constexpr auto format2 = []( uint8_t val ) -> rftl::string
	{
		RF_ASSERT( val <= 99 );
		rftl::string retVal;
		retVal += "< ";
		retVal += '0' + ( val / 10 );
		retVal += '0' + ( val % 10 );
		retVal += " >";
		return retVal;
	};

	static constexpr auto format5Pips = []( uint8_t min, uint8_t cur, uint8_t avail ) -> rftl::string
	{
		RF_ASSERT( min <= 5 );
		RF_ASSERT( min <= cur );
		RF_ASSERT( cur <= 5 );
		uint8_t const max = math::integer_cast<uint8_t>( cur + avail );
		RF_ASSERT( cur <= max );
		rftl::string retVal;
		retVal += "- ";
		for( size_t i = 0; i < 5; i++ )
		{
			if( i < min )
			{
				retVal += "# ";
			}
			else if( i < cur )
			{
				retVal += "@ ";
			}
			else if( i < max )
			{
				retVal += "O ";
			}
			else
			{
				retVal += "  ";
			}
		}
		retVal += "+";
		return retVal;
	};

	static constexpr auto format10Slider = []( uint8_t val ) -> rftl::string
	{
		RF_ASSERT( val <= 10 );
		rftl::string retVal;
		retVal += "|";
		for( size_t i = 0; i < 11; i++ )
		{
			if( i == val )
			{
				retVal += "[";
			}
			else if( i == val + 1u )
			{
				retVal += "]";
			}
			else
			{
				retVal += "-";
			}
		}
		retVal += "|";
		return retVal;
	};

	// TODO
	rftl::string innate = "YYYYYYY";
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Innate" )->SetText( format9( innate ) );

	// TODO
	rftl::string const& genetics = "F.AwokenC";
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Genetics" )->SetText( format9( genetics ) );

	// TODO
	int8_t const avail = 2;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_PhysAtk" )->SetText( format5Pips( 0, 1, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_PhysDef" )->SetText( format5Pips( 0, 1, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemAtk" )->SetText( format5Pips( 0, 1, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemDef" )->SetText( format5Pips( 0, 1, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Balance" )->SetText( format5Pips( 0, 1, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Techniq" )->SetText( format5Pips( 2, 4, avail ) );
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_ElemPwr" )->SetText( format5Pips( 1, 2, avail ) );

	// TODO
	uint8_t const breadth = 4;
	uiManager.GetMutableControllerAs<ui::controller::TextLabel>( "S_Breadth" )->SetText( format10Slider( breadth ) );

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
	// TODO
}



void TitleScreen_CharCreate::InternalState::Load()
{
	// TODO: Actual load

	mChar = {};

	character::CharacterValidator& charValidate = mCharacterValidator;
	charValidate.SanitizeForCharacterCreation( mChar );
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

	gfx::PPUController const& ppu = *app::gGraphics;
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
		messageBox->SetFrameTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );
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
		gfx::PPUCoord const leftFrameDimensions = { 128 - ( 16 + 8 ), 128 + 16 };
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
		leftFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );

		// Floating frame in middle right
		gfx::PPUCoord const rightFrameDimensions = { 64 + 4, 128 + ( 16 - 4 ) };
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
		rightFrame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );

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
						rftl::extent<decltype( details::kLeftText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			leftOptions->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );
			for( size_t i = 0; i < rftl::extent<decltype( details::kLeftText )>::value; i++ )
			{
				leftOptions->GetSlotController( i )->SetText( details::kLeftText[i] );
				uiManager.AssignLabel( leftOptions->GetSlotController( i )->GetContainerID(), details::kLeftTextTags[i] );
			}

			// Create status in the left frame
			// HACK: Text as a mock-up for controls
			// TODO: This shouldn't be text
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
				leftStatus->GetSlotController( i )->SetText( "" );
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
						rftl::extent<decltype( details::kRightText )>::value,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rightOptions->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode().mFavoredChild );
			for( size_t i = 0; i < rftl::extent<decltype( details::kRightText )>::value; i++ )
			{
				rightOptions->GetSlotController( i )->SetText( details::kRightText[i] );
				uiManager.AssignLabel( rightOptions->GetSlotController( i )->GetContainerID(), details::kRightTextTags[i] );
			}

			// Create status in the right frame
			// HACK: Text as a mock-up for controls
			// TODO: This shouldn't be text
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
				rightStatus->GetSlotController( i )->SetText( "" );
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
	rftl::vector<ui::FocusEventType> const focusEvents = TitleScreen::GetInputToProcess();

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
			WeakPtr<ui::FocusTreeNode const> currentFocus;
			WeakPtr<ui::FocusTarget const> currentFocusTarget;
			ui::ContainerID currentFocusContainerID = ui::kInvalidContainerID;
			{
				currentFocus = focusMan.GetFocusTree().GetCurrentFocus();
				if( currentFocus != nullptr )
				{
					currentFocusTarget = currentFocus->mFocusTarget;
				}
				if( currentFocusTarget != nullptr )
				{
					RF_ASSERT( currentFocusTarget->HasHardFocus() );
					currentFocusContainerID = currentFocusTarget->mContainerID;
				}
			}

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
}}}

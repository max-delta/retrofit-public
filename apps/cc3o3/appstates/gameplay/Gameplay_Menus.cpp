#include "stdafx.h"
#include "Gameplay_Menus.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/UINavigation.h"
#include "cc3o3/state/components/Roster.h"
#include "cc3o3/state/components/SiteVisual.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/MultiPassthrough.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/ListBox.h"
#include "GameUI/controllers/FramePackDisplay.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Menus::InternalState
{
	RF_NO_COPY( InternalState );

public:
	struct TopLevelSections
	{
		enum Section : size_t
		{
			kStatus = 0,
			kLoadout,
			kOptions,

			kNumSections
		};

		static constexpr char const* kLabels[kNumSections] = {
			"H_status",
			"H_loadout",
			"H_options",
		};

		static constexpr char const* kHeaders[kNumSections] = {
			"$gamemenu_status_header",
			"$gamemenu_loadout_header",
			"$gamemenu_options_header",
		};
	};


public:
	InternalState() = default;

	void SwitchTopLevelSection( TopLevelSections::Section section );
	bool IsActiveSection( TopLevelSections::Section section ) const;
	void ShowSelector( ui::UIContext& context );
	void HideSelector( ui::UIContext& context, TopLevelSections::Section chosenSection );


public:
	WeakPtr<ui::controller::TextLabel> mMainHeader;

	WeakPtr<ui::controller::InstancedController> mSectionSelector;

	using TopLevelControllers = rftl::array<WeakPtr<ui::controller::InstancedController>, TopLevelSections::kNumSections>;
	TopLevelControllers mTopLevelControllers;

	struct CharSlot
	{
		WeakPtr<ui::controller::BorderFrame> mFrame;
		WeakPtr<ui::controller::FramePackDisplay> mDisplay;
	};
	rftl::array<CharSlot, 3> mCharSlots;

	WeakPtr<state::comp::UINavigation> mNavigation;
};



void Gameplay_Menus::InternalState::SwitchTopLevelSection( TopLevelSections::Section section )
{
	// Only show current section
	for( WeakPtr<ui::Controller> const& controller : mTopLevelControllers )
	{
		controller->SetChildRenderingBlocked( true );
	}
	mTopLevelControllers.at( section )->SetChildRenderingBlocked( false );

	RF_ASSERT( section < rftl::extent<decltype( TopLevelSections::kHeaders )>::value );
	mMainHeader->SetText( TopLevelSections::kHeaders[section] );
}



bool Gameplay_Menus::InternalState::IsActiveSection( TopLevelSections::Section section ) const
{
	return mTopLevelControllers.at( section )->IsChildRenderingBlocked() == false;
}



void Gameplay_Menus::InternalState::ShowSelector( ui::UIContext& context )
{
	mSectionSelector->SetChildRenderingBlocked( false );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mSectionSelector->GetMutableFocusTreeNode( context ) );
}



void Gameplay_Menus::InternalState::HideSelector( ui::UIContext& context, TopLevelSections::Section chosenSection )
{
	SwitchTopLevelSection( chosenSection );

	mSectionSelector->SetChildRenderingBlocked( true );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mTopLevelControllers.at( chosenSection )->GetMutableFocusTreeNode( context ) );
}

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Menus::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();
	//gfx::FramePackManager const& framePackMan = *ppu.GetFramePackManager();

	// Find navigation component
	state::VariableIdentifier const localUIRoot( "localUI" );
	state::MutableObjectRef const localUI = state::FindMutableObjectByIdentifier( localUIRoot );
	RFLOG_TEST_AND_FATAL( localUI.IsSet(), nullptr, RFCAT_CC3O3, "Failed to find UI object" );
	internalState.mNavigation = localUI.GetMutableComponentInstanceT<state::comp::UINavigation>();
	RFLOG_TEST_AND_FATAL( internalState.mNavigation != nullptr, nullptr, RFCAT_CC3O3, "Failed to find navigation component" );

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Cut the whole screen into columns
		ui::controller::ColumnSlicer::Ratios const rootColumnRatios = {
			{ 1.f / 20.f, false },
			{ 18.f / 20.f, true },
			{ 1.f / 20.f, false },
		};
		WeakPtr<ui::controller::ColumnSlicer> const rootColumnSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					rootColumnRatios ) );

		// Cut the middle in 3
		ui::controller::RowSlicer::Ratios const middleRowRatios = {
			{ 1.f / 14.f, true },
			{ 12.f / 14.f, true },
			{ 1.f / 14.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const middleRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					middleRowRatios ) );

		// Header in top center
		WeakPtr<ui::controller::TextLabel> const mainHeader =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainHeader->SetJustification( ui::Justification::BottomCenter );
		mainHeader->SetFont( ui::font::LargeMenuHeader );
		mainHeader->SetText( "UNSET" );
		mainHeader->SetColor( math::Color3f::kWhite );
		mainHeader->SetBorder( true );
		internalState.mMainHeader = mainHeader;

		// Controls in bottom right
		WeakPtr<ui::controller::TextLabel> const mainFooter =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainFooter->SetJustification( ui::Justification::TopRight );
		mainFooter->SetFont( ui::font::SmallMenuSelection );
		mainFooter->SetText( "UNSET" );
		mainFooter->SetColor( math::Color3f::kWhite );
		mainFooter->SetBorder( true );

		// Main sections are in the center
		using TopLevelSections = InternalState::TopLevelSections;
		WeakPtr<ui::controller::MultiPassthrough> const sectionPassthroughs =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::MultiPassthrough>::Create( TopLevelSections::kNumSections + 1 ) );

		// Section selector floats above the screen
		WeakPtr<ui::controller::Floater> const sectionSelector =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kNumSections ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					ui::Justification::MiddleCenter ) );
		uiManager.AdjustRecommendedRenderDepth( sectionSelector->GetContainerID(), -20 );
		sectionSelector->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );
		internalState.mSectionSelector = sectionSelector;

		// Sections each get a passthrough that can be used to shut off all
		//  children when switching sections
		WeakPtr<ui::controller::Passthrough> const statusPassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kStatus ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const loadoutPassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kLoadout ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const optionsPassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kOptions ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		internalState.mTopLevelControllers.at( TopLevelSections::kStatus ) = statusPassthrough;
		internalState.mTopLevelControllers.at( TopLevelSections::kLoadout ) = loadoutPassthrough;
		internalState.mTopLevelControllers.at( TopLevelSections::kOptions ) = optionsPassthrough;
		for( WeakPtr<ui::controller::InstancedController> const& controller : internalState.mTopLevelControllers )
		{
			controller->AddAsSiblingAfterFocusTreeNode( uiContext, sectionSelector->GetMutableFocusTreeNode( uiContext ) );
		}

		// Section selector
		{
			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					sectionSelector->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "retro1_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

			// Implement selector as horizontal list
			// TODO: Image support in list boxes
			WeakPtr<ui::controller::ListBox> const selector =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Horizontal,
						TopLevelSections::kNumSections,
						ui::font::NarrowHalfTileMono ) );
			for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
			{
				WeakPtr<ui::controller::TextLabel> const slotController = selector->GetSlotController( i );
				{
					// TODO: Image support
					rftl::string temp = "0";
					temp.at( 0 ) += math::integer_cast<char>( i );
					slotController->SetText( rftl::move( temp ) );
				}
				uiManager.AssignLabel( slotController->GetContainerID(), TopLevelSections::kLabels[i] );
			}
			selector->AddAsChildToFocusTreeNode( uiContext, *sectionSelector->GetMutableFocusTreeNode( uiContext ) );
		}

		// Status section
		{
			// Placeholder
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					statusPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
		}

		// Loadout section
		{
			// Cut the section into columns
			ui::controller::ColumnSlicer::Ratios const loadoutColumnRatios = {
				{ 6.f / 18.f, true },
				{ 12.f / 18.f, true },
			};
			WeakPtr<ui::controller::ColumnSlicer> const loadoutColumnSlicer =
				uiManager.AssignStrongController(
					loadoutPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::ColumnSlicer>::Create(
						loadoutColumnRatios ) );

			// 3 character slots on left
			ui::controller::RowSlicer::Ratios const characterRowRatios = {
				{ 4.f / 12.f, true },
				{ 4.f / 12.f, true },
				{ 4.f / 12.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const characterRowSlicer =
				uiManager.AssignStrongController(
					loadoutColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						characterRowRatios ) );

			// Character slots
			for( size_t i_char = 0; i_char < 3; i_char++ )
			{
				// Frame
				WeakPtr<ui::controller::BorderFrame> const frame =
					uiManager.AssignStrongController(
						characterRowSlicer->GetChildContainerID( i_char ),
						DefaultCreator<ui::controller::BorderFrame>::Create() );
				frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
				frame->SetChildRenderingBlocked( true );
				internalState.mCharSlots.at( i_char ).mFrame = frame;

				// 2 subsections for info and display
				ui::controller::ColumnSlicer::Ratios const charSlotColumnRatios = {
					{ 7.f / 10.f, true },
					{ 3.f / 10.f, true },
				};
				WeakPtr<ui::controller::ColumnSlicer> const charSlotColumnSlicer =
					uiManager.AssignStrongController(
						frame->GetChildContainerID(),
						DefaultCreator<ui::controller::ColumnSlicer>::Create(
							charSlotColumnRatios ) );

				// 3 info rows
				ui::controller::RowSlicer::Ratios const infoRowRatios = {
					{ 1.f / 3.f, true },
					{ 1.f / 3.f, true },
					{ 1.f / 3.f, true },
				};
				WeakPtr<ui::controller::RowSlicer> const infoRowSlicer =
					uiManager.AssignStrongController(
						charSlotColumnSlicer->GetChildContainerID( 0 ),
						DefaultCreator<ui::controller::RowSlicer>::Create(
							infoRowRatios ) );

				for( size_t i_info = 0; i_info < 3; i_info++ )
				{
					// Info
					WeakPtr<ui::controller::TextLabel> const info =
						uiManager.AssignStrongController(
							infoRowSlicer->GetChildContainerID( i_info ),
							DefaultCreator<ui::controller::TextLabel>::Create() );
					info->SetJustification( ui::Justification::MiddleLeft );
					info->SetFont( ui::font::LargeMenuText );
					info->SetText( "UNSET" );
					info->SetColor( math::Color3f::kWhite );
					info->SetBorder( true );
				}

				// Display
				WeakPtr<ui::controller::FramePackDisplay> const display =
					uiManager.AssignStrongController(
						charSlotColumnSlicer->GetChildContainerID( 1 ),
						DefaultCreator<ui::controller::FramePackDisplay>::Create() );
				display->SetJustification( ui::Justification::MiddleCenter );
				internalState.mCharSlots.at( i_char ).mDisplay = display;
			}

			// 2 subsections for element manager on right
			ui::controller::RowSlicer::Ratios const elementManagerRowRatios = {
				{ 3.f / 12.f, true },
				{ 9.f / 12.f, true },
			};
			WeakPtr<ui::controller::RowSlicer> const elementManagerRowSlicer =
				uiManager.AssignStrongController(
					loadoutColumnSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::RowSlicer>::Create(
						elementManagerRowRatios ) );
		}

		// Options section
		{
			// Placeholder
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					optionsPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
		}
	}

	// Start on the first section
	internalState.SwitchTopLevelSection( static_cast<InternalState::TopLevelSections::Section>( 0 ) );
}



void Gameplay_Menus::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Menus::OnTick( AppStateTickContext& context )
{
	using TopLevelSections = InternalState::TopLevelSections;

	app::gGraphics->DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Menus" );

	InternalState& internalState = *mInternalState;
	//state::comp::UINavigation& navigation = *internalState.mNavigation;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();

	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		focusMan.UpdateHardFocus( uiContext );

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

		if( handled )
		{
			// Normally we would ignore this, but we want to check to see if
			//  this was the section selector moving, in which case we want to
			//  change which section is displayed behind it
			if( focusMan.GetFocusTree().IsInCurrentFocusStack( internalState.mSectionSelector->GetContainerID() ) )
			{
				bool foundSection = false;
				for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
				{
					if( currentFocusContainerID == uiManager.GetContainerID( TopLevelSections::kLabels[i] ) )
					{
						internalState.SwitchTopLevelSection( static_cast<TopLevelSections::Section>( i ) );
						foundSection = true;
						break;
					}
				}
				RF_ASSERT( foundSection );
			}
		}
		else
		{
			// Wasn't handled by general UI

			if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
			{
				if( currentFocusContainerID != ui::kInvalidContainerID )
				{
					if( focusMan.GetFocusTree().IsInCurrentFocusStack( internalState.mSectionSelector->GetContainerID() ) )
					{
						// Section selection
						bool foundSection = false;
						for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
						{
							if( currentFocusContainerID == uiManager.GetContainerID( TopLevelSections::kLabels[i] ) )
							{
								TopLevelSections::Section const chosenSection = static_cast<TopLevelSections::Section>( i );

								// This should've already been made active
								RF_ASSERT( internalState.IsActiveSection( chosenSection ) );

								// Hide the selector, moving focus to the
								//  chosen section instead
								internalState.HideSelector( uiContext, chosenSection );

								foundSection = true;
								break;
							}
						}
						RF_ASSERT( foundSection );
					}
					else
					{
						// TODO: Sections
					}
				}
			}
			else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
			{
				// HACK: Always pop back up to section selector
				// TODO: Proper logic
				internalState.ShowSelector( uiContext );
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}


	// Menus use the local player
	input::PlayerID const playerID = input::HardcodedGetLocalPlayer();
	rftl::string const playerIDAsString = ( rftl::stringstream() << math::integer_cast<size_t>( playerID ) ).str();

	// Find company object
	state::VariableIdentifier const companyRoot( "company", playerIDAsString );
	state::ObjectRef const company = state::FindObjectByIdentifier( companyRoot );
	RFLOG_TEST_AND_FATAL( company.IsSet(), companyRoot, RFCAT_CC3O3, "Failed to find company" );

	// Get the roster
	state::VariableIdentifier const rosterRoot = companyRoot.GetChild( "member" );
	state::comp::Roster const& roster = *company.GetComponentInstanceT<state::comp::Roster>();

	// Get the active party characters
	rftl::array<state::MutableObjectRef, 3> activePartyCharacters;
	for( size_t i_teamIndex = 0; i_teamIndex < state::comp::Roster::kActiveTeamSize; i_teamIndex++ )
	{
		state::MutableObjectRef& activePartyCharacter = activePartyCharacters.at( i_teamIndex );

		state::comp::Roster::RosterIndex const rosterIndex = roster.mActiveTeam.at( i_teamIndex );
		if( rosterIndex == state::comp::Roster::kInvalidRosterIndex )
		{
			// Not active
			continue;
		}
		rftl::string const rosterIndexAsString = ( rftl::stringstream() << math::integer_cast<size_t>( rosterIndex ) ).str();

		state::VariableIdentifier const charRoot = rosterRoot.GetChild( rosterIndexAsString );
		state::MutableObjectRef const character = state::FindMutableObjectByIdentifier( charRoot );
		RFLOG_TEST_AND_FATAL( character.IsSet(), charRoot, RFCAT_CC3O3, "Failed to find character" );
		activePartyCharacter = character;
	}

	// Update character displays
	{
		for( size_t i_char = 0; i_char < 3; i_char++ )
		{
			state::MutableObjectRef const& character = activePartyCharacters.at( i_char );
			InternalState::CharSlot& charSlot = internalState.mCharSlots.at( i_char );
			if( character.IsSet() == false )
			{
				charSlot.mFrame->SetChildRenderingBlocked( true );
				continue;
			}
			charSlot.mFrame->SetChildRenderingBlocked( false );

			state::comp::SiteVisual& visual = *character.GetMutableComponentInstanceT<state::comp::SiteVisual>();
			state::comp::SiteVisual::Anim const& anim = visual.mIdleSouth;
			charSlot.mDisplay->SetFramePack(
				anim.mFramePackID,
				anim.mMaxTimeIndex,
				( gfx::kTileSize / 4 ) * 3,
				( gfx::kTileSize / 4 ) * 5 );

			// TODO: Update text
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

#include "stdafx.h"
#include "Gameplay_Menus.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/state/StateLogging.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/controllers/CharacterSlotList.h"
#include "cc3o3/ui/controllers/ElementGridSelector.h"
#include "cc3o3/ui/controllers/ElementStockpileSelector.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

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

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

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

	state::ObjectRef GetCurrentLoadoutCharacter( ui::UIConstContext const& context ) const;
	state::MutableObjectRef GetCurrentMutableLoadoutCharacter( ui::UIConstContext const& context );


public:
	WeakPtr<ui::controller::TextLabel> mMainHeader;

	WeakPtr<ui::controller::InstancedController> mSectionSelectorFloater;
	WeakPtr<ui::controller::ListBox> mSectionSelector;

	using TopLevelControllers = rftl::array<WeakPtr<ui::controller::InstancedController>, TopLevelSections::kNumSections>;
	TopLevelControllers mTopLevelControllers;

	WeakPtr<ui::controller::CharacterSlotList> mCharSlots;
	WeakPtr<ui::controller::ElementGridSelector> mElementGridSelector;
	WeakPtr<ui::controller::ElementStockpileSelector> mElementStockpileSelector;
};



void Gameplay_Menus::InternalState::SwitchTopLevelSection( TopLevelSections::Section section )
{
	// Only show current section
	for( WeakPtr<ui::controller::InstancedController> const& controller : mTopLevelControllers )
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
	mSectionSelectorFloater->SetChildRenderingBlocked( false );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mSectionSelectorFloater->GetMutableFocusTreeNode( context ) );
}



void Gameplay_Menus::InternalState::HideSelector( ui::UIContext& context, TopLevelSections::Section chosenSection )
{
	SwitchTopLevelSection( chosenSection );

	mSectionSelectorFloater->SetChildRenderingBlocked( true );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mTopLevelControllers.at( chosenSection )->GetMutableFocusTreeNode( context ) );
}



state::ObjectRef Gameplay_Menus::InternalState::GetCurrentLoadoutCharacter( ui::UIConstContext const& context ) const
{
	size_t const currentCharIndex = mCharSlots->GetSlotIndexWithSoftFocus( context );
	return gCompanyManager->FindActivePartyCharacterObject( input::HardcodedGetLocalPlayer(), currentCharIndex );
}



state::MutableObjectRef Gameplay_Menus::InternalState::GetCurrentMutableLoadoutCharacter( ui::UIConstContext const& context )
{
	size_t const currentCharIndex = mCharSlots->GetSlotIndexWithSoftFocus( context );
	return gCompanyManager->FindMutableActivePartyCharacterObject( input::HardcodedGetLocalPlayer(), currentCharIndex );
}

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Menus::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

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
		WeakPtr<ui::controller::Floater> const sectionSelectorFloater =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kNumSections ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					ui::Justification::MiddleCenter ) );
		uiManager.AdjustRecommendedRenderDepth( sectionSelectorFloater->GetContainerID(), -20 );
		sectionSelectorFloater->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
		internalState.mSectionSelectorFloater = sectionSelectorFloater;

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
			controller->AddAsSiblingAfterFocusTreeNode( uiContext, sectionSelectorFloater->GetMutableFocusTreeNode( uiContext ) );
		}

		// Section selector
		{
			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					sectionSelectorFloater->GetChildContainerID(),
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
				WeakPtr<ui::controller::TextLabel> const slotController = selector->GetMutableSlotController( i );
				{
					// TODO: Image support
					rftl::string temp = "0";
					temp.at( 0 ) += math::integer_cast<char>( i );
					slotController->SetText( rftl::move( temp ) );
				}
				uiManager.AssignLabel( slotController->GetContainerID(), TopLevelSections::kLabels[i] );
			}
			selector->AddAsChildToFocusTreeNode( uiContext, *sectionSelectorFloater->GetMutableFocusTreeNode( uiContext ) );
			internalState.mSectionSelector = selector;
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

			// Character slots on left
			WeakPtr<ui::controller::CharacterSlotList> const characterList =
				uiManager.AssignStrongController(
					loadoutColumnSlicer->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::CharacterSlotList>::Create() );
			characterList->AddAsChildToFocusTreeNode(
				uiContext, *internalState.mTopLevelControllers.at( TopLevelSections::kLoadout )->GetMutableFocusTreeNode( uiContext ) );
			internalState.mCharSlots = characterList;

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

			// TODO: Description display on top

			// 2 subsections for selectors on bottom
			WeakPtr<ui::controller::MultiPassthrough> const selectorPassthroughs =
				uiManager.AssignStrongController(
					elementManagerRowSlicer->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::MultiPassthrough>::Create( 2u ) );

			// Element grid selector
			WeakPtr<ui::controller::Floater> const elementGridSelectorFloater =
				uiManager.AssignStrongController(
					selectorPassthroughs->GetChildContainerID( 0 ),
					DefaultCreator<ui::controller::Floater>::Create(
						ui::controller::ElementGridSelector::kContainerWidth,
						ui::controller::ElementGridSelector::kContainerHeight,
						ui::Justification::MiddleCenter ) );
			WeakPtr<ui::controller::ElementGridSelector> const elementGridSelector =
				uiManager.AssignStrongController(
					elementGridSelectorFloater->GetChildContainerID(),
					DefaultCreator<ui::controller::ElementGridSelector>::Create() );
			elementGridSelector->AddAsSiblingAfterFocusTreeNode(
				uiContext, characterList->GetMutableFocusTreeNode( uiContext ) );
			internalState.mElementGridSelector = elementGridSelector;

			// Element stockpile selector
			WeakPtr<ui::controller::Floater> const elementStockpileSelectorFloater =
				uiManager.AssignStrongController(
					selectorPassthroughs->GetChildContainerID( 1 ),
					DefaultCreator<ui::controller::Floater>::Create(
						ui::controller::ElementStockpileSelector::kContainerWidth,
						ui::controller::ElementStockpileSelector::kContainerHeight,
						ui::Justification::BottomRight ) );
			WeakPtr<ui::controller::ElementStockpileSelector> const elementStockpileSelector =
				uiManager.AssignStrongController(
					elementStockpileSelectorFloater->GetChildContainerID(),
					DefaultCreator<ui::controller::ElementStockpileSelector>::Create() );
			elementStockpileSelector->SetPagination( true );
			elementStockpileSelector->UpdateFromCompany(
				gCompanyManager->FindCompanyObject( input::HardcodedGetLocalPlayer() ) );
			elementStockpileSelector->AddAsSiblingAfterFocusTreeNode(
				uiContext, characterList->GetMutableFocusTreeNode( uiContext ) );
			internalState.mElementStockpileSelector = elementStockpileSelector;

			// Raise the stockpile selector over the grid selector
			uiManager.AdjustRecommendedRenderDepth( elementStockpileSelector->GetContainerID(), -3 );

			// Hide grid and stockpile selectors by default
			elementGridSelector->SetRenderingBlocked( true );
			elementGridSelector->SetChildRenderingBlocked( true );
			elementStockpileSelector->SetRenderingBlocked( true );
			elementStockpileSelector->SetChildRenderingBlocked( true );
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

	InternalState& internalState = *mInternalState;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();

	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetGameMenuInputToProcess( input::HardcodedGetLocalPlayer() );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		focusMan.UpdateHardFocus( uiContext );

		// Figure out the useful focus information
		ui::ContainerID const currentFocusContainerID = focusMan.GetFocusTree().GetCurrentFocusContainerID();

		if( handled )
		{
			// Normally we would ignore this, but we want to check to see if
			//  this was the section selector moving, in which case we want to
			//  change which section is displayed behind it
			if( internalState.mSectionSelector->SlotHasCurrentFocus( uiContext ) )
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

			if( currentFocusContainerID != ui::kInvalidContainerID )
			{
				if( internalState.mSectionSelector->SlotHasCurrentFocus( uiContext ) )
				{
					// Section selection

					if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
					{
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
					else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
					{
						// HACK: Always assume returning to overworld
						// TODO: Proper logic
						context.mManager.RequestDeferredStateChange( id::Gameplay_Overworld );
					}
				}
				else
				{
					// Sections
					TopLevelSections::Section currentSection = TopLevelSections::kNumSections;
					for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
					{
						if( internalState.mTopLevelControllers.at( i )->IsInCurrentFocusStack( uiContext ) )
						{
							currentSection = math::enum_bitcast<TopLevelSections::Section>( i );
						}
					}
					RF_ASSERT( currentSection != TopLevelSections::kNumSections );

					if( currentSection == TopLevelSections::kStatus )
					{
						// Status

						if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
						{
							// TODO
						}
						else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
						{
							// HACK: Always pop back up to section selector
							// TODO: Proper logic
							internalState.ShowSelector( uiContext );
						}
					}
					else if( currentSection == TopLevelSections::kLoadout )
					{
						// Loadout

						ui::controller::InstancedController& loadoutSection =
							*internalState.mTopLevelControllers.at( TopLevelSections::kLoadout );
						if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
						{
							if( internalState.mCharSlots->SlotHasCurrentFocus( uiContext ) )
							{
								// Char -> Grid
								internalState.mElementGridSelector->SetRenderingBlocked( false );
								internalState.mElementGridSelector->SetChildRenderingBlocked( false );
								focusMan.GetMutableFocusTree().SetFocusToSpecificChild(
									*loadoutSection.GetMutableFocusTreeNode( uiContext ),
									internalState.mElementGridSelector->GetMutableFocusTreeNode( uiContext ) );

								// Load the grid
								state::ObjectRef const currentChar = internalState.GetCurrentLoadoutCharacter( uiContext );
								internalState.mElementGridSelector->UpdateFromCharacter( currentChar );
							}
							else if( internalState.mElementGridSelector->SlotHasCurrentFocus( uiContext ) )
							{
								// Grid -> Stockpile
								internalState.mElementStockpileSelector->SetRenderingBlocked( false );
								internalState.mElementStockpileSelector->SetChildRenderingBlocked( false );
								focusMan.GetMutableFocusTree().SetFocusToSpecificChild(
									*loadoutSection.GetMutableFocusTreeNode( uiContext ),
									internalState.mElementStockpileSelector->GetMutableFocusTreeNode( uiContext ) );

								// Darken grid
								internalState.mElementGridSelector->DarkenAll();
							}
							else if( internalState.mElementStockpileSelector->SlotHasCurrentFocus( uiContext ) )
							{
								// Grid <- Stockpile (assign)
								internalState.mElementStockpileSelector->SetRenderingBlocked( true );
								internalState.mElementStockpileSelector->SetChildRenderingBlocked( true );
								focusMan.GetMutableFocusTree().SetFocusToSpecificChild(
									*loadoutSection.GetMutableFocusTreeNode( uiContext ),
									internalState.mElementGridSelector->GetMutableFocusTreeNode( uiContext ) );

								// Assign
								state::MutableObjectRef const currentChar = internalState.GetCurrentMutableLoadoutCharacter( uiContext );
								character::ElementSlotIndex const slot = internalState.mElementGridSelector->GetSelectedIndex( uiContext );
								element::ElementIdentifier const element = internalState.mElementStockpileSelector->GetSelectedIdentifier( uiContext );
								gCompanyManager->AssignElementToCharacter( currentChar, slot, element );

								// Un-darken grid (and refresh after assignment)
								internalState.mElementGridSelector->UpdateFromCharacter( currentChar );
							}
						}
						else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
						{
							if( internalState.mCharSlots->SlotHasCurrentFocus( uiContext ) )
							{
								// Exit section
								internalState.ShowSelector( uiContext );

								// Save loadouts
								gCompanyManager->WriteLoadoutsToSave( input::HardcodedGetLocalPlayer() );
							}
							else if( internalState.mElementGridSelector->SlotHasCurrentFocus( uiContext ) )
							{
								// Char <- Grid
								internalState.mElementGridSelector->SetRenderingBlocked( true );
								internalState.mElementGridSelector->SetChildRenderingBlocked( true );
								focusMan.GetMutableFocusTree().SetFocusToSpecificChild(
									*loadoutSection.GetMutableFocusTreeNode( uiContext ),
									internalState.mCharSlots->GetMutableFocusTreeNode( uiContext ) );
							}
							else if( internalState.mElementStockpileSelector->SlotHasCurrentFocus( uiContext ) )
							{
								// Grid <- Stockpile
								internalState.mElementStockpileSelector->SetRenderingBlocked( true );
								internalState.mElementStockpileSelector->SetChildRenderingBlocked( true );
								focusMan.GetMutableFocusTree().SetFocusToSpecificChild(
									*loadoutSection.GetMutableFocusTreeNode( uiContext ),
									internalState.mElementGridSelector->GetMutableFocusTreeNode( uiContext ) );

								// Un-darken grid
								state::ObjectRef const currentChar = internalState.GetCurrentLoadoutCharacter( uiContext );
								internalState.mElementGridSelector->UpdateFromCharacter( currentChar );
							}
						}
					}
					else if( currentSection == TopLevelSections::kOptions )
					{
						// Options

						if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
						{
							// TODO
						}
						else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
						{
							// HACK: Always pop back up to section selector
							// TODO: Proper logic
							internalState.ShowSelector( uiContext );
						}
					}
					else
					{
						// Uknown/unhandled section
						RF_DBGFAIL();
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}


	// Menus use the local player
	input::PlayerID const playerID = input::HardcodedGetLocalPlayer();

	// Get the active party characters
	rftl::array<state::ObjectRef, 3> const activePartyCharacters =
		gCompanyManager->FindActivePartyObjects( playerID );

	// Update character slots
	ui::controller::CharacterSlotList& charSlots = *internalState.mCharSlots;
	for( size_t i_char = 0; i_char < charSlots.GetNumSlots(); i_char++ )
	{
		state::ObjectRef const& character = activePartyCharacters.at( i_char );

		if( character.IsSet() )
		{
			charSlots.UpdateCharacter( i_char, character );
		}
		else
		{
			charSlots.ClearCharacter( i_char );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

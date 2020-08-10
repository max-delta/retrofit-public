#include "stdafx.h"
#include "Gameplay_Battle.h"

#include "cc3o3/Common.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/combat/FightController.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/UINavigation.h"
#include "cc3o3/ui/LocalizationHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/ListBox.h"
#include "GameUI/controllers/MultiPassthrough.h"
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Battle::InternalState
{
	RF_NO_COPY( InternalState );

public:
	struct INTELLISENSE_WORKAROUND
	{
		// Intellisense fails on partial updates of ControlStates::State, and
		//  having this struct precede it fixes it... *shrug*
	};

	struct ControlStates
	{
		enum State : size_t
		{
			kWaiting = 0,
			kTargeting,
			kAction,
			kAttack,
			kElement,

			kNumStates
		};
	};

	struct SelectorActions
	{
		enum Action : size_t
		{
			kAttack = 0,
			kElement,
			kDefend,
			kWait,

			kNumActions
		};

		static constexpr char const* kLabels[kNumActions] = {
			"$battle_action_attack",
			"$battle_action_element",
			"$battle_action_defend",
			"$battle_action_wait",
		};
	};

	struct SelectorAttacks
	{
		enum Action : size_t
		{
			kAttack1 = 0,
			kAttack2 = 1,
			kAttack3 = 2,
			kNumAttacks,
			kElement = kNumAttacks,

			kNumActions
		};
	};

	static constexpr char const kLabelStateWaiting[] = "$battle_state_waiting";
	static constexpr char const kLabelAttackElement[] = "$battle_attack_element";

	enum class TargetingReason : uint8_t
	{
		kInvalid = 0,
		kAttack,
		kElement
	};


public:
	InternalState() = default;

	ControlStates::State GetControlState( ui::UIConstContext const& context ) const;
	void SwitchControlState( ui::UIContext& context, ControlStates::State state );
	void RestoreUIState( ui::UIContext& context );
	void SaveUIState( ui::UIContext& context ) const;
	void SanitizeUIState( ui::UIContext& context );

	void ShiftControlChar( int8_t applyOffset );
	void ShiftTarget( int8_t applyOffset );


public:
	UniquePtr<combat::FightController> mFightController;

	WeakPtr<state::comp::UINavigation> mNavigation;

	using StateControllers = rftl::array<WeakPtr<ui::controller::InstancedController>, ControlStates::kNumStates>;
	StateControllers mStateControllers;

	uint8_t mControlCharIndex = 0;
	TargetingReason mTargetingReason = TargetingReason::kInvalid;
	uint8_t mTargetingIndex = 0;
	WeakPtr<ui::controller::ListBox> mActionMenu;
	WeakPtr<ui::controller::ListBox> mAttackMenu;
};



Gameplay_Battle::InternalState::ControlStates::State Gameplay_Battle::InternalState::GetControlState( ui::UIConstContext const& context ) const
{
	ui::FocusManager const& focusMan = context.GetFocusManager();
	WeakPtr<ui::FocusTreeNode const> focusedControls = focusMan.GetFocusTree().GetRootNode().GetFavoredChild();
	for( size_t i = 0; i < mStateControllers.size(); i++ )
	{
		if( mStateControllers.at( i )->GetFocusTreeNode( context ) == focusedControls )
		{
			return math::enum_bitcast<ControlStates::State>( i );
		}
	}
	RF_DBGFAIL();
	return ControlStates::kNumStates;
}



void Gameplay_Battle::InternalState::SwitchControlState( ui::UIContext& context, ControlStates::State state )
{
	// Only show current control state
	for( WeakPtr<ui::controller::InstancedController> const& controller : mStateControllers )
	{
		controller->SetChildRenderingBlocked( true );
	}
	WeakPtr<ui::controller::InstancedController> const desiredController = mStateControllers.at( state );
	desiredController->SetChildRenderingBlocked( false );

	// Switch focus
	ui::FocusManager& focusMan = context.GetMutableFocusManager();
	focusMan.GetMutableFocusTree().SetRootFocusToSpecificChild( desiredController->GetMutableFocusTreeNode( context ) );
}



void Gameplay_Battle::InternalState::RestoreUIState( ui::UIContext& context )
{
	// Load UI state from navigation component
	state::comp::UINavigation::BattleMenu const& navigation = mNavigation->mBattleMenu;

	mControlCharIndex = navigation.mControlCharIndex;

	ControlStates::State const currentState = math::enum_bitcast<ControlStates::State>( navigation.mControlState );
	SwitchControlState( context, currentState );
	if( currentState == ControlStates::kWaiting )
	{
		// No cursor
	}
	else if( currentState == ControlStates::kTargeting )
	{
		mTargetingReason = math::enum_bitcast<TargetingReason>( navigation.mCursorIndex.at( 0 ) );
		mTargetingIndex = navigation.mCursorIndex.at( 1 );
	}
	else if( currentState == ControlStates::kAction )
	{
		mActionMenu->SetSlotIndexWithSoftFocus( context, navigation.mCursorIndex.at( 0 ) );
	}
	else if( currentState == ControlStates::kAttack )
	{
		mAttackMenu->SetSlotIndexWithSoftFocus( context, navigation.mCursorIndex.at( 0 ) );
	}
	else if( currentState == ControlStates::kElement )
	{
		RF_TODO_BREAK();
	}
	else
	{
		RF_DBGFAIL();
	}
}



void Gameplay_Battle::InternalState::SaveUIState( ui::UIContext& context ) const
{
	// Save UI state to navigation component
	state::comp::UINavigation::BattleMenu& navigation = mNavigation->mBattleMenu;

	navigation.mControlCharIndex = mControlCharIndex;

	ControlStates::State const currentState = GetControlState( context );
	navigation.mControlState = math::integer_cast<uint8_t>( math::enum_bitcast( currentState ) );
	if( currentState == ControlStates::kWaiting )
	{
		// No cursor
	}
	else if( currentState == ControlStates::kTargeting )
	{
		navigation.mCursorIndex.at( 0 ) = math::enum_bitcast( mTargetingReason );
		navigation.mCursorIndex.at( 1 ) = mTargetingIndex;
	}
	else if( currentState == ControlStates::kAction )
	{
		navigation.mCursorIndex.at( 0 ) = math::integer_cast<uint8_t>( mActionMenu->GetSlotIndexWithSoftFocus( context ) );
	}
	else if( currentState == ControlStates::kAttack )
	{
		navigation.mCursorIndex.at( 0 ) = math::integer_cast<uint8_t>( mAttackMenu->GetSlotIndexWithSoftFocus( context ) );
	}
	else if( currentState == ControlStates::kElement )
	{
		RF_TODO_BREAK();
	}
	else
	{
		RF_DBGFAIL();
	}
}



void Gameplay_Battle::InternalState::SanitizeUIState( ui::UIContext& context )
{
	// TODO: Sanitize control state
	ControlStates::State const currentState = GetControlState( context );

	ShiftControlChar( 0 );

	if( currentState == ControlStates::kTargeting )
	{
		ShiftTarget( 0 );
	}
}



void Gameplay_Battle::InternalState::ShiftControlChar( int8_t applyOffset )
{
	mControlCharIndex = mFightController->SanitizeCharacterIndex(
		mControlCharIndex, applyOffset );
}



void Gameplay_Battle::InternalState::ShiftTarget( int8_t applyOffset )
{
	mTargetingIndex = mFightController->SanitizeAttackTargetIndex(
		mControlCharIndex, mTargetingIndex, applyOffset );
}

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Battle::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	// Find navigation component
	state::VariableIdentifier const localUIRoot( "localUI" );
	state::MutableObjectRef const localUI = state::FindMutableObjectByIdentifier( localUIRoot );
	RFLOG_TEST_AND_FATAL( localUI.IsSet(), nullptr, RFCAT_CC3O3, "Failed to find UI object" );
	internalState.mNavigation = localUI.GetMutableComponentInstanceT<state::comp::UINavigation>();
	RFLOG_TEST_AND_FATAL( internalState.mNavigation != nullptr, nullptr, RFCAT_CC3O3, "Failed to find navigation component" );

	// Setup combat instance
	{
		internalState.mFightController = DefaultCreator<combat::FightController>::Create( gCombatEngine, gCompanyManager );
		internalState.mFightController->HardcodedPlaceholderSetup();
	}

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Nine-slice the whole screen
		constexpr bool kSlicesEnabled[9] = {
			true, false, true,
			false, true, false,
			false, true, false
		};
		WeakPtr<ui::controller::NineSlicer> const rootNineSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::NineSlicer>::Create(
					kSlicesEnabled ) );

		// Environment display in upper left
		WeakPtr<ui::controller::Floater> const environmentFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					ui::Justification::TopLeft ) );
		environmentFloater->SetOffset( uiContext, { gfx::kTileSize / 4, gfx::kTileSize / 4 } );
		WeakPtr<ui::controller::TextLabel> const environmentTODO =
			uiManager.AssignStrongController(
				environmentFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		environmentTODO->SetJustification( ui::Justification::MiddleCenter );
		environmentTODO->SetFont( ui::font::LargeMenuText );
		environmentTODO->SetText( "UNSET" );
		environmentTODO->SetColor( math::Color3f::kWhite );
		environmentTODO->SetBorder( true );

		// Team display in upper right
		WeakPtr<ui::controller::Floater> const teamFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
					ui::Justification::TopRight ) );
		teamFloater->SetOffset( uiContext, { -gfx::kTileSize / 4, gfx::kTileSize / 4 } );
		WeakPtr<ui::controller::TextLabel> const teamTODO =
			uiManager.AssignStrongController(
				teamFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		teamTODO->SetJustification( ui::Justification::MiddleCenter );
		teamTODO->SetFont( ui::font::LargeMenuText );
		teamTODO->SetText( "UNSET" );
		teamTODO->SetColor( math::Color3f::kWhite );
		teamTODO->SetBorder( true );

		// Notification display in center
		WeakPtr<ui::controller::Floater> const notificationFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 4 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize ),
					ui::Justification::TopCenter ) );
		WeakPtr<ui::controller::TextLabel> const notificationTODO =
			uiManager.AssignStrongController(
				notificationFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		notificationTODO->SetJustification( ui::Justification::MiddleCenter );
		notificationTODO->SetFont( ui::font::LargeMenuText );
		notificationTODO->SetText( "UNSET" );
		notificationTODO->SetColor( math::Color3f::kWhite );
		notificationTODO->SetBorder( true );

		// Main display in bottom
		WeakPtr<ui::controller::Floater> const mainFloater =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 7 ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 9 ),
					math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
					ui::Justification::BottomCenter ) );
		mainFloater->SetOffset( uiContext, { 0, -gfx::kTileSize / 4 } );

		// Main display split between control and party
		ui::controller::ColumnSlicer::Ratios const mainColumnRatios = {
			{ 3.f / 9.f, true },
			{ 6.f / 9.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const mainColumnSlicer =
			uiManager.AssignStrongController(
				mainFloater->GetChildContainerID(),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					mainColumnRatios ) );

		// Party on right side
		ui::controller::ColumnSlicer::Ratios const partyColumnRatios = {
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
			{ 1.f / 3.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const partyColumnSlicer =
			uiManager.AssignStrongController(
				mainColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					partyColumnRatios ) );
		for( size_t i = 0; i < 3; i++ )
		{
			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					partyColumnSlicer->GetChildContainerID( i ),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" ), { 8, 8 }, { 48, 48 }, { -4, -4 } );

			// Display
			WeakPtr<ui::controller::TextLabel> const partyMemberTODO =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::TextLabel>::Create() );
			partyMemberTODO->SetJustification( ui::Justification::MiddleCenter );
			partyMemberTODO->SetFont( ui::font::LargeMenuText );
			partyMemberTODO->SetText( "UNSET" );
			partyMemberTODO->SetColor( math::Color3f::kWhite );
			partyMemberTODO->SetBorder( true );
		}

		// Control on left side
		using ControlStates = InternalState::ControlStates;
		WeakPtr<ui::controller::MultiPassthrough> const controlPassthroughs =
			uiManager.AssignStrongController(
				mainColumnSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::MultiPassthrough>::Create( ControlStates::kNumStates ) );

		// Controls each get a passthrough that can be used to shut off all
		//  children when switching control states
		WeakPtr<ui::controller::Passthrough> const waitingPassthrough =
			uiManager.AssignStrongController(
				controlPassthroughs->GetChildContainerID( ControlStates::kWaiting ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const targetingPassthrough =
			uiManager.AssignStrongController(
				controlPassthroughs->GetChildContainerID( ControlStates::kTargeting ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const actionPassthrough =
			uiManager.AssignStrongController(
				controlPassthroughs->GetChildContainerID( ControlStates::kAction ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const attackPassthrough =
			uiManager.AssignStrongController(
				controlPassthroughs->GetChildContainerID( ControlStates::kAttack ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const elementPassthrough =
			uiManager.AssignStrongController(
				controlPassthroughs->GetChildContainerID( ControlStates::kElement ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		internalState.mStateControllers.at( ControlStates::kWaiting ) = waitingPassthrough;
		internalState.mStateControllers.at( ControlStates::kTargeting ) = targetingPassthrough;
		internalState.mStateControllers.at( ControlStates::kAction ) = actionPassthrough;
		internalState.mStateControllers.at( ControlStates::kAttack ) = attackPassthrough;
		internalState.mStateControllers.at( ControlStates::kElement ) = elementPassthrough;
		waitingPassthrough->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
		targetingPassthrough->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode().GetMutableFavoredChild() );
		actionPassthrough->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode().GetMutableFavoredChild() );
		attackPassthrough->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode().GetMutableFavoredChild() );
		elementPassthrough->AddAsSiblingAfterFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode().GetMutableFavoredChild() );

		// Waiting state
		{
			// Floater
			WeakPtr<ui::controller::Floater> const floater =
				uiManager.AssignStrongController(
					waitingPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::Floater>::Create(
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
						ui::Justification::MiddleCenter ) );

			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					floater->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" ), { 8, 8 }, { 48, 48 }, { -4, -4 } );

			// Display
			WeakPtr<ui::controller::TextLabel> const label =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::TextLabel>::Create() );
			label->SetJustification( ui::Justification::MiddleCenter );
			label->SetFont( ui::font::SmallMenuText );
			label->SetText( ui::LocalizeKey( InternalState::kLabelStateWaiting ) );
			label->SetColor( math::Color3f::kGray50 );
			label->SetBorder( true );
		}

		// Targeting state
		{
			// Floater
			WeakPtr<ui::controller::Floater> const floater =
				uiManager.AssignStrongController(
					targetingPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::Floater>::Create(
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
						ui::Justification::MiddleCenter ) );

			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					floater->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" ), { 8, 8 }, { 48, 48 }, { -4, -4 } );

			// Display
			WeakPtr<ui::controller::TextLabel> const label =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::TextLabel>::Create() );
			label->SetJustification( ui::Justification::MiddleCenter );
			label->SetFont( ui::font::SmallMenuText );
			label->SetText( "TODO: TARGET" );
			label->SetColor( math::Color3f::kGray50 );
			label->SetBorder( true );
		}

		// Action state
		{
			// Floater
			WeakPtr<ui::controller::Floater> const floater =
				uiManager.AssignStrongController(
					actionPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::Floater>::Create(
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
						ui::Justification::MiddleCenter ) );

			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					floater->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" ), { 8, 8 }, { 48, 48 }, { -4, -4 } );

			// List
			using SelectorActions = InternalState::SelectorActions;
			WeakPtr<ui::controller::ListBox> const actionMenu =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						math::enum_bitcast( SelectorActions::kNumActions ),
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rftl::vector<rftl::string> text;
			for( char const* const& label : SelectorActions::kLabels )
			{
				text.emplace_back( ui::LocalizeKey( label ) );
			}
			actionMenu->SetText( text );
			actionMenu->AddAsChildToFocusTreeNode( uiContext, *actionPassthrough->GetMutableFocusTreeNode( uiContext ) );
			internalState.mActionMenu = actionMenu;
		}

		// Attack state
		{
			// Floater
			WeakPtr<ui::controller::Floater> const floater =
				uiManager.AssignStrongController(
					attackPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::Floater>::Create(
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 2 ),
						math::integer_cast<gfx::PPUCoordElem>( gfx::kTileSize * 3 / 2 ),
						ui::Justification::MiddleCenter ) );

			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					floater->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "flat1_8_48" ), { 8, 8 }, { 48, 48 }, { -4, -4 } );

			// List
			using SelectorAttacks = InternalState::SelectorAttacks;
			WeakPtr<ui::controller::ListBox> const attackMenu =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						SelectorAttacks::kNumActions,
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rftl::vector<rftl::string> text = {
				"1",
				"2",
				"3",
				ui::LocalizeKey( InternalState::kLabelAttackElement )
			};
			attackMenu->SetText( text );
			attackMenu->AddAsChildToFocusTreeNode( uiContext, *attackPassthrough->GetMutableFocusTreeNode( uiContext ) );
			internalState.mAttackMenu = attackMenu;
		}

		// TODO: Element state

		// TODO: Sanitize UI state against battle state on tick start instead
		internalState.SwitchControlState( uiContext, ControlStates::kAction );
		internalState.SaveUIState( uiContext );
	}
}



void Gameplay_Battle::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Battle::OnTick( AppStateTickContext& context )
{
	using ControlState = InternalState::ControlStates::State;
	using TargetingReason = InternalState::TargetingReason;

	InternalState& internalState = *mInternalState;
	//state::comp::UINavigation& navigation = *internalState.mNavigation;
	gfx::PPUController& ppu = *app::gGraphics;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	ui::UIContext uiContext( uiManager );
	combat::FightController& fightController = *internalState.mFightController;

	ppu.DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Battle" );

	// Rollback may have triggered, restore UI to stay in sync
	internalState.RestoreUIState( uiContext );
	internalState.SanitizeUIState( uiContext );

	focusMan.UpdateHardFocus( uiContext );
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetGameMenuInputToProcess( input::HardcodedGetLocalPlayer() );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		focusMan.UpdateHardFocus( uiContext );

		if( handled == false )
		{
			// Wasn't handled by general UI

			ControlState const controlState = internalState.GetControlState( uiContext );
			if( controlState == ControlState::kWaiting )
			{
				// Waiting

				// TODO: (If enemy turn) Check for time counter input? Or
				//  should that be on a different input layer than the menus?

				// TODO: (If own turn) How can this happen?
			}
			else if( controlState == ControlState::kTargeting )
			{
				// Targeting

				if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
				{
					if( internalState.mTargetingReason == TargetingReason::kAttack )
					{
						// Ascend to action menu
						internalState.SwitchControlState( uiContext, ControlState::kAction );
					}
					else if( internalState.mTargetingReason == TargetingReason::kElement )
					{
						// Ascend to element menu
						internalState.SwitchControlState( uiContext, ControlState::kElement );
					}
					else
					{
						RF_DBGFAIL();
					}
				}
				else if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
				{
					if( internalState.mTargetingReason == TargetingReason::kAttack )
					{
						bool const canAttack = fightController.CanCharacterPerformAttack(
							internalState.mControlCharIndex,
							internalState.mTargetingIndex );
						if( canAttack )
						{
							// Descend to attack menu
							internalState.SwitchControlState( uiContext, ControlState::kAttack );
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot attack target" );
						}
					}
					else if( internalState.mTargetingReason == TargetingReason::kElement )
					{
						// TODO: Cast element, ascend to action menu, navigate
						//  to next character
						internalState.SwitchControlState( uiContext, ControlState::kAction );
						internalState.ShiftControlChar( 1 );
					}
					else
					{
						RF_DBGFAIL();
					}
				}
				else if( focusEvent == ui::focusevent::Command_NavigateLeft || focusEvent == ui::focusevent::Command_NavigateRight )
				{
					// Change the current target
					bool const next = focusEvent == ui::focusevent::Command_NavigateRight;
					internalState.ShiftTarget( next ? 1 : -1 );
				}
			}
			else if( controlState == ControlState::kAction )
			{
				// Action

				if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
				{
					using Action = InternalState::SelectorActions::Action;
					size_t const action = internalState.mActionMenu->GetSlotIndexWithSoftFocus( uiContext );
					if( action == Action::kAttack )
					{
						bool const canAttack = fightController.CanCharacterPerformAttack(
							internalState.mControlCharIndex );
						if( canAttack )
						{
							// Descend to targeting
							internalState.mTargetingReason = TargetingReason::kAttack;
							internalState.SwitchControlState( uiContext, ControlState::kTargeting );
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot perform an attack" );
						}
					}
					else if( action == Action::kElement )
					{
						bool const canCast = fightController.CanCharacterCastElement(
							internalState.mControlCharIndex );
						if( canCast )
						{
							// Descend to element menu
							internalState.SwitchControlState( uiContext, ControlState::kElement );
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot cast an element" );
						}
					}
					else if( action == Action::kDefend )
					{
						bool const canDefend = fightController.CanCharacterActivateDefense(
							internalState.mControlCharIndex );
						if( canDefend )
						{
							// TODO: Execute defense, navigate to next character
							internalState.ShiftControlChar( 1 );
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot activate defense" );
						}
					}
					else if( action == Action::kWait )
					{
						bool const canEndTurn = fightController.CanPartyEndTurn();
						if( canEndTurn )
						{
							// TODO: End turn
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Party cannot end turn" );
						}
					}
					else
					{
						RF_DBGFAIL();
					}
				}
				else if( focusEvent == ui::focusevent::Command_NavigateLeft || focusEvent == ui::focusevent::Command_NavigateRight )
				{
					// Navigate to other character
					bool const next = focusEvent == ui::focusevent::Command_NavigateRight;
					internalState.ShiftControlChar( next ? 1 : -1 );
				}
			}
			else if( controlState == ControlState::kAttack )
			{
				// Attack

				if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
				{
					// Ascend to action menu
					internalState.SwitchControlState( uiContext, ControlState::kAction );
				}
				else if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
				{
					using AttackChoice = InternalState::SelectorAttacks::Action;
					size_t const attackChoice = internalState.mAttackMenu->GetSlotIndexWithSoftFocus( uiContext );
					if( attackChoice < AttackChoice::kNumAttacks )
					{
						bool const canAttack = fightController.CanCharacterPerformAttack(
							internalState.mControlCharIndex,
							internalState.mTargetingIndex,
							math::integer_cast<uint8_t>( attackChoice ) );
						if( canAttack )
						{
							// TODO: Execute attack
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot execute attack" );
						}
					}
					else if( attackChoice == AttackChoice::kElement )
					{
						bool const canCast = fightController.CanCharacterCastElement(
							internalState.mControlCharIndex );
						if( canCast )
						{
							// Descend to element menu
							internalState.SwitchControlState( uiContext, ControlState::kElement );
						}
						else
						{
							RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Character cannot cast an element" );
						}
					}
					else
					{
						RF_DBGFAIL();
					}
				}
				else if( focusEvent == ui::focusevent::Command_NavigateLeft || focusEvent == ui::focusevent::Command_NavigateRight )
				{
					// Navigate to other character, ascend to action menu
					internalState.SwitchControlState( uiContext, ControlState::kAction );
					bool const next = focusEvent == ui::focusevent::Command_NavigateRight;
					internalState.ShiftControlChar( next ? 1 : -1 );
				}
			}
			else if( controlState == ControlState::kElement )
			{
				// Element

				if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
				{
					// Ascend to action menu
					internalState.SwitchControlState( uiContext, ControlState::kAction );
				}
				else
				{
					// TODO
				}
			}
			else
			{
				RF_DBGFAIL();
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}

	// Persist in case of rollback
	internalState.SaveUIState( uiContext );

	combat::CombatInstance const& mainInstance = *internalState.mFightController->GetCombatInstance();

	// HACK: Stub battle data
	{
		static constexpr gfx::PPUCoordElem x = 64;
		static constexpr gfx::PPUCoordElem yStep = gfx::kTileSize / 4;
		gfx::PPUCoordElem y = 32 + yStep;

		combat::CombatInstance::PartyIDs const partyIDs = mainInstance.GetPartyIDs();
		for( combat::PartyID const& partyID : partyIDs )
		{
			combat::Party const party = mainInstance.GetParty( partyID );
			ppu.DebugDrawText( { x, y },
				"%i:%i  %03i/%03i",
				partyID.GetTeamIndex(),
				partyID.GetPartyIndex(),
				party.mCounterGuage,
				combat::kCounterGaugeFull );
			y += yStep;
		}

		combat::CombatInstance::FighterIDs const fighterIDs = mainInstance.GetFighterIDs();
		for( combat::FighterID const& fighterID : fighterIDs )
		{
			combat::Fighter const fighter = mainInstance.GetFighter( fighterID );
			char marker = ' ';
			if( fighterID == fightController.GetCharacterByIndex( internalState.mControlCharIndex ) )
			{
				marker = '*';
			}
			else if( fighterID == fightController.GetAttackTargetByIndex( internalState.mControlCharIndex, internalState.mTargetingIndex ) )
			{
				marker = 'X';
			}
			ppu.DebugDrawText( { x, y },
				"%c%i:%i:%i  %03i/%03i  %1i/%1i  %2i -> %i:%i:%i",
				marker,
				fighterID.GetTeamIndex(),
				fighterID.GetPartyIndex(),
				fighterID.GetFighterIndex(),
				fighter.mCurHealth,
				fighter.mMaxHealth,
				fighter.mCurStamina,
				fighter.mMaxStamina,
				fighter.mComboMeter,
				fighter.mComboTarget.GetTeamIndex(),
				fighter.mComboTarget.GetPartyIndex(),
				fighter.mComboTarget.GetFighterIndex() );
			y += yStep;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

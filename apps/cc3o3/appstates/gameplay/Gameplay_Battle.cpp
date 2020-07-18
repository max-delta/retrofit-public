#include "stdafx.h"
#include "Gameplay_Battle.h"

#include "cc3o3/Common.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/state/StateHelpers.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/UINavigation.h"
#include "cc3o3/ui/UIFwd.h"

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
	InternalState() = default;

public:
	struct ControlStates
	{
		enum State : size_t
		{
			Waiting = 0,
			Action,
			Attack,
			Element,

			kNumStates
		};
	};

	struct SelectorActions
	{
		enum Action : size_t
		{
			Attack = 0,
			Element,
			Defend,
			Wait,

			kNumStates
		};

		static constexpr char const* kLabels[kNumStates] = {
			"$battle_action_attack",
			"$battle_action_element",
			"$battle_action_defend",
			"$battle_action_wait",
		};
	};


public:
	UniquePtr<combat::CombatInstance> mCombatInstance;
	combat::PartyID mLocalPlayerParty;

	WeakPtr<state::comp::UINavigation> mNavigation;
};

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
		using namespace combat;

		internalState.mCombatInstance = DefaultCreator<CombatInstance>::Create( gCombatEngine );
		CombatInstance& instance = *internalState.mCombatInstance;

		// Setup players
		// HACK: One player
		// TODO: Multiplayer, opposing teams
		// TODO: Multiplayer, same team
		rftl::vector<input::PlayerID> const playerIDs = { input::HardcodedGetLocalPlayer() };
		for( input::PlayerID const& playerID : playerIDs )
		{
			TeamIndex const playerTeam = instance.AddTeam();
			PartyID const playerParty = instance.AddParty( playerTeam );
			if( playerID == input::HardcodedGetLocalPlayer() )
			{
				internalState.mLocalPlayerParty = playerParty;
			}

			// Get the active party characters
			rftl::array<state::MutableObjectRef, 3> const activePartyCharacters =
				gCompanyManager->FindMutableActivePartyObjects( input::HardcodedGetLocalPlayer() );

			// For each active team member...
			for( size_t i_teamIndex = 0; i_teamIndex < company::kActiveTeamSize; i_teamIndex++ )
			{
				state::MutableObjectRef const& character = activePartyCharacters.at( i_teamIndex );
				if( character.IsSet() == false )
				{
					// Not active
					continue;
				}

				// Add to party
				CombatantID const combatant = instance.AddFighter( playerParty );
				instance.SetCombatant( combatant, character );
			}
		}

		// Setup NPCs
		// HACK: Hard-coded
		// TODO: Encounters
		if constexpr( true )
		{
			TeamIndex const enemyTeam = instance.AddTeam();
			PartyID const enemyParty = instance.AddParty( enemyTeam );

			rftl::array<Combatant, 1> enemies = {};
			{
				Combatant& enemy = enemies.at( 0 );
				enemy.mInnate = element::MakeInnateIdentifier( element::InnateString{ 'r', 'e', 'd' } );
				enemy.mMaxHealth = 999;
				enemy.mCurHealth = enemy.mMaxHealth;
				enemy.mMaxStamina = 7;
				enemy.mCurStamina = enemy.mMaxStamina;
				enemy.mPhysAtk = 2;
				enemy.mPhysDef = 2;
				enemy.mElemAtk = 2;
				enemy.mElemDef = 2;
				enemy.mBalance = 2;
				enemy.mTechniq = 2;

				CombatantID const enemyID = instance.AddFighter( enemyParty );
				instance.SetCombatant( enemyID, enemy );
			}
		}

		// Setup initial field influence
		// HACK: Hard-coded
		// TODO: Encounter specified override or multiplayer-minded hash source
		instance.GenerateFieldInfluence( 0 );
	}

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		//ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
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

		// TODO: Waiting state

		// Action state
		{
			// Floater
			WeakPtr<ui::controller::Floater> const floater =
				uiManager.AssignStrongController(
					controlPassthroughs->GetChildContainerID( math::enum_bitcast( ControlStates::Action ) ),
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
			WeakPtr<ui::controller::ListBox> const actionControls =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						math::enum_bitcast( ControlStates::kNumStates ),
						ui::font::SmallMenuSelection,
						ui::Justification::MiddleLeft,
						math::Color3f::kGray50,
						math::Color3f::kWhite,
						math::Color3f::kYellow ) );
			rftl::vector<rftl::string> text;
			for( char const* const& label : SelectorActions::kLabels )
			{
				text.emplace_back( label );
			}
			actionControls->SetText( text );
		}

		// TODO: Attack state
		// TODO: Element state
	}
}



void Gameplay_Battle::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Battle::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	//state::comp::UINavigation& navigation = *internalState.mNavigation;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugDrawText( gfx::PPUCoord( 32, 32 ), "TODO: Battle" );

	combat::CombatInstance& mainInstance = *internalState.mCombatInstance;

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
				partyID.mTeam,
				partyID.mParty,
				party.mCounterGuage,
				combat::kCounterGaugeFull );
			y += yStep;
		}

		combat::CombatInstance::CombatantIDs const combatantIDs = mainInstance.GetCombatantIDs();
		for( combat::CombatantID const& combatantID : combatantIDs )
		{
			combat::Combatant const combatant = mainInstance.GetCombatant( combatantID );
			ppu.DebugDrawText( { x, y },
				"%i:%i:%i  %03i/%03i  %1i/%1i  %2i -> %i:%i:%i",
				combatantID.mTeam,
				combatantID.mParty,
				combatantID.mFighter,
				combatant.mCurHealth,
				combatant.mMaxHealth,
				combatant.mCurStamina,
				combatant.mMaxStamina,
				combatant.mComboMeter,
				combatant.mComboTarget.mTeam,
				combatant.mComboTarget.mParty,
				combatant.mComboTarget.mFighter );
			y += yStep;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

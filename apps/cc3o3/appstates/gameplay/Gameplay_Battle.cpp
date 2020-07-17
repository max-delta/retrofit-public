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
#include "GameUI/controllers/NineSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"

#include "core_component/TypedObjectRef.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Battle::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	UniquePtr<combat::CombatInstance> mCombatInstance;
	combat::PartyID mLocalPlayerParty;

	WeakPtr<state::comp::UINavigation> mNavigation;
};

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Battle::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

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
		//ui::UIContext uiContext( uiManager );
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
		WeakPtr<ui::controller::TextLabel> const environmentTODO =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		environmentTODO->SetJustification( ui::Justification::TopLeft );
		environmentTODO->SetFont( ui::font::LargeMenuText );
		environmentTODO->SetText( "UNSET" );
		environmentTODO->SetColor( math::Color3f::kWhite );
		environmentTODO->SetBorder( true );

		// Team display in upper right
		WeakPtr<ui::controller::TextLabel> const teamTODO =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		teamTODO->SetJustification( ui::Justification::TopRight );
		teamTODO->SetFont( ui::font::LargeMenuText );
		teamTODO->SetText( "UNSET" );
		teamTODO->SetColor( math::Color3f::kWhite );
		teamTODO->SetBorder( true );

		// Notification display in center
		WeakPtr<ui::controller::TextLabel> const notificationTODO =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 4 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		notificationTODO->SetJustification( ui::Justification::MiddleCenter );
		notificationTODO->SetFont( ui::font::LargeMenuText );
		notificationTODO->SetText( "UNSET" );
		notificationTODO->SetColor( math::Color3f::kWhite );
		notificationTODO->SetBorder( true );

		// Control display in bottom
		WeakPtr<ui::controller::TextLabel> const controlTODO =
			uiManager.AssignStrongController(
				rootNineSlicer->GetChildContainerID( 7 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		controlTODO->SetJustification( ui::Justification::BottomCenter );
		controlTODO->SetFont( ui::font::LargeMenuText );
		controlTODO->SetText( "UNSET" );
		controlTODO->SetColor( math::Color3f::kWhite );
		controlTODO->SetBorder( true );
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

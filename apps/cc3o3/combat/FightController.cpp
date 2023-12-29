#include "stdafx.h"
#include "FightController.h"

#include "cc3o3/casting/CastError.h"
#include "cc3o3/combat/Attack.h"
#include "cc3o3/combat/CombatInstance.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Loadout.h"
#include "cc3o3/state/StateLogging.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////
namespace details {

uint8_t SanitizeFighterIndex( CombatInstance::FighterIDs const& ids, uint8_t index, int8_t applyOffset )
{
	RF_ASSERT( ids.empty() == false );

	// Convert to indices
	using Indices = rftl::static_vector<uint8_t, CombatInstance::FighterIDs::fixed_capacity>;
	Indices indices;
	for( FighterID const& id : ids )
	{
		indices.emplace_back( id.GetFighterIndex() );
	}
	RF_ASSERT( rftl::is_sorted( indices.begin(), indices.end() ) );

	// Find current position in list
	Indices::const_iterator iter = rftl::lower_bound( indices.begin(), indices.end(), index );
	if( iter == indices.end() )
	{
		// Loop around
		iter = indices.begin();
	}
	uint8_t const currentOffset = math::integer_cast<uint8_t>( rftl::distance( indices.cbegin(), iter ) );

	// Apply offset and return lookup
	uint8_t const targetOffset = math::WrapPositiveOffset<uint8_t>(
		currentOffset,
		math::integer_cast<uint8_t>( indices.size() ),
		applyOffset );
	return indices.at( targetOffset );
}



element::ElementIdentifier GetElementIdentiferFromSlotIndex( state::ObjectRef const& character, character::ElementSlotIndex elementSlotIndex )
{
	state::ComponentInstanceRefT<state::comp::Loadout> const loadout =
		character.GetComponentInstanceT<state::comp::Loadout>();
	RFLOG_TEST_AND_FATAL( loadout != nullptr, character, RFCAT_CC3O3, "Missing loadout component" );
	element::ElementIdentifier const elementIdentifier =
		loadout->mEquippedElements.At( elementSlotIndex );
	return elementIdentifier;
}

}
///////////////////////////////////////////////////////////////////////////////

FightController::FightController( WeakPtr<cast::CastingEngine const> castingEngine )
	: mCastingEngine( castingEngine )
{
	//
}



WeakPtr<CombatInstance const> FightController::GetCombatInstance() const
{
	return mCombatInstance;
}



void FightController::SetupFromCombatInstance( UniquePtr<combat::CombatInstance>&& setup, PartyID localPartyID )
{
	RF_ASSERT( mFrameActive == false );

	RF_ASSERT( setup != nullptr );
	mCombatInstance = rftl::move( setup );

	RF_ASSERT( localPartyID.IsValid() );
	mLocalPartyID = localPartyID;

	// Initial save
	mCombatInstance->CommitCombatData();
}



void FightController::StartCombatFrame()
{
	RF_ASSERT( mFrameActive == false );

	mCombatInstance->ReloadCombatData();
	// TODO: Load attack buffer
	RF_TODO_ANNOTATION( "Rollback support for attack buffer" );
	// TODO: Load cast buffer
	RF_TODO_ANNOTATION( "Rollback support for cast buffer" );

	mFrameActive = true;
}



void FightController::EndCombatFrame()
{
	RF_ASSERT( mFrameActive );

	mCombatInstance->CommitCombatData();
	// TODO: Save attack buffer
	RF_TODO_ANNOTATION( "Rollback support for attack buffer" );
	// TODO: Save cast buffer
	RF_TODO_ANNOTATION( "Rollback support for cast buffer" );

	mFrameActive = false;
}



bool FightController::HasPendingActions() const
{
	RF_ASSERT( mFrameActive );

	if( mAttackBuffer.empty() == false )
	{
		return true;
	}

	if( mCastBuffer != nullptr )
	{
		return true;
	}

	return false;
}



void FightController::TickPendingActions()
{
	RF_ASSERT( mFrameActive );

	// TODO: Actions have animations and other delays, which need to be handled
	// NOTE: Probably want a seperate system to handle that aspect, since it
	//  needs to work for other parties as well. Ownership of the main combat
	//  instance will probably need to be moved into that new system.
	RF_TODO_ANNOTATION( "Integration with a display / animation system" );

	CombatInstance& combatInstance = *mCombatInstance;

	if( mAttackBuffer.empty() == false )
	{
		// Pop front
		BufferedAttack const attack = mAttackBuffer.front();
		mAttackBuffer.erase( mAttackBuffer.begin() );

		if( combatInstance.CanPerformAttack( attack.mAttackerID, attack.mDefenderID, attack.mAttackStrength ) )
		{
			// Attack
			AttackResult const result = combatInstance.ExecuteAttack( attack.mAttackerID, attack.mDefenderID, attack.mAttackStrength );

			// TODO: Record to some 'recent attacks' buffer for displaying
			// NOTE: Will likely be the responsibility of whatever system is
			//  monitoring and executing the delays and animations, since we
			//  need to display this for other parties as well
			RF_TODO_ANNOTATION( "Mechanism to hand over attacks to a display / animation system" );
			( (void)result );
		}
		else
		{
			RFLOG_WARNING( nullptr, RFCAT_CC3O3, "Buffered attack invalid, cancelling all attacks" );
			RF_DBGFAIL_MSG( "Bad attack. How did this happen? Fix, or flag as benign and document behavior." );
			mAttackBuffer.clear();
		}
	}

	// HACK: Make casts take a while
	RF_TODO_ANNOTATION( "Implement animation system" );
	if( mHACK_ArtificalCastDelay > 0 )
	{
		mHACK_ArtificalCastDelay--;
		return;
	}

	// Only do casts after attacks have finished
	if( mAttackBuffer.empty() && mCastBuffer != nullptr )
	{
		// Pop cast from buffer and apply it
		// NOTE: Likely pointless optimization consideration - Is it better to
		//  transfer pointer or stomp instance via copy?
		mCombatInstance = rftl::move( mCastBuffer );
		RF_ASSERT( mCastBuffer == nullptr );

		// TODO: Record to some 'recent casts' buffer for displaying
		// NOTE: Will likely be the responsibility of whatever system is
		//  monitoring and executing the delays and animations, since we
		//  need to display this for other parties as well
		RF_TODO_ANNOTATION( "Mechanism to hand over casts to a display / animation system" );
	}
}



uint8_t FightController::SanitizeCharacterIndex( uint8_t attackerIndex, int8_t applyOffset ) const
{
	CombatInstance::FighterIDs const characters = mCombatInstance->GetFighterIDs( mLocalPartyID );
	return details::SanitizeFighterIndex( characters, attackerIndex, applyOffset );
}



uint8_t FightController::SanitizeAttackTargetIndex( uint8_t attackerIndex, uint8_t defenderIndex, int8_t applyOffset ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	CombatInstance::FighterIDs const attackTargets = mCombatInstance->GetValidAttackTargets( attacker );
	return details::SanitizeFighterIndex( attackTargets, defenderIndex, applyOffset );
}



uint8_t FightController::SanitizeCastTargetIndex( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex, int8_t applyOffset ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	element::ElementIdentifier const elementIdentifier = GetElementIdentifierBySlotIndex( attacker, elementSlotIndex );
	cast::CastingEngine::FighterIDs const castTargets =
		mCastingEngine->GetValidCastTargets(
			*mCombatInstance,
			attacker,
			elementIdentifier );
	return details::SanitizeFighterIndex( castTargets, defenderIndex, applyOffset );
}



PartyID FightController::GetLocalPartyID() const
{
	return mLocalPartyID;
}



FighterID FightController::GetCharacterByIndex( uint8_t attackerIndex ) const
{
	return mLocalPartyID.GetFighterAt( attackerIndex );
}



FighterID FightController::GetAttackTargetByIndex( uint8_t attackerIndex, uint8_t defenderIndex ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	CombatInstance::FighterIDs const attackTargets = mCombatInstance->GetValidAttackTargets( attacker );
	RF_ASSERT( defenderIndex < attackTargets.size() );
	return attackTargets.at( defenderIndex );
}



FighterID FightController::GetCastTargetByIndex( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex ) const
{
	FighterID const attacker = GetCharacterByIndex( attackerIndex );
	element::ElementIdentifier const elementIdentifier = GetElementIdentifierBySlotIndex( attacker, elementSlotIndex );
	cast::CastingEngine::FighterIDs const castTargets =
		mCastingEngine->GetValidCastTargets(
			*mCombatInstance,
			attacker,
			elementIdentifier );
	RF_ASSERT( defenderIndex < castTargets.size() );
	return castTargets.at( defenderIndex );
}



bool FightController::CanCharacterPerformAttack() const
{
	if( mCastBuffer != nullptr )
	{
		// Don't allow attacks to buffer behind casts
		// NOTE: This is a bit quirky, since a character could still
		//  theoretically attack directly after a cast, and we could buffer
		//  those attacks, but it would require interleaving buffers, and the
		//  current combat design is such that a character should always be at
		//  zero or negative stamina after a cast, and thus be unable to act
		return false;
	}

	return true;
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex ) const
{
	if( CanCharacterPerformAttack() == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	return mCombatInstance->CanPerformAttack( attackerID );
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const
{
	if( CanCharacterPerformAttack( attackerIndex ) == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );
	return mCombatInstance->CanPerformAttack( attackerID, defenderID );
}



bool FightController::CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const
{
	if( CanCharacterPerformAttack( attackerIndex, defenderIndex ) == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );

	if( mAttackBuffer.empty() )
	{
		return mCombatInstance->CanPerformAttack( attackerID, defenderID, attackStrength );
	}
	else
	{
		// Attacks are queued, simulate them along with this additional attack
		CombatInstance simulation = *mCombatInstance;
		bool const simSuccess = SimulateAttackBuffer( simulation, attackerID, defenderID, attackStrength );
		return simSuccess;
	}
}



void FightController::PredictAttack( AttackProfile& profile, AttackResult& result, uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const
{
	RF_ASSERT( CanCharacterPerformAttack( attackerIndex, defenderIndex, attackStrength ) );

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );

	CombatInstance simulation = *mCombatInstance;
	if( mAttackBuffer.empty() == false )
	{
		bool const simSuccess = SimulateAttackBuffer( simulation, attackerID, defenderID );

		// The CanCharacterPerformAttack(...) check should already cover this
		RF_ASSERT( simSuccess );
	}

	// The CanCharacterPerformAttack(...) check should already cover this
	RF_ASSERT( simulation.CanPerformAttack( attackerID, defenderID, attackStrength ) );

	profile = simulation.PrepareAttack( attackerID, defenderID, attackStrength );
	result = simulation.ExecuteAttack( attackerID, defenderID, attackStrength );
}



void FightController::BufferAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength )
{
	RF_ASSERT( mFrameActive );

	// TODO: This buffering needs to be re-designed to be stored into a
	//  rollback domain, probably something like:
	//  * buffered:Attacker
	//  * buffered:Defender
	//  * buffered:Attack:0
	//  * buffered:Attack:1
	//  * buffered:Attack:2 (Or whatever limit is reasonable)
	RF_TODO_ANNOTATION( "Rollback support for attack buffer" );

	RF_ASSERT( CanCharacterPerformAttack( attackerIndex, defenderIndex, attackStrength ) );
	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetAttackTargetByIndex( attackerIndex, defenderIndex );
	if( mAttackBuffer.empty() == false )
	{
		RF_ASSERT( mAttackBuffer.front().mAttackerID == attackerID );
		RF_ASSERT( mAttackBuffer.front().mDefenderID == defenderID );
	}
	mAttackBuffer.emplace_back( { attackerID, defenderID, attackStrength } );
}



bool FightController::CanCharacterCastElement( uint8_t attackerIndex ) const
{
	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	return mCombatInstance->CanPerformCast( attackerID );
}



bool FightController::CanCharacterCastElement( uint8_t attackerIndex, element::ElementLevel castedLevel ) const
{
	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	return mCombatInstance->CanPerformCast( attackerID, castedLevel );
}



bool FightController::CanCharacterCastElement( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex ) const
{
	if( CanCharacterCastElement( attackerIndex, elementSlotIndex.first ) == false )
	{
		return false;
	}

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	if( mCombatInstance->IsSlotLocked( attackerID, elementSlotIndex ) )
	{
		// Slot is locked out (likely it was a one-time cast that's been used)
		return false;
	}

	// What are we actually casting?
	element::ElementIdentifier const elementIdentifier =
		GetElementIdentifierBySlotIndex( attackerID, elementSlotIndex );
	if( elementIdentifier == element::kInvalidElementIdentifier )
	{
		// Slot element isn't valid
		return false;
	}

	return true;
}



bool FightController::CanCharacterCastElement( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex ) const
{
	if( CanCharacterCastElement( attackerIndex, elementSlotIndex ) == false )
	{
		return false;
	}

	FighterID const defenderID = GetCastTargetByIndex( attackerIndex, elementSlotIndex, defenderIndex );
	if( defenderID.IsValid() == false )
	{
		// Target isn't valid
		// NOTE: It is possible that there are no valid targets for the chosen
		//  element at this time
		return false;
	}

	return true;
}



UniquePtr<cast::CastError> FightController::PredictCast( UniquePtr<CombatInstance>& result, uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex ) const
{
	result = nullptr;

	FighterID const attackerID = GetCharacterByIndex( attackerIndex );
	FighterID const defenderID = GetCastTargetByIndex( attackerIndex, elementSlotIndex, defenderIndex );
	element::ElementLevel const& castedLevel = elementSlotIndex.first;
	element::ElementIdentifier const elementIdentifier =
		GetElementIdentifierBySlotIndex( attackerID, elementSlotIndex );

	if( HasPendingActions() )
	{
		RF_TODO_BREAK_MSG(
			"Simulate pending actions (attacks?) into temp instance before cast" );
		RF_RETAIL_FATAL_MSG( "HasPendingActions()",
			"Unimplemented TODO with buffered attacks, cast will desync" );
	}

	// Create a temp clone instance and cast onto that
	UniquePtr<CombatInstance> tempInstance = DefaultCreator<CombatInstance>::Create( *mCombatInstance );
	UniquePtr<cast::CastError> castError =
		mCastingEngine->ExecuteElementCast(
			*tempInstance,
			attackerID,
			defenderID,
			elementIdentifier,
			castedLevel );
	if( castError != nullptr )
	{
		return castError;
	}

	// Success!
	result = rftl::move( tempInstance );
	return cast::CastError::kNoError;
}



UniquePtr<cast::CastError> FightController::BufferCast( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex )
{
	// Run the cast via the prediction code
	UniquePtr<CombatInstance> tempInstance = nullptr;
	UniquePtr<cast::CastError> castError =
		PredictCast(
			tempInstance,
			attackerIndex,
			elementSlotIndex,
			defenderIndex );
	if( castError != nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_CC3O3, "Cast error occurred on a buffer attempt" );
		return castError;
	}

	// HACK: Make casts take a while
	RF_TODO_ANNOTATION( "Implement animation system" );
	mHACK_ArtificalCastDelay = 15;

	// Buffer the temp instance
	RF_ASSERT( mCastBuffer == nullptr );
	mCastBuffer = rftl::move( tempInstance );
	return cast::CastError::kNoError;
}



bool FightController::CanCharacterActivateDefense( uint8_t attackerIndex ) const
{
	// TODO
	RF_TODO_ANNOTATION( "Implement defense logic" );
	return false;
}



bool FightController::CanPartyEndTurn() const
{
	// TODO
	RF_TODO_ANNOTATION( "Implement end turn logic" );
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool FightController::SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID ) const
{
	RF_ASSERT( mAttackBuffer.empty() == false );

	for( BufferedAttack const& attack : mAttackBuffer )
	{
		RF_ASSERT( attack.mAttackerID == attackerID );
		RF_ASSERT( attack.mDefenderID == defenderID );
		if( simulation.CanPerformAttack( attackerID, defenderID, attack.mAttackStrength ) == false )
		{
			return false;
		}
		simulation.ExecuteAttack( attackerID, defenderID, attack.mAttackStrength );
	}
	return true;
}



bool FightController::SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID, uint8_t additionalAttackStrength ) const
{
	RF_ASSERT( mAttackBuffer.empty() == false );

	// Buffered attacks
	bool const bufferSuccessful = SimulateAttackBuffer( simulation, attackerID, defenderID );
	if( bufferSuccessful == false )
	{
		return false;
	}

	// Additional attack
	if( simulation.CanPerformAttack( attackerID, defenderID, additionalAttackStrength ) == false )
	{
		return false;
	}
	simulation.ExecuteAttack( attackerID, defenderID, additionalAttackStrength );
	return true;
}



element::ElementIdentifier FightController::GetElementIdentifierBySlotIndex( FighterID fighterID, character::ElementSlotIndex elementSlotIndex ) const
{
	state::ObjectRef const character = mCombatInstance->GetCharacter( fighterID );
	element::ElementIdentifier const elementIdentifier =
		details::GetElementIdentiferFromSlotIndex( character, elementSlotIndex );
	return elementIdentifier;
}

///////////////////////////////////////////////////////////////////////////////
}

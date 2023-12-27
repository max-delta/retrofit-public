#pragma once
#include "project.h"

#include "cc3o3/casting/CastingEngine.h"
#include "cc3o3/char/CharFwd.h"
#include "cc3o3/combat/CombatantID.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/extension/static_vector.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

// Manages logic for controlling a local party
// NOTE: Most commands are 'local party'-centric
class FightController
{
	RF_NO_COPY( FightController );

	//
	// Types and constants
private:
	struct BufferedAttack
	{
		FighterID mAttackerID = {};
		FighterID mDefenderID = {};
		uint8_t mAttackStrength = 0;
	};
	static constexpr size_t kMaxBufferedAttacks = 7;
	using AttackBuffer = rftl::static_vector<BufferedAttack, kMaxBufferedAttacks>;


	//
	// Public methods
public:
	explicit FightController( WeakPtr<cast::CastingEngine const> castingEngine );

	WeakPtr<CombatInstance const> GetCombatInstance() const;

	void SetupFromCombatInstance( UniquePtr<combat::CombatInstance>&& setup, PartyID localPartyID );

	void StartCombatFrame();
	void EndCombatFrame();

	bool HasPendingActions() const;
	void TickPendingActions();

	uint8_t SanitizeCharacterIndex( uint8_t attackerIndex, int8_t applyOffset ) const;
	uint8_t SanitizeAttackTargetIndex( uint8_t attackerIndex, uint8_t defenderIndex, int8_t applyOffset ) const;
	uint8_t SanitizeCastTargetIndex( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex, int8_t applyOffset ) const;

	PartyID GetLocalPartyID() const;
	FighterID GetCharacterByIndex( uint8_t attackerIndex ) const;
	FighterID GetAttackTargetByIndex( uint8_t attackerIndex, uint8_t defenderIndex ) const;
	FighterID GetCastTargetByIndex( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex ) const;

	bool CanCharacterPerformAttack() const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const;
	void PredictAttack( AttackProfile& profile, AttackResult& result, uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const;
	void BufferAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength );

	bool CanCharacterCastElement( uint8_t attackerIndex ) const;
	bool CanCharacterCastElement( uint8_t attackerIndex, element::ElementLevel castedLevel ) const;
	bool CanCharacterCastElement( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex ) const;
	bool CanCharacterCastElement( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex ) const;
	bool BufferCast( uint8_t attackerIndex, character::ElementSlotIndex elementSlotIndex, uint8_t defenderIndex );

	bool CanCharacterActivateDefense( uint8_t attackerIndex ) const;

	bool CanPartyEndTurn() const;


	//
	// Private methods
private:
	bool SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID ) const;
	bool SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID, uint8_t additionalAttackStrength ) const;

	element::ElementIdentifier GetElementIdentifierBySlotIndex( FighterID fighterID, character::ElementSlotIndex elementSlotIndex ) const;


	//
	// Private data
private:
	WeakPtr<cast::CastingEngine const> mCastingEngine;

	UniquePtr<combat::CombatInstance> mCombatInstance;

	bool mFrameActive = false;

	PartyID mLocalPartyID = {};

	// NOTE: Design choice is that attacks can buffer up, as well as one cast,
	//  but nothing else can buffer up while a cast is pending
	AttackBuffer mAttackBuffer = {};
	UniquePtr<combat::CombatInstance> mCastBuffer = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}

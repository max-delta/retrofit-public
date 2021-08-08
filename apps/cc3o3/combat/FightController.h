#pragma once
#include "project.h"

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
	FightController();

	WeakPtr<CombatInstance const> GetCombatInstance() const;

	void SetupFromCombatInstance( UniquePtr<combat::CombatInstance>&& setup, PartyID localPartyID );

	void StartCombatFrame();
	void EndCombatFrame();
	void TickPendingActions();

	uint8_t SanitizeCharacterIndex( uint8_t attackerIndex, int8_t applyOffset ) const;
	uint8_t SanitizeAttackTargetIndex( uint8_t attackerIndex, uint8_t defenderIndex, int8_t applyOffset ) const;

	PartyID GetLocalPartyID() const;
	FighterID GetCharacterByIndex( uint8_t attackerIndex ) const;
	FighterID GetAttackTargetByIndex( uint8_t attackerIndex, uint8_t defenderIndex ) const;

	bool CanCharacterPerformAttack( uint8_t attackerIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const;
	void PredictAttack( AttackProfile& profile, AttackResult& result, uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const;
	void BufferAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength );

	bool CanCharacterCastElement( uint8_t attackerIndex ) const;

	bool CanCharacterActivateDefense( uint8_t attackerIndex ) const;

	bool CanPartyEndTurn() const;


	//
	// Private methods
private:
	bool SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID ) const;
	bool SimulateAttackBuffer( CombatInstance& simulation, FighterID attackerID, FighterID defenderID, uint8_t additionalAttackStrength ) const;


	//
	// Private data
private:
	UniquePtr<combat::CombatInstance> mCombatInstance;

	bool mFrameActive = false;

	PartyID mLocalPartyID = {};
	AttackBuffer mAttackBuffer = {};
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/company/CompanyFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

// Manages logic for controlling a local party
// NOTE: Most commands are 'local party'-centric
class FightController
{
	RF_NO_COPY( FightController );

	//
	// Public methods
public:
	FightController(
		WeakPtr<CombatEngine const> const& combatEngine,
		WeakPtr<company::CompanyManager const> const& companyManager );

	WeakPtr<CombatInstance const> GetCombatInstance() const;

	void HardcodedPlaceholderSetup();

	uint8_t SanitizeCharacterIndex( uint8_t attackerIndex, int8_t applyOffset ) const;
	uint8_t SanitizeAttackTargetIndex( uint8_t attackerIndex, uint8_t defenderIndex, int8_t applyOffset ) const;

	PartyID GetLocalPartyID() const;
	FighterID GetCharacterByIndex( uint8_t attackerIndex ) const;
	FighterID GetAttackTargetByIndex( uint8_t attackerIndex, uint8_t defenderIndex ) const;

	bool CanCharacterPerformAttack( uint8_t attackerIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex ) const;
	bool CanCharacterPerformAttack( uint8_t attackerIndex, uint8_t defenderIndex, uint8_t attackStrength ) const;

	bool CanCharacterCastElement( uint8_t attackerIndex ) const;

	bool CanCharacterActivateDefense( uint8_t attackerIndex ) const;

	bool CanPartyEndTurn() const;


	//
	// Private data
private:
	WeakPtr<company::CompanyManager const> mCompanyManager;
	UniquePtr<combat::CombatInstance> mCombatInstance;

	PartyID mLocalPartyID = {};
};

///////////////////////////////////////////////////////////////////////////////
}

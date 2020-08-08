#pragma once
#include "project.h"

#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/company/CompanyFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

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
	PartyID GetLocalPartyID() const;

	void HardcodedPlaceholderSetup();

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

#pragma once
#include "project.h"

#include "cc3o3/combat/Field.h"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

struct CastDamageProfile
{
	// Elements may not alway be cast at their natural level
	SimVal mElementStrength = 0;
	element::ElementLevel mCastedLevel = 0;

	// Elements that are multi-target may have damage calculations adjusted
	bool mMultiTarget = false;

	// Deal the damage
	SimVal mAttackerElementalAttack = 0;
	SimVal mDefenderElementalDefense = 0;
	element::InnateIdentifier mElementInnate = {};
	element::InnateIdentifier mAttackerInnate = {};
	element::InnateIdentifier mDefenderInnate = {};
	Field::Influence mInfluence = {};
};



struct CastDamageResult
{
	SimVal mDamage = 0;
	SimVal mCoungerGuageIncrease = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

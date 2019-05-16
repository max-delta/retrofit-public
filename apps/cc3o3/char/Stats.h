#pragma once
#include "project.h"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct Stats
{
	using StatModifier = int8_t;
	static constexpr StatModifier kMinModifier = -10;
	static constexpr StatModifier kDefaultModifier = 0;
	static constexpr StatModifier kMaxModifier = 10;

	using GridShape = uint8_t;
	static constexpr GridShape kMaxBreadth = 0;
	static constexpr GridShape kDefaultShape = 0;
	static constexpr GridShape kMaxDepth = 10;

	StatModifier mMHealth = kDefaultModifier; // More max hit points

	StatModifier mPhysAtk = kDefaultModifier; // Punch harder
	StatModifier mPhysDef = kDefaultModifier; // Take less from punches

	StatModifier mElemAtk = kDefaultModifier; // Cast harder
	StatModifier mElemDef = kDefaultModifier; // Take less from casts

	StatModifier mBalance = kDefaultModifier; // Higher initial shield
	StatModifier mTechniq = kDefaultModifier; // Faster combo growth

	StatModifier mElemPwr = kDefaultModifier; // Better grid
	GridShape mGridDep = kDefaultShape; // Breadth vs. depth
};

///////////////////////////////////////////////////////////////////////////////
}}}

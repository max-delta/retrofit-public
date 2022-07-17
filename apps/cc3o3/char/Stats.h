#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct Stats
{
	static constexpr GridShape kDefaultShape = GridShape::Standard;

	StatValue mMHealth = kDefaultStatValue; // More max hit points

	StatValue mPhysAtk = kDefaultStatValue; // Punch harder
	StatValue mPhysDef = kDefaultStatValue; // Take less from punches

	StatValue mElemAtk = kDefaultStatValue; // Cast harder
	StatValue mElemDef = kDefaultStatValue; // Take less from casts

	StatValue mBalance = kDefaultStatValue; // Higher initial shield
	StatValue mTechniq = kDefaultStatValue; // Faster combo growth

	StatValue mElemPwr = kDefaultStatValue; // Better grid
	union
	{
		GridShape mGridShp = kDefaultShape; // Layout of grid
		uint8_t mGridShpReflect; // TODO: Enum support in reflection
	};
};

///////////////////////////////////////////////////////////////////////////////
}

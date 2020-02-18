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

	enum class GridShape : uint8_t
	{
		Standard = 0,
		Wide,
		Heavy,

		NumShapeTypes
	};
	static constexpr GridShape kDefaultShape = GridShape::Standard;
	static constexpr uint8_t kMaxShapeValue = static_cast<uint8_t>( GridShape::NumShapeTypes ) - 1;

	StatModifier mMHealth = kDefaultModifier; // More max hit points

	StatModifier mPhysAtk = kDefaultModifier; // Punch harder
	StatModifier mPhysDef = kDefaultModifier; // Take less from punches

	StatModifier mElemAtk = kDefaultModifier; // Cast harder
	StatModifier mElemDef = kDefaultModifier; // Take less from casts

	StatModifier mBalance = kDefaultModifier; // Higher initial shield
	StatModifier mTechniq = kDefaultModifier; // Faster combo growth

	StatModifier mElemPwr = kDefaultModifier; // Better grid
	union
	{
		GridShape mGridShp = kDefaultShape; // Layout of grid
		uint8_t mGridShpReflect; // TODO: Enum support in reflection
	};
};

///////////////////////////////////////////////////////////////////////////////
}}}

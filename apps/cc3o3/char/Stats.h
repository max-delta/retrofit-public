#pragma once
#include "project.h"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct Stats
{
	using StatValue = uint8_t;
	static constexpr StatValue kMinStatValue = 0;
	static constexpr StatValue kDefaultStatValue = 0;
	static constexpr StatValue kMaxStatValue = 10;

	enum class GridShape : uint8_t
	{
		Standard = 0,
		Wide,
		Heavy,

		NumShapeTypes
	};
	static constexpr GridShape kDefaultShape = GridShape::Standard;
	static constexpr uint8_t kMaxShapeValue = static_cast<uint8_t>( GridShape::NumShapeTypes ) - 1;

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

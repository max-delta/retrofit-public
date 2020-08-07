#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "core_math/Color4u8.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API MelaninColorGenerator
{
	//
	// Public methods
public:
	// NOTE: Negative ratios cause a additional reduction of that melanin type
	// NOTE: Negative 1 or less cause a complete absence of that melanin type
	math::Color4u8 GenerateColor(
		float baseRatio,
		float pheomelaninRatio,
		float eumelaninRatio ) const;

	sprite::Bitmap GeneratePallete( size_t toneCount ) const;
	sprite::Bitmap GeneratePallete( float baseRatio, float minPheo, float maxPheo, float minEu, float maxEu, size_t toneCount ) const;
	sprite::Bitmap GenerateComplexPallete( size_t toneCountScaler ) const;


	//
	// Private methods
private:
	static math::Color4u8 Mix( math::Color4u8 const& base, math::Color4u8 const& add, float amount );


	//
	// Public data
public:
	float mBaseStartHDeg = 36.f;
	float mBaseStartSVal = 1.f;
	float mBaseStartLVal = .95f;

	float mBaseEndHDeg = 8.f;
	float mBaseEndSVal = .9f;
	float mBaseEndLVal = .84f;

	float mPheomelaninStartHDeg = 37.f;
	float mPheomelaninStartSVal = .57f;
	float mPheomelaninStartLVal = .54f;
	float mPheomelaninStartOpac = .66f;

	float mPheomelaninEndHDeg = 13.f;
	float mPheomelaninEndSVal = .60f;
	float mPheomelaninEndLVal = .41f;
	float mPheomelaninEndOpac = .98f;

	float mEumelaninStartHDeg = 22.f;
	float mEumelaninStartSVal = .60f;
	float mEumelaninStartLVal = .26f;
	float mEumelaninStartOpac = .66f;

	float mEumelaninEndHDeg = 22.f;
	float mEumelaninEndSVal = 1.00f;
	float mEumelaninEndLVal = .00f;
	float mEumelaninEndOpac = .98f;
};

///////////////////////////////////////////////////////////////////////////////
}}

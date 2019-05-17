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
	// TODO: Support for generating base color as another axis of generation
	//  (Will allow support for translucent skin where the blood makes up a
	//  major portion of the overall color)
	math::Color4u8 GenerateColor(
		float pheomelaninRatio,
		float eumelaninRatio ) const;

	sprite::Bitmap GeneratePallete( size_t toneCount ) const;


	//
	// Private methods
private:
	static math::Color4u8 Mix( math::Color4u8 const& base, math::Color4u8 const& add, float amount );

	//
	// Public data
public:
	math::Color4u8 mBaseColor = math::Color4u8( 246, 213, 164 );

	float mPheomelaninStartHDeg = 30.f;
	float mPheomelaninStartSVal = .60f;
	float mPheomelaninStartLVal = .51f;
	float mPheomelaninStartOpac = .66f;

	float mPheomelaninEndHDeg = 0.f;
	float mPheomelaninEndSVal = .91f;
	float mPheomelaninEndLVal = .24f;
	float mPheomelaninEndOpac = .98f;

	float mEumelaninStartHDeg = 35.f;
	float mEumelaninStartSVal = .43f;
	float mEumelaninStartLVal = .37f;
	float mEumelaninStartOpac = .66f;

	float mEumelaninEndHDeg = 30.f;
	float mEumelaninEndSVal = 1.00f;
	float mEumelaninEndLVal = .00f;
	float mEumelaninEndOpac = .98f;
};

///////////////////////////////////////////////////////////////////////////////
}}

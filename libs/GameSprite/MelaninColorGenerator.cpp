#include "stdafx.h"
#include "MelaninColorGenerator.h"

#include "GameSprite/Bitmap.h"

#include "core_math/Color3f.h"
#include "core_math/Lerp.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "rftl/array"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

math::Color4u8 MelaninColorGenerator::GenerateColor( float baseRatio, float pheomelaninRatio, float eumelaninRatio ) const
{
	bool const genPheo = pheomelaninRatio > -1.f;
	bool const genEu = eumelaninRatio > -1.f;

	// Base
	math::Color4u8 base = math::Color4u8( 0, 0, 0, 0 );
	{
		float const genRatio = math::Clamp( 0.f, baseRatio, 1.f );
		float const baseH = math::Lerp( mBaseStartHDeg, mBaseEndHDeg, genRatio );
		float const baseS = math::Lerp( mBaseStartSVal, mBaseEndSVal, genRatio );
		float const baseL = math::Lerp( mBaseStartLVal, mBaseEndLVal, genRatio );
		math::Color3f const base3 = math::Color3f::FromHSL( baseH, baseS, baseL );
		base = math::Color4u8(
			math::integer_cast<math::Color4u8::ElementType>( base3.r * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( base3.g * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( base3.b * 255 ) );
	}

	// Pheomelanin
	math::Color4u8 pheo = math::Color4u8( 0, 0, 0, 0 );
	if( genPheo )
	{
		float const genRatio = math::Clamp( 0.f, pheomelaninRatio, 1.f );
		float const pheoH = math::Lerp( mPheomelaninStartHDeg, mPheomelaninEndHDeg, genRatio );
		float const pheoS = math::Lerp( mPheomelaninStartSVal, mPheomelaninEndSVal, genRatio );
		float const pheoL = math::Lerp( mPheomelaninStartLVal, mPheomelaninEndLVal, genRatio );
		float pheoO;
		if( pheomelaninRatio >= 0.f )
		{
			pheoO = math::Lerp( mPheomelaninStartOpac, mPheomelaninEndOpac, pheomelaninRatio );
		}
		else
		{
			float const positiveRatio = 1.f + pheomelaninRatio;
			pheoO = math::Lerp( 0.f, mPheomelaninStartOpac, positiveRatio );
		}
		math::Color3f const pheo3 = math::Color3f::FromHSL( pheoH, pheoS, pheoL );
		pheo = math::Color4u8(
			math::integer_cast<math::Color4u8::ElementType>( pheo3.r * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( pheo3.g * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( pheo3.b * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( pheoO * 255 ) );
	}

	// Eumelanin
	math::Color4u8 eu = math::Color4u8( 0, 0, 0, 0 );
	if( genEu )
	{
		float const genRatio = math::Clamp( 0.f, eumelaninRatio, 1.f );
		float const euH = math::Lerp( mEumelaninStartHDeg, mEumelaninEndHDeg, genRatio );
		float const euS = math::Lerp( mEumelaninStartSVal, mEumelaninEndSVal, genRatio );
		float const euL = math::Lerp( mEumelaninStartLVal, mEumelaninEndLVal, genRatio );
		float euO;
		if( eumelaninRatio >= 0.f )
		{
			euO = math::Lerp( mEumelaninStartOpac, mEumelaninEndOpac, eumelaninRatio );
		}
		else
		{
			float const positiveRatio = 1.f + eumelaninRatio;
			euO = math::Lerp( 0.f, mEumelaninStartOpac, positiveRatio );
		}
		math::Color3f const eu3 = math::Color3f::FromHSL( euH, euS, euL );
		eu = math::Color4u8(
			math::integer_cast<math::Color4u8::ElementType>( eu3.r * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( eu3.g * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( eu3.b * 255 ),
			math::integer_cast<math::Color4u8::ElementType>( euO * 255 ) );
	}

	// Mix
	float const pPlusE = ( math::float_cast<float>( pheo.a ) / 255.f ) + ( math::float_cast<float>( eu.a ) / 255.f );
	float const pToERatio = 1.f - ( ( math::float_cast<float>( pheo.a ) / 255.f ) / pPlusE );
	math::Color4u8 const melanin = Mix( pheo, eu, pToERatio );
	float const melaninRatio = math::Clamp( 0.f, pPlusE, 1.f );
	math::Color4u8 const retVal = Mix( base, melanin, melaninRatio );
	return retVal;
}



sprite::Bitmap MelaninColorGenerator::GeneratePallete( size_t toneCount ) const
{
	return GeneratePallete( .5f, -1.f, 1.f, -1.f, 1.f, toneCount );
}



sprite::Bitmap MelaninColorGenerator::GeneratePallete( float baseRatio, float minPheo, float maxPheo, float minEu, float maxEu, size_t toneCount ) const
{
	RF_ASSERT( toneCount > 0 );

	sprite::Bitmap retVal = sprite::Bitmap( toneCount, toneCount );
	size_t const melaninCount = toneCount - 1;
	for( size_t y = 0; y < toneCount; y++ )
	{
		float eu = minEu;
		if( y > 0 )
		{
			float const ratio = math::float_cast<float>( y ) / math::float_cast<float>( melaninCount );
			eu = math::Lerp( minEu, maxEu, ratio );
		}

		for( size_t x = 0; x < toneCount; x++ )
		{
			float pheo = minPheo;
			if( x > 0 )
			{
				float const ratio = math::float_cast<float>( x ) / math::float_cast<float>( melaninCount );
				pheo = math::Lerp( minPheo, maxPheo, ratio );
			}

			retVal.GetMutablePixel( x, y ) = GenerateColor( baseRatio, pheo, eu );
		}
	}

	return retVal;
}



sprite::Bitmap MelaninColorGenerator::GenerateComplexPallete( size_t toneCountScaler ) const
{
	RF_ASSERT( toneCountScaler > 2 );

	sprite::Bitmap retVal = sprite::Bitmap( 4 * toneCountScaler, 3 * toneCountScaler );

	sprite::Bitmap const mainPallete = GeneratePallete( toneCountScaler * 3 );
	retVal.ApplyStencilOverwrite( mainPallete, 0, 0, 1 );
	size_t const subPalleteStartX = mainPallete.GetWidth();

	// Any melanin will quickly overcome the base color, so focus on the
	//  differences in bases while supressing melanin
	static constexpr float kMinAlbMel = -1.f;
	static constexpr float kMaxAlbMel = -.75f;
	rftl::array<sprite::Bitmap, 4> const albinoAlternatePalletes = {
		GeneratePallete( 0.0f, kMinAlbMel, kMaxAlbMel, kMinAlbMel, kMaxAlbMel, toneCountScaler / 2 ),
		GeneratePallete( .25f, kMinAlbMel, kMaxAlbMel, kMinAlbMel, kMaxAlbMel, toneCountScaler / 2 ),
		GeneratePallete( .75f, kMinAlbMel, kMaxAlbMel, kMinAlbMel, kMaxAlbMel, toneCountScaler / 2 ),
		GeneratePallete( 1.0f, kMinAlbMel, kMaxAlbMel, kMinAlbMel, kMaxAlbMel, toneCountScaler / 2 )
	};
	retVal.ApplyStencilOverwrite( albinoAlternatePalletes.at( 0 ), subPalleteStartX, 0, 1 );
	retVal.ApplyStencilOverwrite( albinoAlternatePalletes.at( 1 ), subPalleteStartX + toneCountScaler / 2, 0, 1 );
	retVal.ApplyStencilOverwrite( albinoAlternatePalletes.at( 2 ), subPalleteStartX, toneCountScaler / 2, 1 );
	retVal.ApplyStencilOverwrite( albinoAlternatePalletes.at( 3 ), subPalleteStartX + toneCountScaler / 2, toneCountScaler / 2, 1 );

	// Melanin extremes get mixed out quickly, so focus closer on the edges
	static constexpr float kMinFocusMel = .5f;
	static constexpr float kMaxFocusMel = 1.f;
	static constexpr float kMinSuppressMel = -1.f;
	static constexpr float kMaxSuppressMel = -.5f;

	sprite::Bitmap const pheomelaninHeavyPallete =
		GeneratePallete( .5f, kMinFocusMel, kMaxFocusMel, kMinSuppressMel, kMaxSuppressMel, toneCountScaler );
	retVal.ApplyStencilOverwrite( pheomelaninHeavyPallete, subPalleteStartX, toneCountScaler, 1 );

	sprite::Bitmap const eumelaninHeavyPallete =
		GeneratePallete( .5f, kMinSuppressMel, kMaxSuppressMel, kMinFocusMel, kMaxFocusMel, toneCountScaler );
	retVal.ApplyStencilOverwrite( eumelaninHeavyPallete, subPalleteStartX, toneCountScaler * 2, 1 );

	return retVal;
}



math::Color4u8 MelaninColorGenerator::Mix( math::Color4u8 const& base, math::Color4u8 const& add, float amount )
{
	math::Color4u8 retval;
	retval.r = math::integer_cast<math::Color4u8::ElementType>( math::Lerp<int16_t>( base.r, add.r, amount ) );
	retval.g = math::integer_cast<math::Color4u8::ElementType>( math::Lerp<int16_t>( base.g, add.g, amount ) );
	retval.b = math::integer_cast<math::Color4u8::ElementType>( math::Lerp<int16_t>( base.b, add.b, amount ) );
	retval.a = base.a;
	return retval;
}

///////////////////////////////////////////////////////////////////////////////
}

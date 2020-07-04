#include "stdafx.h"
#include "MelaninColorGenerator.h"

#include "GameSprite/Bitmap.h"

#include "core_math/Color3f.h"
#include "core_math/Lerp.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

math::Color4u8 MelaninColorGenerator::GenerateColor( float pheomelaninRatio, float eumelaninRatio ) const
{
	bool const genPheo = pheomelaninRatio > -1.f;
	bool const genEu = eumelaninRatio > -1.f;

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
	float const pPlusE = ( pheo.a / 255.f ) + ( eu.a / 255.f );
	float const pToERatio = 1.f - ( ( pheo.a / 255.f ) / pPlusE );
	math::Color4u8 const melanin = Mix( pheo, eu, pToERatio );
	float const melaninRatio = math::Clamp( 0.f, pPlusE, 1.f );
	math::Color4u8 const retVal = Mix( mBaseColor, melanin, melaninRatio );
	return retVal;
}



sprite::Bitmap MelaninColorGenerator::GeneratePallete( size_t toneCount ) const
{
	RF_ASSERT( toneCount > 0 );

	sprite::Bitmap retVal = sprite::Bitmap( toneCount, toneCount );
	size_t const melaninCount = toneCount - 1;
	for( size_t y = 0; y < toneCount; y++ )
	{
		float eu = -1.f;
		if( y > 0 )
		{
			float const ratio = math::real_cast<float>( y ) / math::real_cast<float>( melaninCount );
			eu = math::Lerp( -1.f, 1.f, ratio );
		}

		for( size_t x = 0; x < toneCount; x++ )
		{
			float pheo = -1.f;
			if( x > 0 )
			{
				float const ratio = math::real_cast<float>( x ) / math::real_cast<float>( melaninCount );
				pheo = math::Lerp( -1.f, 1.f, ratio );
			}

			retVal.GetMutablePixel( x, y ) = GenerateColor( pheo, eu );
		}
	}

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
}}

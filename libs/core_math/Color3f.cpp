#include "stdafx.h"
#include "Color3f.h"

#include "core_math/Color3u8.h"
#include "core_math/Color4u8.h"
#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"
#include "core_math/Lerp.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

Color3f const Color3f::kBlack{ Color3u8::kBlack };
Color3f const Color3f::kGray25{ Color3u8::kGray25 };
Color3f const Color3f::kGray50{ Color3u8::kGray50 };
Color3f const Color3f::kGray75{ Color3u8::kGray75 };
Color3f const Color3f::kWhite{ Color3u8::kWhite };

Color3f const Color3f::kRed{ Color3u8::kRed };
Color3f const Color3f::kGreen{ Color3u8::kGreen };
Color3f const Color3f::kBlue{ Color3u8::kBlue };

Color3f const Color3f::kCyan{ Color3u8::kCyan };
Color3f const Color3f::kMagenta{ Color3u8::kMagenta };
Color3f const Color3f::kYellow{ Color3u8::kYellow };

Color3f const Color3f::kTeal{ Color3u8::kTeal };
Color3f const Color3f::kPurple{ Color3u8::kPurple };
Color3f const Color3f::kOrange{ Color3u8::kOrange };

///////////////////////////////////////////////////////////////////////////////

Color3f::Color3f() = default;



Color3f::Color3f( ElementType r, ElementType g, ElementType b )
	: r( r )
	, g( g )
	, b( b )
{
	//
}



Color3f::Color3f( Color3u8 rgb )
	: Color3f(
		  float_cast<ElementType>( rgb.r ) / float_cast<float>( rftl::numeric_limits<Color3u8::ElementType>::max() ),
		  float_cast<ElementType>( rgb.g ) / float_cast<float>( rftl::numeric_limits<Color3u8::ElementType>::max() ),
		  float_cast<ElementType>( rgb.b ) / float_cast<float>( rftl::numeric_limits<Color3u8::ElementType>::max() ) )
{
	//
}



Color3f::Color3f( Color4u8 rgba )
	: Color3f( Color3u8( rgba ) )
{
	//
}



Color3f Color3f::RandomFromHash( uint64_t hashVal )
{
	return Color3f( Color3u8::RandomFromHash( hashVal ) );
}



Color3f Color3f::FromHSL( float hueDegrees, float saturationRatio, ElementType luminanceValue )
{
	if( Equals( saturationRatio, 0.f ) )
	{
		// Gray, just set luminance for everything
		return Color3f( luminanceValue, luminanceValue, luminanceValue );
	}

	// HERE BE DRAGONS!

	float nonObviousTemp1;
	if( luminanceValue < .5f )
	{
		nonObviousTemp1 = luminanceValue * ( 1.f + saturationRatio );
	}
	else
	{
		nonObviousTemp1 = luminanceValue + saturationRatio - ( luminanceValue * saturationRatio );
	}
	float const confusingTemp2 = 2.f * luminanceValue - nonObviousTemp1;

	RF_ASSERT( hueDegrees >= 0.f );
	RF_ASSERT( hueDegrees <= 360.f );
	float const hueRatio = hueDegrees / 360.f;
	RF_ASSERT( hueRatio >= 0.f );
	RF_ASSERT( hueRatio <= 1.f );

	// If you're debugging this, I want you to know that I weep in solidarity

	float tempR = hueRatio + 1.f / 3.f;
	if( tempR > 1.f )
	{
		tempR -= 1.f;
	}
	float tempG = hueRatio;
	float tempB = hueRatio - 1.f / 3.f;
	if( tempB < 0.f )
	{
		tempB += 1.f;
	}
	RF_ASSERT( tempR >= 0.f );
	RF_ASSERT( tempG >= 0.f );
	RF_ASSERT( tempB >= 0.f );
	RF_ASSERT( tempR <= 1.f );
	RF_ASSERT( tempG <= 1.f );
	RF_ASSERT( tempB <= 1.f );

	// You've met with a terrible fate, haven't you?

	float components[3] = { tempR, tempG, tempB };
	for( float& component : components )
	{
		if( 6.f * component < 1.f )
		{
			const float newComponent = confusingTemp2 + ( nonObviousTemp1 - confusingTemp2 ) * 6.f * component;
			RF_ASSERT( newComponent >= 0.f );
			component = newComponent;
		}
		else if( 2.f * component < 1.f )
		{
			RF_ASSERT( nonObviousTemp1 >= 0.f );
			RF_ASSERT( nonObviousTemp1 <= 1.f );
			component = nonObviousTemp1;
		}
		else if( 3.f * component < 2.f )
		{
			const float newComponent = confusingTemp2 + ( nonObviousTemp1 - confusingTemp2 ) * ( ( 2.f / 3.f ) - component );
			RF_ASSERT( newComponent >= 0.f );
			component = newComponent;
		}
		else
		{
			RF_ASSERT( confusingTemp2 >= 0.f );
			RF_ASSERT( confusingTemp2 <= 1.f );
			component = confusingTemp2;
		}
		RF_ASSERT( component >= 0.f );
		RF_ASSERT( component <= 1.f );
	}

	// No, I don't understand any of that either, but there's some math proofs
	//  showing it to be the inverse of the rgb->hsl logic
	return Color3f( components[0], components[1], components[2] );
}



Color3f& Color3f::ClampValues()
{
	ClampValues( 0.f, 1.f );
	return *this;
}



Color3f& Color3f::ClampValues( ElementType min, ElementType max )
{
	r = math::Clamp( min, r, max );
	g = math::Clamp( min, g, max );
	b = math::Clamp( min, b, max );
	return *this;
}



Color3f& Color3f::ClampValues( Color3f const& min, Color3f const& max )
{
	r = math::Clamp( min.r, r, max.r );
	g = math::Clamp( min.g, g, max.g );
	b = math::Clamp( min.b, b, max.b );
	return *this;
}



Color3f& Color3f::ClampHue( float minDegrees, float maxDegrees )
{
	ClampValues();

	float h, s, l;
	GetHSL( h, s, l );

	RF_ASSERT( minDegrees >= 0.f );
	RF_ASSERT( maxDegrees <= 360.f );
	RF_ASSERT( minDegrees < maxDegrees );
	float const newH = math::Clamp( minDegrees, h, maxDegrees );
	if( Equals( h, newH ) == false )
	{
		if( Equals( s, 0.f ) )
		{
			// Gray, will have no effect
			// THINK: "I'd like this gray to be different, but still the exact
			//  same brightness, and still completely devoid of color"
		}
		else
		{
			*this = FromHSL( newH, s, l );
		}
	}
	return *this;
}



Color3f& Color3f::ClampSaturation( float minRatio, float maxRatio )
{
	ClampValues();

	float h, s, l;
	GetHSL( h, s, l );

	RF_ASSERT( minRatio >= 0.f );
	RF_ASSERT( maxRatio <= 1.f );
	RF_ASSERT( minRatio < maxRatio );
	float const newS = math::Clamp( minRatio, s, maxRatio );
	if( Equals( s, newS ) == false )
	{
		// WARNING: If it was gray, then hue is undefined, so you will get some
		//  arbitrary color as result of increasing saturation here
		// THINK: "What would gray look like if it were more colorful?"
		*this = FromHSL( h, newS, l );
	}
	return *this;
}



Color3f& Color3f::ClampLuminance( ElementType min, ElementType max )
{
	ClampValues();

	float h, s, l;
	GetHSL( h, s, l );

	RF_ASSERT( min >= 0.f );
	RF_ASSERT( max <= 1.f );
	RF_ASSERT( min < max );
	float const newL = math::Clamp( min, l, max );
	if( Equals( l, newL ) == false )
	{
		*this = FromHSL( h, s, newL );
	}
	return *this;
}



void Color3f::GetHSL( float& hueDegrees, float& saturationRatio, ElementType& luminanceValue ) const
{
	// PRO TIP: The MS color picker is a trap, it uses RGB 0-255 and HSL 0-239,
	//  just to make it extra confusing for you by throwing more conversions
	//  in. Use a different color picker, like from Paint.NET or something if
	//  you're trying to debug this.

	// Hue is defined as the angle on the surface of a hexagon created by the
	//  projection of the RGB cube onto a plane... Yeah, I know right?
	ElementType min;
	ElementType max;
	if( r >= g && r >= b )
	{
		// R is greatest
		max = r;
		if( g >= b )
		{
			// R, G, B
			min = b;
		}
		else
		{
			// R, B, G
			min = g;
		}
		hueDegrees = ( ( g - b ) / ( max - min ) ) * 60.f;
	}
	else if( g >= r && g >= b )
	{
		// G is greatest
		max = g;
		if( r >= b )
		{
			// G, R, B
			min = b;
		}
		else
		{
			// G, B, R
			min = r;
		}
		hueDegrees = ( 2.f + ( ( b - r ) / ( max - min ) ) ) * 60.f;
	}
	else
	{
		// B is greatest
		max = b;
		if( r >= g )
		{
			// B, R, G
			min = g;
		}
		else
		{
			// B, G, R
			min = r;
		}
		hueDegrees = ( 4.f + ( ( r - g ) / ( max - min ) ) ) * 60.f;
	}
	RF_ASSERT( min <= max );
	bool const isGray = Equals( min, max );
	if( isGray )
	{
		// Is some kind of gray, and thus hue is undefined, probably divided by
		//  zero during the calculations
		hueDegrees = 0.f;
	}
	RF_ASSERT( hueDegrees >= -360.f );
	RF_ASSERT( hueDegrees <= 360.f );
	if( UncheckedEquals( hueDegrees, -0.f ) )
	{
		hueDegrees = 0.f;
	}
	else if( hueDegrees < 0.f )
	{
		hueDegrees += 360.f;
	}
	RF_ASSERT( hueDegrees >= 0.f );
	RF_ASSERT( hueDegrees <= 360.f );

	// TODO: IEC 61966-2-1:1999
	// luminanceValue = 0.2126f * r + 0.7152f * g + 0.0722 * b;
	luminanceValue = .5f * ( min + max );

	// There's some odd futzing of saturation to make sure it doesn't fall out
	//  of the color-space. I don't know either, I'm not a color technician :/
	if( isGray )
	{
		// Is some kind of gray, and thus saturation is zero
		saturationRatio = 0.f;
	}
	else if( luminanceValue < .5f )
	{
		saturationRatio = ( max - min ) / ( max + min );
	}
	else
	{
		saturationRatio = ( max - min ) / ( 2.f - max - min );
	}
}



bool Color3f::operator==( Color3f const& rhs ) const
{
	return Equals( r, rhs.r ) && Equals( g, rhs.g ) && Equals( b, rhs.b );
}



Color3f Color3f::operator+( Color3f const& rhs ) const
{
	return Color3f( r + rhs.r, g + rhs.g, b + rhs.b );
}



Color3f Color3f::operator-( Color3f const& rhs ) const
{
	return Color3f( r - rhs.r, g - rhs.g, b - rhs.b );
}



Color3f& Color3f::operator+=( Color3f const& rhs )
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}



Color3f& Color3f::operator-=( Color3f const& rhs )
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}



Color3f Color3f::operator+( ElementType const& rhs ) const
{
	return Color3f( r + rhs, g + rhs, b + rhs );
}



Color3f Color3f::operator-( ElementType const& rhs ) const
{
	return Color3f( r - rhs, g - rhs, b - rhs );
}



Color3f Color3f::operator*( ElementType const& rhs ) const
{
	return Color3f( r * rhs, g * rhs, b * rhs );
}



Color3f& Color3f::operator+=( ElementType const& rhs )
{
	r += rhs;
	g += rhs;
	b += rhs;
	return *this;
}



Color3f& Color3f::operator-=( ElementType const& rhs )
{
	r -= rhs;
	g -= rhs;
	b -= rhs;
	return *this;
}



Color3f& Color3f::operator*=( ElementType const& rhs )
{
	r *= rhs;
	g *= rhs;
	b *= rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"

#include "core_math/SoftFloat.h"

#include "rftl/limits"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( SoftFloat, IEEE754Binary32 )
{
	static_assert( rftl::numeric_limits<float>::is_iec559, "This test assumes standard floats" );
	using Hard = float;
	using Soft = SoftFloat<Hard>;
	{
		Hard const hard = 0.f;
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 0 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), false );
		ASSERT_EQ( Soft::ExponentValue( hard ), 0 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = -0.f;
		ASSERT_EQ( Soft::SignBit( hard ), 1 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 0 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), true );
		ASSERT_EQ( Soft::ExponentValue( hard ), 0 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = 0.5f;
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 126 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), false );
		ASSERT_EQ( Soft::ExponentValue( hard ), -1 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = -2.f;
		ASSERT_EQ( Soft::SignBit( hard ), 1 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 128 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), true );
		ASSERT_EQ( Soft::ExponentValue( hard ), 1 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = -3.f;
		ASSERT_EQ( Soft::SignBit( hard ), 1 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 128 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b100'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), true );
		ASSERT_EQ( Soft::ExponentValue( hard ), 1 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = rftl::numeric_limits<Hard>::epsilon();
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 104 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), false );
		ASSERT_EQ( Soft::ExponentValue( hard ), -23 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = rftl::numeric_limits<Hard>::denorm_min();
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 0 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0001 );
		ASSERT_EQ( Soft::IsNegative( hard ), false );
		ASSERT_EQ( Soft::ExponentValue( hard ), -127 );
		ASSERT_EQ( Soft::IsDenormalized( hard ), true );
		ASSERT_EQ( Soft::IsInfinity( hard ), false );
	}
	{
		Hard const hard = rftl::numeric_limits<Hard>::infinity();
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 255 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b000'0000'0000'0000'0000'0000 );
		ASSERT_EQ( Soft::IsNegative( hard ), false );
		ASSERT_EQ( Soft::IsInfinity( hard ), true );
	}
	{
		Hard const hard = rftl::numeric_limits<Hard>::quiet_NaN();
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 255 );
		ASSERT_EQ( Soft::FractionalBits( hard ), 0b100'0000'0000'0000'0000'0000 );
	}
	{
		Hard const hard = rftl::numeric_limits<Hard>::signaling_NaN();
		ASSERT_EQ( Soft::SignBit( hard ), 0 );
		ASSERT_EQ( Soft::ExponentBits( hard ), 255 );
		switch( compiler::kCompiler )
		{
			case compiler::Compiler::MSVC:
				ASSERT_EQ( Soft::FractionalBits( hard ), 0b100'0000'0000'0000'0000'0001 );
				break;
			case compiler::Compiler::Clang:
			{
				Soft::Rep const frac = Soft::FractionalBits( hard );
				if constexpr( compiler::kArchitecture == compiler::Architecture::x86_32 )
				{
					// Clang seems to have some bizarre behaviors here that
					//  results in inconsistent literals in the assembly
					ASSERT_TRUE(
						frac == 0b000'0000'0000'0000'0000'0001 ||
						frac == 0b100'0000'0000'0000'0000'0001 );
				}
				else
				{
					ASSERT_EQ( frac, 0b000'0000'0000'0000'0000'0001 );
				}
				break;
			}
			case compiler::Compiler::Invalid:
			default:
				// Unimplemented
				ASSERT_TRUE( false );
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}

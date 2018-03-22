#include "stdafx.h"

#include "core_math/NumericTraits.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathTraits, Basics )
{
	static_assert( NumericTraits<int>::Empty() == 0, "Bad trait" );
	static_assert( NumericTraits<int>::Identity() == 1, "Bad trait" );
	static_assert( NumericTraits<float>::Empty() == 0.f, "Bad trait" );
	static_assert( NumericTraits<float>::Identity() == 1.f, "Bad trait" );
}

///////////////////////////////////////////////////////////////////////////////
}}

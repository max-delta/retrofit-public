#pragma once
#include "Rollback/Var.h"

#include "core_math/math_casts.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

#define RF_OP( OP, SYM ) \
	template<typename LHS, typename RHS> \
	inline Var<LHS> OP( Var<LHS> lhs, RHS const& rhs ) \
	{ \
		lhs = math::integer_cast<LHS>( lhs SYM rhs ); \
		return lhs; \
	}

// clang-format off
RF_OP( operator+=, + );
RF_OP( operator-=, - );
RF_OP( operator*=, * );
RF_OP( operator/=, / );
RF_OP( operator%=, % );
RF_OP( operator&=, & );
RF_OP( operator|=, | );
RF_OP( operator^=, ^ );
RF_OP( operator<<=, << );
RF_OP( operator>>=, >> );
// clang-format on

#undef RF_OP

///////////////////////////////////////////////////////////////////////////////
}

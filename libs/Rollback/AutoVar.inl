#pragma once
#include "Rollback/AutoVar.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

#define RF_OP( OP ) \
	template<typename LHS, typename RHS> \
	inline Var<LHS> OP( AutoVar<LHS>& lhs, RHS const& rhs ) \
	{ \
		return OP( static_cast<Var<LHS>>( lhs ), rhs ); \
	}

// clang-format off
RF_OP( operator+= );
RF_OP( operator-= );
RF_OP( operator*= );
RF_OP( operator/= );
RF_OP( operator%= );
RF_OP( operator&= );
RF_OP( operator|= );
RF_OP( operator^= );
RF_OP( operator<<= );
RF_OP( operator>>= );
// clang-format on

#undef RF_OP

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "Rollback/AutoVar.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
AutoVar<T>& AutoVar<T>::operator=( Type const& value )
{
	this->mVar.operator=( value );
	return *this;
}



template<typename T>
AutoVar<T>::operator Var<typename AutoVar<T>::VarType>() const
{
	return this->mVar;
}



template<typename T>
AutoVar<T>::operator Type() const
{
	// NOTE: May be different type
	return static_cast<Type>( this->mVar.operator VarType() );
}



template<typename T>
void AutoVar<T>::Write( time::CommonClock::time_point time, Type const& value )
{
	this->mVar.Write( time, value );
}



template<typename T>
typename AutoVar<T>::Type AutoVar<T>::Read( time::CommonClock::time_point time ) const
{
	return this->mVar.Read( time );
}



template<typename T>
Var<typename AutoVar<T>::VarType> AutoVar<T>::VarT() const
{
	return this->mVar;
}



template<typename T>
typename AutoVar<T>::Type AutoVar<T>::As() const
{
	// NOTE: May be different type
	return static_cast<Type>( this->mVar.As() );
}

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

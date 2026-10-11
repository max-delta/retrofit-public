#pragma once
#include "core/todo.h"

#include "rftl/format"
#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Clang compiler weakness requires the formatter specialization to be in the
//  global namespace instead of the enclosed namespace

///////////////////////////////////////////////////////////////////////////////
}

// Formats as an integer
template<typename EnumT, typename CtxCharT>
	requires rftl::is_enum<EnumT>::value
struct rftl::formatter<EnumT, CtxCharT> : rftl::formatter<typename underlying_type<EnumT>::type, CtxCharT>
{
	using Input = EnumT;
	using Shim = underlying_type<EnumT>::type;
	using Base = formatter<Shim, CtxCharT>;

	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		return Base::parse( ctx );
	}

	template<class FmtContext>
	typename FmtContext::iterator format( Input const& arg, FmtContext& ctx ) const
	{
		RF_CPP23_TODO( "Use to_underlying instead" );
		Shim const underlying = static_cast<Shim>( arg );
		return Base::format( underlying, ctx );
	}
};

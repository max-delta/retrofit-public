#pragma once

#include "core/meta/TypeList.h"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

enum class CallType : uint8_t
{
	Invalid = 0,
	FreeStanding,
	Member
};



// Un-defined, requires a specialization to match
template<typename Function>
struct FunctionTraits;



// Free-standing function (lvalue?)
template<typename RetType, typename... Params>
struct FunctionTraits<RetType( Params... )>
{
	static constexpr CallType kCallType = CallType::FreeStanding;
	using ReturnType = RetType;
	using ParamTypes = TypeList<Params...>;
	using FunctionType = RetType( Params... );
	using FunctionPointerType = RetType ( * )( Params... );
};



// Free-standing function pointer (rvalue?)
template<typename RetType, typename... Params>
struct FunctionTraits<RetType ( * )( Params... )>
{
	static constexpr CallType kCallType = CallType::FreeStanding;
	using ReturnType = RetType;
	using ParamTypes = TypeList<Params...>;
	using FunctionType = RetType( Params... );
	using FunctionPointerType = RetType ( * )( Params... );
};



// Member function, non-const
template<typename RetType, typename This, typename... Params>
struct FunctionTraits<RetType ( This::* )( Params... )>
{
	static constexpr CallType kCallType = CallType::Member;
	static constexpr bool kRequiresConst = false;
	using ThisType = This;
	using ReturnType = RetType;
	using ParamTypes = TypeList<Params...>;
	// NOTE: No non-pointer function type attribute, not permitted by standard
	using FunctionPointerType = RetType ( This::* )( Params... );
};



// Member function, const
template<typename RetType, typename This, typename... Params>
struct FunctionTraits<RetType ( This::* )( Params... ) const>
{
	static constexpr CallType kCallType = CallType::Member;
	static constexpr bool kRequiresConst = true;
	using ThisType = This const;
	using ReturnType = RetType;
	using ParamTypes = TypeList<Params...>;
	// NOTE: No non-pointer function type attribute, not permitted by standard
	using FunctionPointerType = RetType ( This::* )( Params... ) const;
};

///////////////////////////////////////////////////////////////////////////////
}

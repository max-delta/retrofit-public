#pragma once

#include "core/meta/TypeList.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

enum class CallType : uint8_t
{
	Invalid = 0,
	FreeStanding,
	Member
};



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
	using FunctionPointerType = RetType( *)( Params... );
};



// Free-standing function pointer (rvalue?)
template<typename RetType, typename... Params>
struct FunctionTraits<RetType( *)( Params... )>
{
	static constexpr CallType kCallType = CallType::FreeStanding;
	using ReturnType = RetType;
	using ParamTypes = TypeList<Params...>;
	using FunctionType = RetType( Params... );
	using FunctionPointerType = RetType( *)( Params... );
};

///////////////////////////////////////////////////////////////////////////////
}}

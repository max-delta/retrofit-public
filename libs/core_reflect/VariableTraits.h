#pragma once
#include "rftl/type_traits"

namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

enum class VariableType : uint8_t
{
	Invalid = 0,
	FreeStanding,
	Member
};



// Un-defined, requires a specialization to match
template<typename Variable>
struct VariableTraits;



// Free-standing variable
template<typename Variable>
struct VariableTraits<Variable*>
{
	static constexpr VariableType kVariableType = VariableType::FreeStanding;
	static constexpr bool kIsConst = rftl::is_const<Variable>::value;
	using VariableType = Variable;
};



// Member variable
template<typename Class, typename Member>
struct VariableTraits<Member Class::*>
{
	static constexpr VariableType kVariableType = VariableType::Member;
	static constexpr bool kIsConst = rftl::is_const<Member>::value;
	using ClassType = Class;
	using VariableType = Member;
};

///////////////////////////////////////////////////////////////////////////////
}

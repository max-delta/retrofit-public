#pragma once
#include "core_rftype/ClassInfoAccessor.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// Un-defined, requires a specialization to match
template<typename Type, typename Unused = void>
struct TypeInference;

// Matches value types
template<typename Type>
struct TypeInference<Type, typename rftl::enable_if<reflect::Value::DetermineType<Type>() != reflect::Value::Type::Invalid>::type>
{
	static reflect::VariableTypeInfo GetTypeInfo()
	{
		reflect::VariableTypeInfo retVal = {};
		retVal.mValueType = reflect::Value::DetermineType<Type>();
		return retVal;
	}
};

// Matches non-value types
// TODO: Support accessors. Will likely have to be able to identify accessors
//  immediately after values during template consideration, and only try
//  GetClassInfo() as a last resort, since GetClassInfo()will always compile
//  and won't fail until link-time
template<typename Type>
struct TypeInference<Type, typename rftl::enable_if<reflect::Value::DetermineType<Type>() == reflect::Value::Type::Invalid>::type>
{
	static reflect::VariableTypeInfo GetTypeInfo()
	{
		reflect::VariableTypeInfo retVal = {};
		retVal.mClassInfo = &rftype::GetClassInfo<Type>();
		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}

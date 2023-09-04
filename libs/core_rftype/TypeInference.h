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
	static reflect::VariableTypeInfo GetValueTypeInfo()
	{
		reflect::VariableTypeInfo retVal = {};
		retVal.mValueType = reflect::Value::DetermineType<Type>();
		return retVal;
	}

	static reflect::VariableTypeInfo GetTypeInfo()
	{
		return GetValueTypeInfo();
	}
};

// Matches non-value types that have accessors
template<typename Type>
struct TypeInference<Type, typename rftl::enable_if<reflect::Value::DetermineType<Type>() == reflect::Value::Type::Invalid && extensions::Accessor<Type>::kExists>::type>
{
	static reflect::VariableTypeInfo GetAccessorTypeInfo()
	{
		reflect::VariableTypeInfo retVal = {};
		static reflect::ExtensionAccessor const sSharedModuleCache = extensions::Accessor<Type>::Get();
		retVal.mAccessor = &sSharedModuleCache;
		return retVal;
	}

	static reflect::VariableTypeInfo GetTypeInfo()
	{
		return GetAccessorTypeInfo();
	}
};

// Matches non-value types without accessors
// NOTE: Expects ClassInfo to be present, which will always compile, but may fail
//  to be found during link-time if it isn't present
template<typename Type>
struct TypeInference<Type, typename rftl::enable_if<reflect::Value::DetermineType<Type>() == reflect::Value::Type::Invalid && extensions::Accessor<Type>::kExists == false>::type>
{
	static reflect::VariableTypeInfo GetClassTypeInfo()
	{
		reflect::VariableTypeInfo retVal = {};
		retVal.mClassInfo = &rftype::GetClassInfo<Type>();
		return retVal;
	}

	static reflect::VariableTypeInfo GetTypeInfo()
	{
		return GetClassTypeInfo();
	}
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_math/math_casts.h"


namespace RF { namespace rftype { namespace extensions {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueType, typename Allocator>
struct Accessor<std::vector<ValueType, Allocator>> final : private AccessorTemplate
{
	static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

	using AccessedType = std::vector<ValueType, Allocator>;
	static constexpr bool kExists = true;

	// NOTE: Size_t may vary between platforms, so a 32-bit save could appear
	//  later as a 64-bit load, which deserializers should detect and handle
	using KeyType = size_t;

	static size_t GetNumVariables( RootInst root )
	{
		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		return pThis->size();
	}

	static VariableTypeInfo GetVariableKeyInfoByIndex( RootInst root, size_t index )
	{
		// Always key by same type
		VariableTypeInfo retVal{};
		retVal.mValueType = Value::DetermineType<KeyType>();
		return retVal;
	}

	static Value GetVariableKeyAsValueByIndex( RootInst root, size_t index )
	{
		// Keyed directly by index
		return Value( math::integer_cast<KeyType>( index ) );
	}

	static VariableTypeInfo GetVariableTargetInfoByValue( RootInst root, Value key )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

		// All targets are always the same type in STL containers
		VariableTypeInfo retVal{};
		retVal.mValueType = Value::DetermineType<ValueType>();
		return retVal;
	}

	static Value GetVariableTargetAsValueByKeyAsValue( RootInst root, Value key )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

		if( key.GetStoredType() != Value::DetermineType<KeyType>() )
		{
			RF_DBGFAIL_MSG( "Key type differs from expected" );
			return Value{};
		}

		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		KeyType const index = *key.GetAs<KeyType>();
		static_assert( std::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index >= pThis->size() )
		{
			RF_DBGFAIL_MSG( "Index out of bounds" );
			return Value{};
		}

		return Value( pThis->at( index ) );
	}

	static ExtensionAccessor Get()
	{
		ExtensionAccessor retVal{};

		retVal.mGetNumVariables = &GetNumVariables;
		retVal.mGetVariableKeyInfoByIndex = &GetVariableKeyInfoByIndex;
		retVal.mGetVariableKeyAsValueByIndex = &GetVariableKeyAsValueByIndex;
		retVal.mGetVariableTargetInfoByValue = &GetVariableTargetInfoByValue;

		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );
		retVal.mGetVariableTargetAsValueByKeyAsValue = &GetVariableTargetAsValueByKeyAsValue;

		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}}}

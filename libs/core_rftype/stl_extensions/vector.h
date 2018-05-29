#pragma once

#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_math/math_casts.h"


namespace RF { namespace rftype { namespace extensions {
///////////////////////////////////////////////////////////////////////////////
namespace vector_accessor_details {
size_t const* GetStableKey( size_t key );
}

template<typename ValueType, typename Allocator>
struct Accessor<rftl::vector<ValueType, Allocator>> final : private AccessorTemplate
{
	static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

	using AccessedType = rftl::vector<ValueType, Allocator>;
	static constexpr bool kExists = true;

	// NOTE: Size_t may vary between platforms, so a 32-bit save could appear
	//  later as a 64-bit load, which deserializers should detect and handle
	using KeyType = size_t;

	static VariableTypeInfo GetDirectKeyInfo( RootInst root )
	{
		// Always key by same type
		VariableTypeInfo retVal{};
		retVal.mValueType = Value::DetermineType<KeyType>();
		return retVal;
	}

	static VariableTypeInfo GetSharedTargetInfo( RootInst root )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

		// All targets are always the same type in STL containers
		VariableTypeInfo retVal{};
		retVal.mValueType = Value::DetermineType<ValueType>();
		return retVal;
	}

	static size_t GetNumVariables( RootInst root )
	{
		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		return pThis->size();
	}

	static VariableTypeInfo GetVariableKeyInfoByIndex( RootInst root, size_t index )
	{
		return GetDirectKeyInfo( root );
	}

	static bool GetVariableKeyByIndex( RootInst root, size_t index, UntypedConstInst& key, VariableTypeInfo& keyInfo )
	{
		// Keyed directly by index
		key = vector_accessor_details::GetStableKey( index );
		keyInfo = GetVariableKeyInfoByIndex( root, index );
		return true;
	}

	static VariableTypeInfo GetVariableTargetInfoByKey( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		return GetSharedTargetInfo( root );
	}

	static bool GetVariableTargetByKey( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst& value, VariableTypeInfo& valueInfo )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

		if( keyInfo.mValueType != Value::DetermineType<KeyType>() )
		{
			RF_DBGFAIL_MSG( "Key type differs from expected" );
			return false;
		}

		KeyType const* castedKey = reinterpret_cast<KeyType const*>( key );
		if( castedKey == nullptr )
		{
			RF_DBGFAIL_MSG( "Key is null" );
			return false;
		}

		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index >= pThis->size() )
		{
			RF_DBGFAIL_MSG( "Index out of bounds" );
			return false;
		}

		value = &( pThis->at( index ) );
		valueInfo = GetVariableTargetInfoByKey( root, key, keyInfo );
		return true;
	}

	static bool mInsertVariableViaCopy( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst value, VariableTypeInfo const& valueInfo )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "TODO: Support for non-value types" );

		if( keyInfo.mValueType != Value::DetermineType<KeyType>() )
		{
			RF_DBGFAIL_MSG( "Key type differs from expected" );
			return false;
		}

		if( valueInfo.mValueType != Value::DetermineType<ValueType>() )
		{
			RF_DBGFAIL_MSG( "Value type differs from expected" );
			return false;
		}

		KeyType const* castedKey = reinterpret_cast<KeyType const*>( key );
		if( castedKey == nullptr )
		{
			RF_DBGFAIL_MSG( "Key is null" );
			return false;
		}

		ValueType const* castedValue = reinterpret_cast<ValueType const*>( value );
		if( castedKey == nullptr )
		{
			RF_DBGFAIL_MSG( "Key is null" );
			return false;
		}

		AccessedType* const pThis = reinterpret_cast<AccessedType*>( root );
		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index >= pThis->size() )
		{
			pThis->resize( index + 1 );
		}

		pThis->at( index ) = *castedValue;
		return true;
	}

	static ExtensionAccessor Get()
	{
		ExtensionAccessor retVal{};

		retVal.mGetDirectKeyInfo = &GetDirectKeyInfo;
		retVal.mGetSharedTargetInfo = &GetSharedTargetInfo;

		retVal.mGetNumVariables = &GetNumVariables;
		retVal.mGetVariableKeyInfoByIndex = &GetVariableKeyInfoByIndex;
		retVal.mGetVariableKeyByIndex = &GetVariableKeyByIndex;
		retVal.mGetVariableTargetInfoByKey = &GetVariableTargetInfoByKey;

		retVal.mGetVariableTargetByKey = &GetVariableTargetByKey;

		// TODO: Move support
		retVal.mInsertVariableViaCopy = &mInsertVariableViaCopy;

		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}}}

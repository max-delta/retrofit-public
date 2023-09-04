#pragma once

#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_rftype/TypeInference.h"
#include "core_math/math_casts.h"


namespace RF::rftype::extensions {
///////////////////////////////////////////////////////////////////////////////
namespace basic_string_accessor_details {
size_t const* GetStableKey( size_t key );
}

template<typename ValueType, typename Allocator>
struct Accessor<rftl::basic_string<ValueType, Allocator>> final : private AccessorTemplate
{
	static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "String only supports value types that do not rely on constructors" );

	using AccessedType = rftl::basic_string<ValueType, Allocator>;
	static constexpr bool kExists = true;

	// NOTE: Size_t may vary between platforms, so a 32-bit save could appear
	//  later as a 64-bit load, which deserializers should detect and handle
	using KeyType = size_t;

	static VariableTypeInfo GetDirectKeyInfo( RootConstInst root )
	{
		// Always key by same type
		return TypeInference<KeyType>::GetValueTypeInfo();
	}

	static VariableTypeInfo GetSharedKeyInfo( RootConstInst root )
	{
		// Key is always the same type
		return TypeInference<KeyType>::GetValueTypeInfo();
	}

	static VariableTypeInfo GetSharedTargetInfo( RootConstInst root )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "String only supports value types that do not rely on constructors" );

		// All targets are always the same type in STL containers
		return TypeInference<ValueType>::GetValueTypeInfo();
	}

	static size_t GetNumVariables( RootConstInst root )
	{
		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		return pThis->size();
	}

	static VariableTypeInfo GetKeyInfoByIndex( RootConstInst root, size_t index )
	{
		return GetDirectKeyInfo( root );
	}

	static bool GetKeyByIndex( RootConstInst root, size_t index, UntypedConstInst& key, VariableTypeInfo& keyInfo )
	{
		// Keyed directly by index
		key = basic_string_accessor_details::GetStableKey( index );
		keyInfo = GetKeyInfoByIndex( root, index );
		return true;
	}

	static VariableTypeInfo GetTargetInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		return GetSharedTargetInfo( root );
	}

	template<typename AccessedTypeT, typename RootT, typename ValueT>
	static bool GetTargetByKeyHelper( RootT root, UntypedConstInst key, VariableTypeInfo const& keyInfo, ValueT& value, VariableTypeInfo& valueInfo )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "String only supports value types that do not rely on constructors" );

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

		AccessedTypeT* const pThis = reinterpret_cast<AccessedTypeT*>( root );
		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index >= pThis->size() )
		{
			RF_DBGFAIL_MSG( "Index out of bounds" );
			return false;
		}

		value = &( pThis->at( index ) );
		valueInfo = GetTargetInfoByKey( root, key, keyInfo );
		return true;
	}

	static bool GetTargetByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst& value, VariableTypeInfo& valueInfo )
	{
		return GetTargetByKeyHelper<AccessedType const>( root, key, keyInfo, value, valueInfo );
	}

	static bool GetMutableTargetByKey( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedInst& value, VariableTypeInfo& valueInfo )
	{
		return GetTargetByKeyHelper<AccessedType>( root, key, keyInfo, value, valueInfo );
	}

	static bool InsertVariableViaCopy( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst value, VariableTypeInfo const& valueInfo )
	{
		static_assert( Value::DetermineType<ValueType>() != Value::Type::Invalid, "String only supports value types that do not rely on constructors" );

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
		if( castedValue == nullptr )
		{
			RF_DBGFAIL_MSG( "Value is null" );
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
		retVal.mGetSharedKeyInfo = &GetSharedKeyInfo;
		retVal.mGetSharedTargetInfo = &GetSharedTargetInfo;

		retVal.mGetNumVariables = &GetNumVariables;
		retVal.mGetKeyInfoByIndex = &GetKeyInfoByIndex;
		retVal.mGetKeyByIndex = &GetKeyByIndex;
		retVal.mGetTargetInfoByKey = &GetTargetInfoByKey;

		retVal.mGetTargetByKey = &GetTargetByKey;
		retVal.mGetMutableTargetByKey = &GetMutableTargetByKey;

		// TODO: Move support
		retVal.mInsertVariableViaCopy = &InsertVariableViaCopy;

		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}

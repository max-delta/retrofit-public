#pragma once

#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_rftype/TypeInference.h"
#include "core_math/math_casts.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::rftype::extensions {
///////////////////////////////////////////////////////////////////////////////
namespace uniqueptr_accessor_details {
size_t const* GetStableKey();
}

template<typename ValueType>
struct Accessor<UniquePtr<ValueType>> final : private AccessorTemplate
{
	using AccessedType = UniquePtr<ValueType>;
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
		// Target is always the same type
		return TypeInference<ValueType>::GetTypeInfo();
	}

	static size_t GetNumVariables( RootConstInst root )
	{
		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		if( pThis->Get() == nullptr )
		{
			return 0;
		}
		return 1;
	}

	static VariableTypeInfo GetKeyInfoByIndex( RootConstInst root, size_t index )
	{
		return GetDirectKeyInfo( root );
	}

	static bool GetKeyByIndex( RootConstInst root, size_t index, UntypedConstInst& key, VariableTypeInfo& keyInfo )
	{
		// There is only one valid key, and it's always the same
		RF_ASSERT( index == 0 );
		key = uniqueptr_accessor_details::GetStableKey();
		keyInfo = GetDirectKeyInfo( root );
		return true;
	}

	static VariableTypeInfo GetTargetInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		return GetSharedTargetInfo( root );
	}

	template<typename AccessedTypeT, typename RootT, typename ValueT>
	static bool GetTargetByKeyHelper( RootT root, UntypedConstInst key, VariableTypeInfo const& keyInfo, ValueT& value, VariableTypeInfo& valueInfo )
	{
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

		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index != 0 )
		{
			RF_DBGFAIL_MSG( "Key value is invalid" );
			return false;
		}

		AccessedTypeT* const pThis = reinterpret_cast<AccessedTypeT*>( root );
		if( pThis->Get() == nullptr )
		{
			RF_DBGFAIL_MSG( "Index out of bounds" );
			return false;
		}

		value = pThis->Get();
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

	// Enable only if ValueType can be default constructed
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value, int>::type = 0>
	static bool InsertVariableDefault( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
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

		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index != 0 )
		{
			RF_DBGFAIL_MSG( "Key is invalid" );
			return false;
		}

		AccessedType* const pThis = reinterpret_cast<AccessedType*>( root );
		// NOTE: Not checking for presence of existing value, just stomping

		// HACK: Dubious, this probably doesn't work with things like non-copyable
		//  or non-defaultable types
		// TODO: Conditionally allow this operation, or forbid it entirely and
		//  require different ways to assign it
		*pThis = DefaultCreator<ValueType>::Create( ValueType{} );
		return true;
	}

	// Enable only if ValueType can be default constructed
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value, int>::type = 0>
	static bool InsertVariableViaCopy( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst value, VariableTypeInfo const& valueInfo )
	{
		if( keyInfo.mValueType != Value::DetermineType<KeyType>() )
		{
			RF_DBGFAIL_MSG( "Key type differs from expected" );
			return false;
		}

		// Make sure the value coming in matches what we store
		VariableTypeInfo const storedTypeInfo = GetSharedTargetInfo( root );
		if( storedTypeInfo != valueInfo )
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

		KeyType const index = *castedKey;
		static_assert( rftl::is_unsigned<KeyType>::value, "Assuming unsigned" );
		if( index != 0 )
		{
			RF_DBGFAIL_MSG( "Key is invalid" );
			return false;
		}

		ValueType const* castedValue = reinterpret_cast<ValueType const*>( value );
		if( castedValue == nullptr )
		{
			RF_DBGFAIL_MSG( "Value is null" );
			return false;
		}

		AccessedType* const pThis = reinterpret_cast<AccessedType*>( root );
		// NOTE: Not checking for presence of existing value, just stomping

		// HACK: Dubious, this probably doesn't work with things like non-copyable
		//  types
		// TODO: Conditionally allow this operation, or forbid it entirely and
		//  require different ways to assign it
		*pThis = DefaultCreator<ValueType>::Create( *castedValue );
		return true;
	}

	// Conditional function pointer checks
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value, int>::type = 0>
	static ExtensionAccessor::FunctPtrInsertVariableDefault Conditional_InsertVariableDefault_Pointer()
	{
		return &InsertVariableDefault;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value == false, int>::type = 0>
	static ExtensionAccessor::FunctPtrInsertVariableDefault Conditional_InsertVariableDefault_Pointer()
	{
		return nullptr;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value, int>::type = 0>
	static ExtensionAccessor::FunctPtrInsertVariableViaCopy Conditional_InsertVariableViaCopy_Pointer()
	{
		return &InsertVariableViaCopy;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_default_constructible<ValueTypeTest>::value == false, int>::type = 0>
	static ExtensionAccessor::FunctPtrInsertVariableViaCopy Conditional_InsertVariableViaCopy_Pointer()
	{
		return nullptr;
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

		retVal.mInsertVariableDefault = Conditional_InsertVariableDefault_Pointer();
		// TODO: Move support
		retVal.mInsertVariableViaCopy = Conditional_InsertVariableViaCopy_Pointer();
		// TODO: UniquePtr support

		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}

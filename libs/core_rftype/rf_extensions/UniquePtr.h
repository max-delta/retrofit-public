#pragma once

#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_rftype/TypeInference.h"
#include "core_rftype/VirtualCast.h"
#include "core_rftype/VirtualPtrCast.h"
#include "core_math/math_casts.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::rftype::extensions {
///////////////////////////////////////////////////////////////////////////////
namespace uniqueptr_accessor_details {
size_t const* GetStableKey();

template<
	typename TargetT,
	typename SourceT,
	typename rftl::enable_if<
		rftl::is_base_of<reflect::VirtualClassWithoutDestructor, TargetT>::value &&
			rftl::is_base_of<reflect::VirtualClassWithoutDestructor, SourceT>::value,
		int>::type = 0>
UniquePtr<TargetT> conditional_virtual_ptr_cast( UniquePtr<SourceT>&& source )
{
	return virtual_ptr_cast<TargetT>( rftl::move( source ) );
}

template<
	typename TargetT,
	typename SourceT,
	typename rftl::enable_if<
		rftl::is_base_of<reflect::VirtualClassWithoutDestructor, TargetT>::value == false ||
			rftl::is_base_of<reflect::VirtualClassWithoutDestructor, SourceT>::value == false,
		int>::type = 0>
UniquePtr<TargetT> conditional_virtual_ptr_cast( UniquePtr<SourceT>&& source )
{
	return nullptr;
}

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

	// Enable only if ValueType isn't a virtual reflectable type
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value == false, int>::type = 0>
	static VariableTypeInfo GetSharedTargetInfo( RootConstInst root )
	{
		// If not virtual and reflectable, target is always the same type
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

	template<typename RootT>
	static bool IsValidZeroKeyHelper( RootT root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
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

		return true;
	}

	// Varies depending on whether the type is virtual and reflectable
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value, int>::type = 0>
	static VariableTypeInfo GetTargetInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		// Virtual and reflectable

		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		if( pThis->Get() == nullptr )
		{
			// No storage yet, return the type of the base class
			// NOTE: This is slightly sketchy, but is used by some callers so
			//  that they can try to figure out what's expected of them to
			//  provide, for cases where they don't even know if it's supposed
			//  to be a class, a primitive type, or a whole other accessor
			RF_TODO_ANNOTATION(
				"A better mechanism for callers to ask the question 'What"
				" should I insert here?'" );
			return TypeInference<ValueType>::GetTypeInfo();
		}

		VariableTypeInfo retVal = {};
		retVal.mClassInfo = pThis->Get()->GetVirtualClassInfo();
		RF_ASSERT_MSG( retVal.mClassInfo != nullptr,
			"Invalid virtual, has no class info, corrupt due to improper API"
			" usage of reflection declarations?" );
		return retVal;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value == false, int>::type = 0>
	static VariableTypeInfo GetTargetInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		// Possibly virtual, but non-reflectable if so

		return GetSharedTargetInfo( root );
	}

	// Varies depending on whether the type is virtual and reflectable
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value, int>::type = 0>
	static rftl::optional<IndirectionInfo> GetTargetIndirectionInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		// Virtual and reflectable

		if( IsValidZeroKeyHelper( root, key, keyInfo ) == false )
		{
			RF_DBGFAIL_MSG( "Not a valid zero key" );
			return rftl::nullopt;
		}

		AccessedType const* const pThis = reinterpret_cast<AccessedType const*>( root );
		if( pThis->Get() == nullptr )
		{
			RF_DBGFAIL_MSG( "Index out of bounds" );
			return rftl::nullopt;
		}

		// Instance type might differ from pointer type
		IndirectionInfo retVal = {};
		retVal.mMemoryLookupPointerType = TypeInference<ValueType>::GetTypeInfo();
		retVal.mMemoryLookupInstanceType = VariableTypeInfo{};
		retVal.mMemoryLookupInstanceType->mClassInfo = pThis->Get()->GetVirtualClassInfo();
		RF_ASSERT_MSG( retVal.mMemoryLookupInstanceType->mClassInfo != nullptr,
			"Invalid virtual, has no class info, corrupt due to improper API"
			" usage of reflection declarations?" );
		return retVal;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value == false, int>::type = 0>
	static rftl::optional<IndirectionInfo> GetTargetIndirectionInfoByKey( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo )
	{
		// Possibly virtual, but non-reflectable if so

		if( IsValidZeroKeyHelper( root, key, keyInfo ) == false )
		{
			RF_DBGFAIL_MSG( "Not a valid zero key" );
			return rftl::nullopt;
		}

		// Instance is same as pointer
		VariableTypeInfo const pointerType = GetSharedTargetInfo( root );
		IndirectionInfo retVal = {};
		retVal.mMemoryLookupPointerType = pointerType;
		retVal.mMemoryLookupInstanceType = pointerType;
		return retVal;
	}

	template<typename AccessedTypeT, typename RootT, typename ValueT>
	static bool GetTargetByKeyHelper( RootT root, UntypedConstInst key, VariableTypeInfo const& keyInfo, ValueT& value, VariableTypeInfo& valueInfo )
	{
		if( IsValidZeroKeyHelper( root, key, keyInfo ) == false )
		{
			RF_DBGFAIL_MSG( "Not a valid zero key" );
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

	static bool InsertVariableViaUPtr( RootInst root, UniquePtr<void>&& key, VariableTypeInfo const& keyInfo, UniquePtr<void>&& value, VariableTypeInfo const& valueInfo )
	{
		if( keyInfo.mValueType != Value::DetermineType<KeyType>() )
		{
			RF_DBGFAIL_MSG( "Key type differs from expected" );
			return false;
		}

		KeyType const* castedKey = reinterpret_cast<KeyType const*>( key.Get() );
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

		// Need to somehow get something in this pointer
		UniquePtr<ValueType> storableValue;

		// Check type
		// NOTE: Can't use GetSharedTargetInfo(...) here, since it might be a
		//  virtual reflection type, which at time of writing has variantly
		//  typed targets, so conditionally compiles out the support for
		//  GetSharedTargetInfo(...) in those cases
		VariableTypeInfo const storedTypeInfo = TypeInference<ValueType>::GetTypeInfo();
		if( valueInfo == storedTypeInfo )
		{
			// Same type, blindly cast and hope for the best
			PtrTransformer<ValueType>::PerformNonTypesafeTransformation( rftl::move( value ), storableValue );
		}
		else
		{
			// Different type, will be complicated

			// Only support classes
			// NOTE: For non-class types, expect caller to convert them before
			//  trying to store them in the accessor, to reduce complexity
			//  inside the accessor code
			if( storedTypeInfo.mClassInfo == nullptr )
			{
				RF_DBGFAIL_MSG( "Stored type isn't a class" );
				return false;
			}
			if( valueInfo.mClassInfo == nullptr )
			{
				RF_DBGFAIL_MSG( "Value isn't a class" );
				return false;
			}

			// Only support virtuals
			// NOTE: For non-virtual types, don't have a way to support the
			//  polymorphism that is presumably being asked for here, with
			//  these two types not being the same
			if( storedTypeInfo.mClassInfo->mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor == false )
			{
				RF_DBGFAIL_MSG( "Stored type isn't a virtual" );
				return false;
			}
			if( valueInfo.mClassInfo->mVirtualRootInfo.mDerivesFromVirtualClassWithoutDestructor == false )
			{
				RF_DBGFAIL_MSG( "Value isn't a virtual" );
				return false;
			}

			// Convert to a virtual pointer, which theoretically shouldn't fail
			//  unless it's something stupid like virtual diamond inheritance
			UniquePtr<reflect::VirtualClassWithoutDestructor> casted =
				rftype::virtual_reflect_ptr_cast(
					*valueInfo.mClassInfo,
					rftl::move( value ) );
			if( casted == nullptr )
			{
				RF_DBGFAIL_MSG( "Value couldn't be coerced into a virtual pointer" );
				return false;
			}

			// Attempt the downcast, which might fail if there's no inheritance
			//  path to the target type
			storableValue = uniqueptr_accessor_details::conditional_virtual_ptr_cast<ValueType>(
				rftl::move( casted ) );
			if( storableValue == nullptr )
			{
				RF_DBGFAIL_MSG(
					"Value couldn't be downcast to the target type,"
					" the types may be unrelated or the inheritance chain"
					" might not be properly annotated to reflection" );
				return false;
			}
		}

		// Don't let non-null pointers be stored, as that breaks the pattern
		//  this accessor is using where it treats a nullptr as an empty
		//  container, not a container that contains a nullptr
		if( storableValue == nullptr )
		{
			RF_DBGFAIL_MSG( "Storable value is null" );
			return false;
		}

		AccessedType* const pThis = reinterpret_cast<AccessedType*>( root );
		// NOTE: Not checking for presence of existing value, just stomping
		*pThis = rftl::move( storableValue );
		return true;
	}

	// Conditional function pointer checks
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value == false, int>::type = 0>
	static ExtensionAccessor::FuncPtrGetSharedTargetInfo Conditional_GetSharedTargetInfo()
	{
		return &GetSharedTargetInfo;
	}
	template<typename ValueTypeTest = ValueType, typename rftl::enable_if<rftl::is_base_of<reflect::VirtualClassWithoutDestructor, ValueTypeTest>::value, int>::type = 0>
	static ExtensionAccessor::FuncPtrGetSharedTargetInfo Conditional_GetSharedTargetInfo()
	{
		return nullptr;
	}
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
		retVal.mGetSharedTargetInfo = Conditional_GetSharedTargetInfo();

		retVal.mGetNumVariables = &GetNumVariables;
		retVal.mGetKeyInfoByIndex = &GetKeyInfoByIndex;
		retVal.mGetKeyByIndex = &GetKeyByIndex;
		retVal.mGetTargetInfoByKey = &GetTargetInfoByKey;
		retVal.mGetTargetIndirectionInfoByKey = GetTargetIndirectionInfoByKey;

		retVal.mGetTargetByKey = &GetTargetByKey;
		retVal.mGetMutableTargetByKey = &GetMutableTargetByKey;

		retVal.mInsertVariableDefault = Conditional_InsertVariableDefault_Pointer();
		// TODO: Move support
		retVal.mInsertVariableViaCopy = Conditional_InsertVariableViaCopy_Pointer();
		retVal.mInsertVariableViaUPtr = &InsertVariableViaUPtr;

		return retVal;
	}
};

///////////////////////////////////////////////////////////////////////////////
}

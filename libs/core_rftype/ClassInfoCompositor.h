#pragma once

#include "core_rftype/ClassInfoAccessor.h"
#include "core_rftype/ExtensionAccessorLookup.h"
#include "core_reflect/TypeInfoBuilder.h"
#include "core_reflect/VariableTraits.h"
#include "core/meta/FailTemplate.h"
#include "core/meta/MemberTest.h"
#include "core/macros.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// Compositor for use in user-provided initialization
template<class CLASS>
struct ClassInfoCompositor
{
	RF_NO_COPY( ClassInfoCompositor );

	ClassInfoCompositor( reflect::ClassInfo& classInfo )
		: mClassInfo( classInfo )
	{
		// Set things that don't require user-provided info to figure out
		reflect::builder::CreateClassInfo<CLASS>( mClassInfo );
	}

	template<typename T>
	ClassInfoCompositor& BaseClass()
	{
		static_assert( rftl::is_base_of<T, CLASS>::value, "Not a base class" );
		static_assert( rftl::is_same<T, CLASS>::value == false, "Not a base class, is same class" );
		reflect::BaseClassInfo classInfo = {};
		classInfo.mBaseClassInfo = &GetClassInfo<T>();
		CLASS const* const kInvalidClass = reinterpret_cast<CLASS const*>( compiler::kInvalidNonNullPointer );

		// NOTE: Performing this transformation test requires the derived class
		//  to be cast to the base class. This is normally required for
		//  type-safe casts, and reflection COULD work around it, but that
		//  creates dangerous assumptions, which this test is trying to detect.
		// NOTE: In some inheritance implementations, the act of casting an
		//  invalid pointer creates run-time code that will crash, so you will
		//  need to use the more expensive ComplexBaseClass() instead
		bool const nonTrivialTransformationNeeded = reinterpret_cast<T const*>( kInvalidClass ) != kInvalidClass;
		if( nonTrivialTransformationNeeded )
		{
			classInfo.mGetBasePointerFromDerived = GetBasePointerFromDerived<T>;
		}
		else
		{
			classInfo.mGetBasePointerFromDerived = nullptr;
		}
		mClassInfo.mBaseTypes.emplace_back( rftl::move( classInfo ) );
		return *this;
	}

	template<typename T>
	ClassInfoCompositor& ComplexBaseClass()
	{
		static_assert( rftl::is_base_of<T, CLASS>::value, "Not a base class" );
		static_assert( rftl::is_same<T, CLASS>::value == false, "Not a base class, is same class" );
		reflect::BaseClassInfo classInfo = {};
		classInfo.mBaseClassInfo = &GetClassInfo<T>();
		classInfo.mGetBasePointerFromDerived = GetBasePointerFromDerived<T>;
		mClassInfo.mBaseTypes.emplace_back( rftl::move( classInfo ) );
		return *this;
	}

	template<typename T, typename rftl::enable_if<reflect::FunctionTraits<T>::kCallType == reflect::CallType::FreeStanding, int>::type = 0>
	ClassInfoCompositor& Method( char const* identifier, T method )
	{
		reflect::FreeStandingFunctionInfo funcInfo = {};
		reflect::builder::CreateFreeStandingFunctionInfo( funcInfo, method );
		funcInfo.mIdentifier = mClassInfo.StoreString( identifier );
		mClassInfo.mStaticFunctions.emplace_back( rftl::move( funcInfo ) );
		return *this;
	}

	template<typename T, typename rftl::enable_if<reflect::FunctionTraits<T>::kCallType == reflect::CallType::Member, int>::type = 0>
	ClassInfoCompositor& Method( char const* identifier, T method )
	{
		reflect::MemberFunctionInfo funcInfo = {};
		reflect::builder::CreateMemberFunctionInfo( funcInfo, method );
		funcInfo.mIdentifier = mClassInfo.StoreString( identifier );
		mClassInfo.mNonStaticFunctions.emplace_back( rftl::move( funcInfo ) );
		return *this;
	}

	template<typename T, typename rftl::enable_if<reflect::VariableTraits<T>::kVariableType == reflect::VariableType::FreeStanding, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::FreeStandingVariableInfo varInfo = {};
		reflect::builder::CreateFreeStandingVariableInfo( varInfo, variable );
		varInfo.mIdentifier = mClassInfo.StoreString( identifier );
		mClassInfo.mStaticVariables.emplace_back( rftl::move( varInfo ) );
		return *this;
	}

	template<typename T,
		typename rftl::enable_if<reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0,
		typename rftl::enable_if<reflect::Value::DetermineType<typename reflect::VariableTraits<T>::VariableType>() != reflect::Value::Type::Invalid, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		varInfo.mIdentifier = mClassInfo.StoreString( identifier );
		mClassInfo.mNonStaticVariables.emplace_back( rftl::move( varInfo ) );
		return *this;
	}

	template<typename T,
		typename rftl::enable_if<reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0,
		typename rftl::enable_if<reflect::Value::DetermineType<typename reflect::VariableTraits<T>::VariableType>() == reflect::Value::Type::Invalid, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		varInfo.mIdentifier = mClassInfo.StoreString( identifier );
		using NestedType = typename reflect::VariableTraits<T>::VariableType;
		static_assert( rftl::is_class<NestedType>::value, "A member variable doesn't appear to be a known value type, or a class/struct" );
		RF_ASSERT( varInfo.mVariableTypeInfo.mValueType == reflect::Value::Type::Invalid );
		varInfo.mVariableTypeInfo.mClassInfo = &( GetClassInfo<NestedType>() );
		RF_ASSERT( varInfo.mVariableTypeInfo.mAccessor == nullptr );
		mClassInfo.mNonStaticVariables.emplace_back( rftl::move( varInfo ) );
		return *this;
	}

	template<typename T,
		typename rftl::enable_if<reflect::VariableTraits<T>::kVariableType == reflect::VariableType::FreeStanding, int>::type = 0>
	ClassInfoCompositor& ExtensionProperty( char const* identifier, T variable )
	{
		static_assert( FailTemplate<T>(), "TODO" );
		return *this;
	}

	template<typename T,
		typename rftl::enable_if<reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0>
	ClassInfoCompositor& ExtensionProperty( char const* identifier, T variable )
	{
		// TODO: Templatize off this for RawProperty<...>(...) instead once the
		//  kinks are worked out, and replace usage of this function
		static_assert( extensions::Accessor<typename reflect::VariableTraits<T>::VariableType>::kExists, "Type doesn't have an extension accessor" );

		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		varInfo.mIdentifier = mClassInfo.StoreString( identifier );
		using ExtensionType = typename reflect::VariableTraits<T>::VariableType;
		static_assert( rftl::is_class<ExtensionType>::value, "A member variable doesn't appear to be a known value type, or a class/struct" );
		using Extension = extensions::Accessor<ExtensionType>;
		static_assert( rftl::is_same<ExtensionType, typename Extension::AccessedType>::value, "Accessor's type differs from lookup's type" );
		RF_ASSERT( varInfo.mVariableTypeInfo.mValueType == reflect::Value::Type::Invalid );
		RF_ASSERT( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		reflect::ExtensionAccessor const extensionAccessor = extensions::Accessor<ExtensionType>::Get();
		reflect::ExtensionAccessor const& emplacedAccessor = mClassInfo.mExtensionStorage.emplace_back( extensionAccessor );
		varInfo.mVariableTypeInfo.mAccessor = &emplacedAccessor;
		mClassInfo.mNonStaticVariables.emplace_back( rftl::move( varInfo ) );
		return *this;
	}


private:
	template<typename T>
	static void const* GetBasePointerFromDerived( void const* ptr )
	{
		return static_cast<T const*>( reinterpret_cast<CLASS const*>( ptr ) );
	}


private:
	reflect::ClassInfo& mClassInfo;
};



// Generated structure for CRT startup
template<class CLASS>
struct CRTCompositionTrigger
{
	RF_NO_COPY( CRTCompositionTrigger );

	CRTCompositionTrigger()
		: mFallbackClassInfo( ExplicitDefaultConstruct() )
		, mCompositor( GetClassInfoStorage<CLASS>() )
	{
		Initialize( mCompositor );
	}
	template<typename TCLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TCLASS, ___RFType_Static_ClassInfo ), int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		static_assert( rftl::is_same<TCLASS, CLASS>::value );
		return CLASS::___RFType_Static_ClassInfo();
	}
	template<typename TCLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( TCLASS, ___RFType_Static_ClassInfo ) == false, int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		static_assert( rftl::is_same<TCLASS, CLASS>::value );
		return mFallbackClassInfo;
	}
	void Initialize( ::RF::rftype::ClassInfoCompositor<CLASS>& ___RFType_Macro_Target );

	using BackingClassType = CLASS;
	::RF::reflect::ClassInfo mFallbackClassInfo;
	::RF::rftype::ClassInfoCompositor<CLASS> mCompositor;
};

///////////////////////////////////////////////////////////////////////////////
}

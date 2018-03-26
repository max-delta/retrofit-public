#pragma once

#include "core_rftype/ClassInfoAccessor.h"
#include "core_reflect/ClassInfoBuilder.h"
#include "core_reflect/VariableTraits.h"
#include "core/meta/MemberTest.h"
#include "core/macros.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

// Compositor for use in user-provided initialization
template<class CLASS>
struct ClassInfoCompositor
{
	ClassInfoCompositor( reflect::ClassInfo& classInfo )
		: mClassInfo( classInfo )
	{
		//
	}

	template<typename T>
	ClassInfoCompositor& BaseClass()
	{
		static_assert( std::is_base_of<T, CLASS>::value, "Not a base class" );
		static_assert( std::is_same<T, CLASS>::value == false, "Not a base class, is same class" );
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
		mClassInfo.mBaseTypes.emplace_back( std::move( classInfo ) );
		return *this;
	}

	template<typename T>
	ClassInfoCompositor& ComplexBaseClass()
	{
		static_assert( std::is_base_of<T, CLASS>::value, "Not a base class" );
		static_assert( std::is_same<T, CLASS>::value == false, "Not a base class, is same class" );
		reflect::BaseClassInfo classInfo = {};
		classInfo.mBaseClassInfo = &GetClassInfo<T>();
		classInfo.mGetBasePointerFromDerived = GetBasePointerFromDerived<T>;
		mClassInfo.mBaseTypes.emplace_back( std::move( classInfo ) );
		return *this;
	}

	template<typename T, typename std::enable_if< reflect::FunctionTraits<T>::kCallType == reflect::CallType::FreeStanding,int>::type = 0>
	ClassInfoCompositor& Method( char const* identifier, T method )
	{
		reflect::FreeStandingFunctionInfo funcInfo = {};
		reflect::builder::CreateFreeStandingFunctionInfo( funcInfo, method );
		mClassInfo.mStaticFunctions.emplace_back( std::move( funcInfo ) );
		return *this;
	}

	template<typename T, typename std::enable_if< reflect::FunctionTraits<T>::kCallType == reflect::CallType::Member, int>::type = 0>
	ClassInfoCompositor& Method( char const* identifier, T method )
	{
		reflect::MemberFunctionInfo funcInfo = {};
		reflect::builder::CreateMemberFunctionInfo( funcInfo, method );
		mClassInfo.mNonStaticFunctions.emplace_back( std::move( funcInfo ) );
		return *this;
	}

	template<typename T, typename std::enable_if< reflect::VariableTraits<T>::kVariableType == reflect::VariableType::FreeStanding, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::FreeStandingVariableInfo varInfo = {};
		reflect::builder::CreateFreeStandingVariableInfo( varInfo, variable );
		mClassInfo.mStaticVariables.emplace_back( std::move( varInfo ) );
		return *this;
	}

	template<typename T,
		typename std::enable_if< reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0,
		typename std::enable_if< reflect::Value::DetermineType<typename reflect::VariableTraits<T>::VariableType>() != reflect::Value::Type::Invalid, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		mClassInfo.mNonStaticVariables.emplace_back( std::move( varInfo ) );
		return *this;
	}

	template<typename T,
		typename std::enable_if< reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0,
		typename std::enable_if< reflect::Value::DetermineType<typename reflect::VariableTraits<T>::VariableType>() == reflect::Value::Type::Invalid, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		using NestedType = typename reflect::VariableTraits<T>::VariableType;
		static_assert( std::is_class<NestedType>::value, "A member variable doesn't appear to be a known value type, or a class/struct" );
		varInfo.mClassInfo = &( GetClassInfo<NestedType>() );
		mClassInfo.mNonStaticVariables.emplace_back( std::move( varInfo ) );
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
	CRTCompositionTrigger()
		: mFallbackClassInfo()
		, mCompositor( GetClassInfoStorage<CLASS>() )
	{
		Initialize( mCompositor );
	}
	template<typename CLASS, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ), int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		return CLASS::___RFType_Static_ClassInfo;
	}
	template<typename CLASS, typename std::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ) == false, int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		return mFallbackClassInfo;
	}
	void Initialize( ::RF::rftype::ClassInfoCompositor<CLASS>& ___RFType_Macro_Target );

	using BackingClassType = CLASS;
	::RF::reflect::ClassInfo mFallbackClassInfo;
	::RF::rftype::ClassInfoCompositor<CLASS> mCompositor;
};

///////////////////////////////////////////////////////////////////////////////
}}

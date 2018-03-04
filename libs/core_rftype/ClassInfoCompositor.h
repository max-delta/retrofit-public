#pragma once

#include "core_reflect/ClassInfoBuilder.h"
#include "core_reflect/VariableTraits.h"
#include "core/meta/MemberTest.h"
#include "core/macros.h"


RF_DECLARE_HAS_MEMBER_NAME_TEST( ___RFType_Static_ClassInfo );

namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

// Compositor for use in user-provided initialization
struct ClassInfoCompositor
{
	ClassInfoCompositor( reflect::ClassInfo& classInfo )
		: mClassInfo( classInfo )
	{
		//
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

	template<typename T, typename std::enable_if< reflect::VariableTraits<T>::kVariableType == reflect::VariableType::Member, int>::type = 0>
	ClassInfoCompositor& RawProperty( char const* identifier, T variable )
	{
		reflect::MemberVariableInfo varInfo = {};
		reflect::builder::CreateMemberVariableInfo( varInfo, variable );
		mClassInfo.mNonStaticVariables.emplace_back( std::move( varInfo ) );
		return *this;
	}

	reflect::ClassInfo& mClassInfo;
};



// Created by macro-machinery and exported, this will need to be imported when
//  bringing a ClassInfo definition cross-module, if static ClassInfo was not
//  exported as part of the class itself
template<class CLASS>
::RF::reflect::ClassInfo const& GetClassInfoWithinModule();



// Template specialization for lookup, relies on static member of class, which
//  is expected to inherit the module-export behavior of its containing class
template<typename CLASS, typename std::enable_if<RF_HAS_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ), int>::type = 0>
::RF::reflect::ClassInfo const& GetClassInfo()
{
	return CLASS::___RFType_Static_ClassInfo;
}



// Template specialization for lookup, assumed to be only within the module
//  that instantiated the class info
template<typename CLASS, typename std::enable_if<RF_HAS_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ) == false, int>::type = 0>
::RF::reflect::ClassInfo const& GetClassInfo()
{
	return GetClassInfoWithinModule<CLASS>();
}



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
	template<typename CLASS, typename std::enable_if<RF_HAS_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ), int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		return CLASS::___RFType_Static_ClassInfo;
	}
	template<typename CLASS, typename std::enable_if<RF_HAS_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ) == false, int>::type = 0>
	::RF::reflect::ClassInfo& GetClassInfoStorage()
	{
		return mFallbackClassInfo;
	}
	void Initialize( ::RF::rftype::ClassInfoCompositor& ___RFType_Macro_Target );

	using BackingClassType = CLASS;
	::RF::reflect::ClassInfo mFallbackClassInfo;
	::RF::rftype::ClassInfoCompositor mCompositor;
};

///////////////////////////////////////////////////////////////////////////////
}}

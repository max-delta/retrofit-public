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



// Created by macro-machinery and exported, this will need to be imported when
//  bringing a ClassInfo definition cross-module, if 
template<class CLASS>
::RF::reflect::ClassInfo const& GetClassInfoWithinModule();



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

	::RF::reflect::ClassInfo mFallbackClassInfo;
	::RF::rftype::ClassInfoCompositor mCompositor;
};

///////////////////////////////////////////////////////////////////////////////
}}



// Used to define and instantiate ClassInfo meta information
// NOTE: Must be used at the root namespace, due to definitions of forwards
// NOTE: Requires a fully-qualified CLASSTYPE parameter, due to root namespace
// EXAMPLE:
//  RFTYPE_CREATE_META( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//  	RFTYPE_META().Method( "method", &CL::method );
//  	RFTYPE_META().RawProperty( "variable", &CL::variable );
//  }
#define RFTYPE_CREATE_META( CLASSTYPE ) \
	/* Instance for CRT startup */ \
	/* NOTE: Internal linkage (namespace static) to avoid any issues with */ \
	/*  multiple-y defined symbols during link, since we're not encoding very */ \
	/*  much information into the variable name, and instead relying on access */ \
	/*  via a template method with a stronger type signature */ \
	static RF::rftype::CRTCompositionTrigger<CLASSTYPE> RF_CONCAT(___rftype_macro_generated_instance_, __LINE__) = RF::rftype::CRTCompositionTrigger<CLASSTYPE>(); \
	\
	/* Forward for user-provided initialization */ \
	/* NOTE: Forwarded to allow macro to end with the body of this function */ \
	/* NOTE: Forwarded as 1-time-use template instead of macro-magic, since */ \
	/*  macros can't transform namespaces in CLASSTYPE into variable names */ \
	template<class CLASS> \
	void ___rftype_macro_generated_initializer( ::RF::rftype::ClassInfoCompositor& ___RFType_Macro_Target ); \
	template<> \
	void ___rftype_macro_generated_initializer<CLASSTYPE>( ::RF::rftype::ClassInfoCompositor& ___RFType_Macro_Target ); \
	\
	namespace RF { namespace rftype { \
	/* Template specialization for lookup only within module */ \
	template<> \
	__declspec( dllexport ) ::RF::reflect::ClassInfo const& GetClassInfoWithinModule<CLASSTYPE>() \
	{ \
		return RF_CONCAT(___rftype_macro_generated_instance_, __LINE__).GetClassInfoStorage<CLASSTYPE>(); \
	} \
	\
	/* Template specialization for chaining composition trigger into initialization */ \
	template<> \
	void CRTCompositionTrigger<CLASSTYPE>::Initialize( ::RF::rftype::ClassInfoCompositor& ___RFType_Macro_Target ) \
	{ \
		___rftype_macro_generated_initializer<CLASSTYPE>( ___RFType_Macro_Target ); \
	} \
	}} \
	\
	/* Header before user-supplied directives */ \
	template<> \
	void ___rftype_macro_generated_initializer<CLASSTYPE>( ::RF::rftype::ClassInfoCompositor& ___RFType_Macro_Target )

// Used to access the compositor during initialization
// EXAMPLE:
//  RFTYPE_CREATE_META( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//  	RFTYPE_META().Method( "method", &CL::method );
//  	RFTYPE_META().RawProperty( "variable", &CL::variable );
//  }
#define RFTYPE_META() (___RFType_Macro_Target)

// Creates static storage within a class, primarily intended to tie the storage
//  to the class so it participates in module exports and imports that the
//  enclosing class specifies
// NOTE: Requires either a fully-qualified CLASSTYPE parameter or appropriate
//  namespace wrapping for te definition
// NOTE: Normal definition rules apply, including multiple definitions
// EXAMPLE:
//  struct CLASSTYPE
//  {
//  	RFTYPE_STATIC_CLASSINFO();
//  };
//  RFTYPE_DEFINE_STATIC_CLASSINFO( CLASSTYPE );
#define RFTYPE_STATIC_CLASSINFO() static ::RF::reflect::ClassInfo ___RFType_Static_ClassInfo
#define RFTYPE_DEFINE_STATIC_CLASSINFO( CLASSTYPE ) ::RF::reflect::ClassInfo CLASSTYPE::___RFType_Static_ClassInfo

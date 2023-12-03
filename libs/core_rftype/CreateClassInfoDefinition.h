#pragma once

#include "core_rftype/CreateClassInfoDeclaration.h"
#include "core_rftype/ClassInfoCompositor.h"

// Internal details for handling macro logic, do not call directly
#define ___RFTYPE_CREATE_META_IMPL( CLASSTYPE, ... ) \
	/* Instance for CRT startup */ \
	/* NOTE: Internal linkage (namespace static) to avoid any issues with */ \
	/*  multiple-y defined symbols during link, since we're not encoding very */ \
	/*  much information into the variable name, and instead relying on access */ \
	/*  via a template method with a stronger type signature */ \
	static RF::rftype::CRTCompositionTrigger<CLASSTYPE> RF_CONCAT( ___rftype_macro_generated_instance_, __LINE__ ){}; \
	/**/ \
	/* Forward for user-provided initialization */ \
	/* NOTE: Forwarded to allow macro to end with the body of this function */ \
	/* NOTE: Forwarded as 1-time-use template instead of macro-magic, since */ \
	/*  macros can't transform namespaces in CLASSTYPE into variable names */ \
	template<class CLASS> \
	void ___rftype_macro_generated_initializer( ::RF::rftype::ClassInfoCompositor<CLASSTYPE>& ___RFType_Macro_Target, ::RF::rftype::CRTCompositionTrigger<CLASS> const* __RFType_CRT_Trigger ); \
	template<> \
	void ___rftype_macro_generated_initializer<CLASSTYPE>( ::RF::rftype::ClassInfoCompositor<CLASSTYPE> & ___RFType_Macro_Target, ::RF::rftype::CRTCompositionTrigger<CLASSTYPE> const* __RFType_CRT_Trigger ); \
	/**/ \
	namespace RF::rftype { /* Opening rftype namespace */ \
	/* Template specialization for lookup only within module */ \
	template<> \
	RF_NO_LINK_STRIP ::RF::reflect::ClassInfo const& GetClassInfoWithinModule<CLASSTYPE>() \
	{ \
		return RF_CONCAT( ___rftype_macro_generated_instance_, __LINE__ ).GetClassInfoStorage<CLASSTYPE>(); \
	} \
	/**/ \
	/* Template specialization for chaining composition trigger into initialization */ \
	/* NOTE: This seemingly redundant hop is to escape the namespace before the */ \
	/*  macro ends, so users don't have to close it themselves, which would be */ \
	/*  syntactically disastrous on Intellisense, and human sanity (ex: 'MACRO{}}' ) */ \
	template<> \
	void CRTCompositionTrigger<CLASSTYPE>::Initialize( ::RF::rftype::ClassInfoCompositor<CLASSTYPE>& ___RFType_Macro_Target ) \
	{ \
		{ \
			/* Run any code the macro call has baked in */ \
			__VA_ARGS__; \
		} \
		___rftype_macro_generated_initializer<CLASSTYPE>( ___RFType_Macro_Target, this ); \
	} \
	} /* Closing rftype namespace */ \
	/**/ \
	/* Header before user-supplied directives */ \
	template<> \
	void ___rftype_macro_generated_initializer<CLASSTYPE>( ::RF::rftype::ClassInfoCompositor<CLASSTYPE> & ___RFType_Macro_Target, ::RF::rftype::CRTCompositionTrigger<CLASSTYPE> const* __RFType_CRT_Trigger )

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
	___RFTYPE_CREATE_META_IMPL( CLASSTYPE, ___RFType_Macro_Target.AutoVirtualRoot() )

// Alternative to RFTYPE_CREATE_META(...) that disables some things that fall
//  over in diamond cases, so if you're a novice programmer that's prone to
//  writing terrible code, then this may be required in some situations
// NOTE: Must be used at the root namespace, due to definitions of forwards
// NOTE: Requires a fully-qualified CLASSTYPE parameter, due to root namespace
// EXAMPLE:
//  RFTYPE_CREATE_META_FOR_DIAMONDS( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//  	RFTYPE_META().Method( "method", &CL::method );
//  	RFTYPE_META().RawProperty( "variable", &CL::variable );
//  }
#define RFTYPE_CREATE_META_FOR_DIAMONDS( CLASSTYPE ) \
	___RFTYPE_CREATE_META_IMPL( CLASSTYPE )

// Used to access the compositor during initialization
// EXAMPLE:
//  RFTYPE_CREATE_META( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//  	RFTYPE_META().Method( "method", &CL::method );
//  	RFTYPE_META().RawProperty( "variable", &CL::variable );
//  }
#define RFTYPE_META() ( ___RFType_Macro_Target )

// Used to access the backing type during initialization
// EXAMPLE:
//  RFTYPE_CREATE_META( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//		static_assert( rftl::is_same<RFTYPE_METATYPE(), CLASS>::value, "Unexpected type" );
//  }
#define RFTYPE_METATYPE() typename rftl::remove_reference<decltype( *__RFType_CRT_Trigger )>::type::BackingClassType

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
#define RFTYPE_DEFINE_STATIC_CLASSINFO( CLASSTYPE ) \
	::RF::reflect::ClassInfo& CLASSTYPE::___RFType_Static_ClassInfo() \
	{ \
		static ::RF::reflect::ClassInfo sClassInfo{ ::RF::ExplicitDefaultConstruct() }; \
		return sClassInfo; \
	}

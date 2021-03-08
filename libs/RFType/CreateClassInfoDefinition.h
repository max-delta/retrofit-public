#pragma once
#include "project.h"

#include "core_rftype/CreateClassInfoDefinition.h"

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

RFTYPE_API void GlobalRegisterNewClassByName( char const* name, reflect::ClassInfo const& classInfo );
RFTYPE_API void GlobalRegisterNewClassByQualifiedName( char const* name, reflect::ClassInfo const& classInfo );

///////////////////////////////////////////////////////////////////////////////
}

// Used to register ClassInfo meta information in the type database
// NOTE: Must be used at the root namespace, due to definitions of forwards
// NOTE: Requires a fully-qualified CLASSTYPE parameter, due to root namespace
// EXAMPLE:
//  RFTYPE_CREATE_META( NS1::NS2::CLASS )
//  {
//  	using namespace NS1::NS2;
//  	RFTYPE_META().Method( "method", &CL::method );
//  	RFTYPE_META().RawProperty( "variable", &CL::variable );
//		RFTYPE_REGISTER_BY_NAME( "CLASS" );
//		RFTYPE_REGISTER_BY_QUALIFIED_NAME( NS1::NS2::CLASS );
//  }
#define RFTYPE_REGISTER_BY_NAME( CLASSNAME ) \
	::RF::rftype::GlobalRegisterNewClassByName( CLASSNAME, ::RF::rftype::GetClassInfo<RFTYPE_METATYPE()>() )
#define RFTYPE_REGISTER_BY_QUALIFIED_NAME( CLASSNAME ) \
	static_assert( alignof(::CLASSNAME) != 123456, "Unused test for compilation check of type name" );\
	::RF::rftype::GlobalRegisterNewClassByQualifiedName( #CLASSNAME, ::RF::rftype::GetClassInfo<RFTYPE_METATYPE()>() )

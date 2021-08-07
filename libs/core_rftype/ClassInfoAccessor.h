#pragma once

#include "core_reflect/ClassInfoBuilder.h"
#include "core_reflect/VariableTraits.h"
#include "core/meta/MemberTest.h"
#include "core/macros.h"


RF_DECLARE_HAS_PUBLIC_MEMBER_NAME_TEST( ___RFType_Static_ClassInfo );

namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// Created by macro-machinery and exported, this will need to be imported when
//  bringing a ClassInfo definition cross-module, if static ClassInfo was not
//  exported as part of the class itself
template<class CLASS>
::RF::reflect::ClassInfo const& GetClassInfoWithinModule();



// Template specialization for lookup, relies on static member of class, which
//  is expected to inherit the module-export behavior of its containing class
template<typename CLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ), int>::type = 0>
::RF::reflect::ClassInfo const& GetClassInfo()
{
	return CLASS::___RFType_Static_ClassInfo();
}



// Template specialization for lookup, assumed to be only within the module
//  that instantiated the class info
template<typename CLASS, typename rftl::enable_if<RF_HAS_PUBLIC_MEMBER_NAME( CLASS, ___RFType_Static_ClassInfo ) == false, int>::type = 0>
::RF::reflect::ClassInfo const& GetClassInfo()
{
	return GetClassInfoWithinModule<CLASS>();
}

///////////////////////////////////////////////////////////////////////////////
}

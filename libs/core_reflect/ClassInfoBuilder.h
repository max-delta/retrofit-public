#pragma once

#include "core_reflect/ClassInfo.h"
#include "core_reflect/FunctionTraits.h"


namespace RF::reflect::builder {
///////////////////////////////////////////////////////////////////////////////

template<class T>
void CreateClassInfo( ClassInfo& classInfo );

template<typename T>
void CreateFreeStandingVariableInfo( FreeStandingVariableInfo& variableInfo, T* variable );

template<typename T, typename Class>
void CreateMemberVariableInfo( MemberVariableInfo& variableInfo, T Class::*variable );

template<typename T>
void CreateFreeStandingFunctionInfo( FreeStandingFunctionInfo& functionInfo, T& function );

template<typename T>
void CreateMemberFunctionInfo( MemberFunctionInfo& functionInfo, T function );

///////////////////////////////////////////////////////////////////////////////
}

#include "ClassInfoBuilder.inl"

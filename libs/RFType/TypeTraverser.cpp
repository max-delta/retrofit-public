#include "stdafx.h"
#include "TypeTraverser.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

void TypeTraverser::TraverseVariables(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnNestedTypeFoundFunc const & onNestedTypeFoundFunc )
{
	TraverseVariablesT(
		traversalRoot,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
}



void TypeTraverser::TraverseVariables(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnNestedTypeFoundFunc const & onNestedTypeFoundFunc )
{
	TraverseVariablesT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "TypeTraverser.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

void TypeTraverser::TraverseVariables(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnNestedTypeFoundFunc const & onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFunc const& onReturnFromNestedTypeFunc )
{
	TraverseVariablesT(
		traversalRoot,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
}



void TypeTraverser::TraverseVariables(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnNestedTypeFoundFunc const & onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFunc const& onReturnFromNestedTypeFunc )
{
	TraverseVariablesT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
}

///////////////////////////////////////////////////////////////////////////////
}}

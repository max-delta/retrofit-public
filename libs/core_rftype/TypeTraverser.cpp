#include "stdafx.h"
#include "TypeTraverser.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

void TypeTraverser::TraverseVariables(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnTraversalFunc const & onTraversalFunc,
	OnReturnFromTraversalFunc const& onReturnFromTraversalFunc )
{
	TraverseVariablesT(
		traversalRoot,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}



void TypeTraverser::TraverseVariables(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFunc const & onMemberVariableFunc,
	OnTraversalFunc const & onTraversalFunc,
	OnReturnFromTraversalFunc const& onReturnFromTraversalFunc )
{
	TraverseVariablesT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "TypeTraverser.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

TypeTraverser::MemberVariableInstance::MemberVariableInstance(
	reflect::MemberVariableInfo const& memberVariableInfo,
	void const* memberVariableLocation )
	: mMemberVariableInfo( memberVariableInfo )
	, mMemberVariableLocation( memberVariableLocation )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

TypeTraverser::TraversalVariableInstance::TraversalVariableInstance(
	reflect::VariableTypeInfo const& variableTypeInfo,
	void const* const variableLocation )
	: mVariableTypeInfo( variableTypeInfo )
	, mVariableLocation( variableLocation )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

void TypeTraverser::TraverseVariables(
	reflect::VirtualClass const& traversalRoot,
	OnMemberVariableFunc const& onMemberVariableFunc,
	OnTraversalFunc const& onTraversalFunc,
	OnReturnFromTraversalFunc const& onReturnFromTraversalFunc )
{
	TraverseVariablesT(
		traversalRoot,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}



void TypeTraverser::TraverseVariables(
	reflect::ClassInfo const& classInfo,
	void const* classLocation,
	OnMemberVariableFunc const& onMemberVariableFunc,
	OnTraversalFunc const& onTraversalFunc,
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
}

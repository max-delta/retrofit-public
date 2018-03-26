#pragma once
#include "project.h"


// Forwards
namespace RF { namespace reflect {
	class VirtualClass;
	struct ClassInfo;
	struct MemberVariableInfo;
}}

namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

class RFTYPE_API TypeTraverser
{
	//
	// Structs
public:
	struct MemberVariableInstance
	{
		reflect::MemberVariableInfo const& mMemberVariableInfo;
		void const* const mMemberVariableLocation;
	};


	//
	// Types
public:
	using OnMemberVariableFunc = void( *)( MemberVariableInstance const& );
	using OnNestedTypeFoundFunc = void( *)( MemberVariableInstance const&, bool& shouldRecurse );


	//
	// Public methods
public:
	static void TraverseVariables(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnNestedTypeFoundFunc const& onNestedTypeFoundFunc );

	template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
	static void TraverseVariablesT(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc );

	static void TraverseVariables(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnNestedTypeFoundFunc const& onNestedTypeFoundFunc );

	template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
	static void TraverseVariablesT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc );


	//
	// Private methods
private:
	template< typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
	static void TraverseVariablesWithInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc );

	template< typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
	static void TraverseVariablesWithoutInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc );
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypeTraverser.inl"

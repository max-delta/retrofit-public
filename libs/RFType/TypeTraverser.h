#pragma once
#include "project.h"

#include "core/macros.h"


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
		MemberVariableInstance() = delete;
		MemberVariableInstance& operator=( MemberVariableInstance const& ) = delete;

		reflect::MemberVariableInfo const& mMemberVariableInfo;
		void const* const mMemberVariableLocation;
	};


	//
	// Types
public:
	using OnMemberVariableFunc = void( *)( MemberVariableInstance const& );
	using OnNestedTypeFoundFunc = void( *)( MemberVariableInstance const&, bool& shouldRecurse );
	using OnReturnFromNestedTypeFunc = void( *)( );


	//
	// Public methods
public:
	static void TraverseVariables(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnNestedTypeFoundFunc const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFunc const& onReturnFromNestedTypeFunc );

	template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
	static void TraverseVariablesT(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc );

	static void TraverseVariables(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnNestedTypeFoundFunc const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFunc const& onReturnFromNestedTypeFunc );

	template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
	static void TraverseVariablesT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc );


	//
	// Private methods
private:
	template< typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
	static void TraverseVariablesWithInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc );

	template< typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
	static void TraverseVariablesWithoutInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
		OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc );
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypeTraverser.inl"

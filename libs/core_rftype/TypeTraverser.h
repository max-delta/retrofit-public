#pragma once
#include "core/macros.h"


// Forwards
namespace RF { namespace reflect {
	class VirtualClass;
	struct ClassInfo;
	struct MemberVariableInfo;
	struct VariableTypeInfo;
}}

namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

class TypeTraverser
{
	//
	// Enums
public:
	enum class TraversalType
	{
		Invalid = 0,
		NestedType,
		Accessor,
		AccessorKey,
		AccessorTarget
	};


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

	struct TraversalVariableInstance
	{
		TraversalVariableInstance() = delete;
		TraversalVariableInstance& operator=( TraversalVariableInstance const& ) = delete;

		reflect::VariableTypeInfo const& mVariableTypeInfo;
		void const* const mVariableLocation;
	};


	//
	// Types
public:
	using OnMemberVariableFunc = void ( * )( MemberVariableInstance const& );
	using OnTraversalFunc = void ( * )( TraversalType, TraversalVariableInstance const&, bool& shouldRecurse );
	using OnReturnFromTraversalFunc = void ( * )( TraversalType, TraversalVariableInstance const& );


	//
	// Public methods
public:
	static void TraverseVariables(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnTraversalFunc const& onTraversalFunc,
		OnReturnFromTraversalFunc const& onReturnFromTraversalFunc );

	template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
	static void TraverseVariablesT(
		reflect::VirtualClass const& traversalRoot,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnTraversalFuncT const& onTraversalFunc,
		OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc );

	static void TraverseVariables(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFunc const& onMemberVariableFunc,
		OnTraversalFunc const& onTraversalFunc,
		OnReturnFromTraversalFunc const& onReturnFromTraversalFunc );

	template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
	static void TraverseVariablesT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnTraversalFuncT const& onTraversalFunc,
		OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc );


	//
	// Private methods
private:
	template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
	static void TraverseVariablesWithInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnTraversalFuncT const& onTraversalFunc,
		OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc );

	template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
	static void TraverseVariablesWithoutInheritanceT(
		reflect::ClassInfo const& classInfo,
		void const* classLocation,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnTraversalFuncT const& onTraversalFunc,
		OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc );
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypeTraverser.inl"

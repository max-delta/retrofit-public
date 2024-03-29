#pragma once
#include "core/macros.h"


// Forwards
namespace RF::reflect {
class VirtualClass;
struct ClassInfo;
struct IndirectionInfo;
struct MemberVariableInfo;
struct VariableTypeInfo;
}

namespace RF::rftype {
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
		MemberVariableInstance(
			reflect::MemberVariableInfo const& memberVariableInfo,
			void const* memberVariableLocation );
		MemberVariableInstance( MemberVariableInstance const& ) = default;
		MemberVariableInstance& operator=( MemberVariableInstance const& ) = delete;

		reflect::MemberVariableInfo const& mMemberVariableInfo;
		void const* const mMemberVariableLocation = nullptr;
	};

	struct TraversalVariableInstance
	{
		TraversalVariableInstance() = delete;
		TraversalVariableInstance(
			reflect::VariableTypeInfo const& variableTypeInfo,
			void const* const variableLocation );
		TraversalVariableInstance(
			reflect::VariableTypeInfo const& variableTypeInfo,
			void const* const variableLocation,
			reflect::IndirectionInfo const& indirectionInfo );
		TraversalVariableInstance( TraversalVariableInstance const& ) = default;
		TraversalVariableInstance& operator=( TraversalVariableInstance const& ) = delete;

		reflect::VariableTypeInfo const& mVariableTypeInfo;
		void const* const mVariableLocation = nullptr;
		reflect::IndirectionInfo const* const mIndirectionInfo = nullptr;
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

	template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
	static void TraverseVariableInternalT(
		reflect::VariableTypeInfo const& typeInfo,
		void const* varLoc,
		OnMemberVariableFuncT const& onMemberVariableFunc,
		OnTraversalFuncT const& onTraversalFunc,
		OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc );
};

///////////////////////////////////////////////////////////////////////////////
}

#include "TypeTraverser.inl"

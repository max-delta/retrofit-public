#pragma once
#include "TypeTraverser.h"

#include "core_reflect/VirtualClass.h"
#include "core_reflect/ClassInfo.h"
#include "core/macros.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc )
{
	TraverseVariablesWithInheritanceT(
		*traversalRoot.GetVirtualClassInfo(),
		&traversalRoot,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
}



template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const & onMemberVariableFunc,
	OnNestedTypeFoundFuncT const & onNestedTypeFoundFunc )
{
	TraverseVariablesWithInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
}

///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
inline void TypeTraverser::TraverseVariablesWithInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc )
{
	using namespace RF::reflect;

	for( BaseClassInfo const& baseClassInfo : classInfo.mBaseTypes )
	{
		void const* baseClassLocation;
		if( baseClassInfo.mGetBasePointerFromDerived != nullptr )
		{
			// Special handler needed, probably due to multiple-inheritance
			baseClassLocation = baseClassInfo.mGetBasePointerFromDerived( classLocation );
		}
		else
		{
			baseClassLocation = classLocation;
		}

		TraverseVariablesWithInheritanceT(
			*baseClassInfo.mBaseClassInfo,
			baseClassLocation,
			onMemberVariableFunc,
			onNestedTypeFoundFunc );
	}

	TraverseVariablesWithoutInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
}



template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT>
inline void TypeTraverser::TraverseVariablesWithoutInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc )
{
	using namespace RF::reflect;

	for( MemberVariableInfo const& varInfo : classInfo.mNonStaticVariables )
	{
		void const* const varLoc =
			reinterpret_cast<uint8_t const*>( classLocation ) +
			varInfo.mOffset;
		bool const nested =
			varInfo.mValueType == Value::Type::Invalid &&
			varInfo.mClassInfo != nullptr;

		if( nested )
		{
			bool shouldTraverse = false;
			onNestedTypeFoundFunc( { varInfo, varLoc }, shouldTraverse );
			if( shouldTraverse )
			{
				TraverseVariablesWithoutInheritanceT(
					*varInfo.mClassInfo,
					varLoc,
					onMemberVariableFunc,
					onNestedTypeFoundFunc );
			}
			continue;
		}

		onMemberVariableFunc( { varInfo, varLoc } );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

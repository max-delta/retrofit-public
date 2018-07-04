#pragma once
#include "TypeTraverser.h"

#include "core_reflect/VirtualClass.h"
#include "core_reflect/ClassInfo.h"
#include "core/macros.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc )
{
	TraverseVariablesWithInheritanceT(
		*traversalRoot.GetVirtualClassInfo(),
		&traversalRoot,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
}



template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const & onMemberVariableFunc,
	OnNestedTypeFoundFuncT const & onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc )
{
	TraverseVariablesWithInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
}

///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
inline void TypeTraverser::TraverseVariablesWithInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc )
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
			onNestedTypeFoundFunc,
			onReturnFromNestedTypeFunc );
	}

	TraverseVariablesWithoutInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
}



template<typename OnMemberVariableFuncT, typename OnNestedTypeFoundFuncT, typename OnReturnFromNestedTypeFuncT>
inline void TypeTraverser::TraverseVariablesWithoutInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnNestedTypeFoundFuncT const& onNestedTypeFoundFunc,
	OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc )
{
	using namespace RF::reflect;

	for( MemberVariableInfo const& varInfo : classInfo.mNonStaticVariables )
	{
		void const* const varLoc =
			reinterpret_cast<uint8_t const*>( classLocation ) +
			varInfo.mOffset;

		bool const nested =
			varInfo.mVariableTypeInfo.mValueType == Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo != nullptr &&
			varInfo.mVariableTypeInfo.mAccessor == nullptr;
		if( nested )
		{
			bool shouldTraverse = false;
			onNestedTypeFoundFunc( { varInfo, varLoc }, shouldTraverse );
			if( shouldTraverse )
			{
				TraverseVariablesWithoutInheritanceT(
					*varInfo.mVariableTypeInfo.mClassInfo,
					varLoc,
					onMemberVariableFunc,
					onNestedTypeFoundFunc,
					onReturnFromNestedTypeFunc );
			}
			continue;
		}

		bool const extension =
			varInfo.mVariableTypeInfo.mValueType == Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo == nullptr &&
			varInfo.mVariableTypeInfo.mAccessor != nullptr;
		if( extension )
		{
			RF_DBGFAIL_MSG( "TODO" );
			continue;
		}

		bool const value =
			varInfo.mVariableTypeInfo.mValueType != Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo == nullptr &&
			varInfo.mVariableTypeInfo.mAccessor == nullptr;
		if( value )
		{
			onMemberVariableFunc( { varInfo, varLoc } );
			continue;
		}

		RF_DBGFAIL_MSG( "Unknown variable type encountered during traversal" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

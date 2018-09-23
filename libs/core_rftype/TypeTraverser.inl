#pragma once
#include "TypeTraverser.h"

#include "core_reflect/VirtualClass.h"
#include "core_reflect/ClassInfo.h"
#include "core/macros.h"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::VirtualClass const & traversalRoot,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
	OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc )
{
	TraverseVariablesWithInheritanceT(
		*traversalRoot.GetVirtualClassInfo(),
		&traversalRoot,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}



template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const & onMemberVariableFunc,
	OnTraversalFuncT const & onTraversalFunc,
	OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc )
{
	TraverseVariablesWithInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}

///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariablesWithInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
	OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc )
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
			onTraversalFunc,
			onReturnFromTraversalFunc );
	}

	TraverseVariablesWithoutInheritanceT(
		classInfo,
		classLocation,
		onMemberVariableFunc,
		onTraversalFunc,
		onReturnFromTraversalFunc );
}



template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromNestedTypeFuncT>
inline void TypeTraverser::TraverseVariablesWithoutInheritanceT(
	reflect::ClassInfo const & classInfo,
	void const * classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
	OnReturnFromNestedTypeFuncT const& onReturnFromNestedTypeFunc )
{
	using namespace RF::reflect;

	for( MemberVariableInfo const& varInfo : classInfo.mNonStaticVariables )
	{
		// NOTE: Null class locations get null variable locations so they are
		//  clearly inaccessible
		void const* const varLoc = classLocation == nullptr ? nullptr :
			reinterpret_cast<uint8_t const*>( classLocation ) +
			varInfo.mOffset;

		onMemberVariableFunc( { varInfo, varLoc } );

		bool const value =
			varInfo.mVariableTypeInfo.mValueType != Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo == nullptr &&
			varInfo.mVariableTypeInfo.mAccessor == nullptr;
		if( value )
		{
			// Value type
			continue;
		}

		bool const nested =
			varInfo.mVariableTypeInfo.mValueType == Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo != nullptr &&
			varInfo.mVariableTypeInfo.mAccessor == nullptr;
		if( nested )
		{
			// Nested type

			bool shouldTraverse = false;
			onTraversalFunc( TraversalType::NestedType, { varInfo.mVariableTypeInfo, varLoc }, shouldTraverse );
			if( shouldTraverse )
			{
				TraverseVariablesWithoutInheritanceT(
					*varInfo.mVariableTypeInfo.mClassInfo,
					varLoc,
					onMemberVariableFunc,
					onTraversalFunc,
					onReturnFromNestedTypeFunc );

				onReturnFromNestedTypeFunc( TraversalType::NestedType, { varInfo.mVariableTypeInfo, varLoc } );
			}
			continue;
		}

		bool const extension =
			varInfo.mVariableTypeInfo.mValueType == Value::Type::Invalid &&
			varInfo.mVariableTypeInfo.mClassInfo == nullptr &&
			varInfo.mVariableTypeInfo.mAccessor != nullptr;
		if( extension )
		{
			// Extension type

			bool shouldTraverseAccessor = false;
			// TODO: Callback
			onTraversalFunc( TraversalType::Accessor, { varInfo.mVariableTypeInfo, varLoc }, shouldTraverseAccessor );
			if( shouldTraverseAccessor )
			{
				// NOTE: Can't perform traversal on null accessors, so they
				//  will behave like null pointers are empty containers
				if( varLoc != nullptr )
				{
					ExtensionAccessor const& accessor = *varInfo.mVariableTypeInfo.mAccessor;
					accessor.mBeginAccess( varLoc );
					size_t const numVars = accessor.mGetNumVariables( varLoc );
					for( size_t i = 0; i < numVars; i++ )
					{
						VariableTypeInfo keyInfo = {};
						void const* keyLoc = nullptr;
						bool const foundKey = accessor.mGetVariableKeyByIndex( varLoc, i, keyLoc, keyInfo );
						RF_ASSERT( foundKey );
						bool shouldTraverseKey = false;
						onTraversalFunc( TraversalType::AccessorKey, { keyInfo, keyLoc }, shouldTraverseKey );
						if( shouldTraverseKey )
						{
							RF_DBGFAIL_MSG( "TODO" );
							onReturnFromNestedTypeFunc( TraversalType::AccessorKey, { keyInfo, keyLoc } );
						}

						VariableTypeInfo targetInfo = {};
						void const* targetLoc = nullptr;
						bool const foundTarget = accessor.mGetVariableTargetByKey( varLoc, keyLoc, keyInfo, targetLoc, targetInfo );
						RF_ASSERT( foundTarget );
						bool shouldTraverseTarget = false;
						onTraversalFunc( TraversalType::AccessorTarget, { targetInfo, targetLoc }, shouldTraverseTarget );
						if( shouldTraverseTarget )
						{
							RF_DBGFAIL_MSG( "TODO" );
							onReturnFromNestedTypeFunc( TraversalType::AccessorTarget, { targetInfo, targetLoc } );
						}
					}
					accessor.mEndAccess( varLoc );
				}

				onReturnFromNestedTypeFunc( TraversalType::Accessor, { varInfo.mVariableTypeInfo, varLoc } );
			}
			continue;
		}

		RF_DBGFAIL_MSG( "Unknown variable type encountered during traversal" );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

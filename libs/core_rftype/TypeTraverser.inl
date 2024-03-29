#pragma once
#include "TypeTraverser.h"

#include "core_reflect/VirtualClass.h"
#include "core_reflect/ClassInfo.h"
#include "core/macros.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariablesT(
	reflect::VirtualClass const& traversalRoot,
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
	reflect::ClassInfo const& classInfo,
	void const* classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
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
	reflect::ClassInfo const& classInfo,
	void const* classLocation,
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



template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariablesWithoutInheritanceT(
	reflect::ClassInfo const& classInfo,
	void const* classLocation,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
	OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc )
{
	using namespace RF::reflect;

	for( MemberVariableInfo const& varInfo : classInfo.mNonStaticVariables )
	{
		// NOTE: Null class locations get null variable locations so they are
		//  clearly inaccessible
		void const* const varLoc =
			classLocation == nullptr ?
			nullptr :
			reinterpret_cast<uint8_t const*>( classLocation ) +
				varInfo.mOffset;

		onMemberVariableFunc( MemberVariableInstance( varInfo, varLoc ) );

		VariableTypeInfo const& typeInfo = varInfo.mVariableTypeInfo;

		TraverseVariableInternalT(
			typeInfo,
			varLoc,
			onMemberVariableFunc,
			onTraversalFunc,
			onReturnFromTraversalFunc );
	}
}



template<typename OnMemberVariableFuncT, typename OnTraversalFuncT, typename OnReturnFromTraversalFuncT>
inline void TypeTraverser::TraverseVariableInternalT(
	reflect::VariableTypeInfo const& typeInfo,
	void const* varLoc,
	OnMemberVariableFuncT const& onMemberVariableFunc,
	OnTraversalFuncT const& onTraversalFunc,
	OnReturnFromTraversalFuncT const& onReturnFromTraversalFunc )
{
	using namespace RF::reflect;

	bool const value =
		typeInfo.mValueType != Value::Type::Invalid &&
		typeInfo.mClassInfo == nullptr &&
		typeInfo.mAccessor == nullptr;
	if( value )
	{
		// Value type
		return;
	}

	bool const nested =
		typeInfo.mValueType == Value::Type::Invalid &&
		typeInfo.mClassInfo != nullptr &&
		typeInfo.mAccessor == nullptr;
	if( nested )
	{
		// Nested type

		bool shouldTraverse = false;
		onTraversalFunc( TraversalType::NestedType, TraversalVariableInstance( typeInfo, varLoc ), shouldTraverse );
		if( shouldTraverse )
		{
			TraverseVariablesWithoutInheritanceT(
				*typeInfo.mClassInfo,
				varLoc,
				onMemberVariableFunc,
				onTraversalFunc,
				onReturnFromTraversalFunc );

			onReturnFromTraversalFunc( TraversalType::NestedType, TraversalVariableInstance( typeInfo, varLoc ) );
		}
		return;
	}

	bool const extension =
		typeInfo.mValueType == Value::Type::Invalid &&
		typeInfo.mClassInfo == nullptr &&
		typeInfo.mAccessor != nullptr;
	if( extension )
	{
		// Extension type

		bool shouldTraverseAccessor = false;
		{
			// Callback
			onTraversalFunc(
				TraversalType::Accessor,
				TraversalVariableInstance( typeInfo, varLoc ),
				shouldTraverseAccessor );
		}
		if( shouldTraverseAccessor )
		{
			// NOTE: Can't perform traversal on null accessors, so they
			//  will behave like null pointers are empty containers
			if( varLoc != nullptr )
			{
				ExtensionAccessor const& accessor = *typeInfo.mAccessor;

				// Start access
				if( accessor.mBeginAccess != nullptr )
				{
					accessor.mBeginAccess( varLoc );
				}

				// For each index...
				size_t const numVars = accessor.mGetNumVariables( varLoc );
				for( size_t i = 0; i < numVars; i++ )
				{
					// Key, via index
					VariableTypeInfo keyInfo = {};
					void const* keyLoc = nullptr;
					bool const foundKey = accessor.mGetKeyByIndex( varLoc, i, keyLoc, keyInfo );
					RF_ASSERT( foundKey );
					bool shouldTraverseKey = false;
					{
						// Callback
						onTraversalFunc(
							TraversalType::AccessorKey,
							TraversalVariableInstance( keyInfo, keyLoc ),
							shouldTraverseKey );
					}
					if( shouldTraverseKey )
					{
						// Recurse
						TraverseVariableInternalT(
							keyInfo,
							keyLoc,
							onMemberVariableFunc,
							onTraversalFunc,
							onReturnFromTraversalFunc );

						// Callback
						onReturnFromTraversalFunc(
							TraversalType::AccessorKey,
							TraversalVariableInstance( keyInfo, keyLoc ) );
					}

					// Target, via key
					VariableTypeInfo targetInfo = {};
					void const* targetLoc = nullptr;
					bool const foundTarget = accessor.mGetTargetByKey( varLoc, keyLoc, keyInfo, targetLoc, targetInfo );
					RF_ASSERT( foundTarget );
					rftl::optional<reflect::IndirectionInfo> indirectionInfo = rftl::nullopt;
					if( accessor.mGetTargetIndirectionInfoByKey != nullptr )
					{
						indirectionInfo = accessor.mGetTargetIndirectionInfoByKey( varLoc, keyLoc, keyInfo );
					}
					bool shouldTraverseTarget = false;
					{
						// Callback
						onTraversalFunc(
							TraversalType::AccessorTarget,
							indirectionInfo.has_value() ?
								TraversalVariableInstance( targetInfo, targetLoc, indirectionInfo.value() ) :
								TraversalVariableInstance( targetInfo, targetLoc ),
							shouldTraverseTarget );
					}
					if( shouldTraverseTarget )
					{
						// Recurse
						TraverseVariableInternalT(
							targetInfo,
							targetLoc,
							onMemberVariableFunc,
							onTraversalFunc,
							onReturnFromTraversalFunc );

						// Callback
						onReturnFromTraversalFunc(
							TraversalType::AccessorTarget,
							TraversalVariableInstance( targetInfo, targetLoc ) );
					}
				}

				// End access
				if( accessor.mEndAccess != nullptr )
				{
					accessor.mEndAccess( varLoc );
				}
			}

			// Callback
			onReturnFromTraversalFunc(
				TraversalType::Accessor,
				TraversalVariableInstance( typeInfo, varLoc ) );
		}
		return;
	}

	RF_DBGFAIL_MSG( "Unknown variable type encountered during traversal" );
}

///////////////////////////////////////////////////////////////////////////////
}

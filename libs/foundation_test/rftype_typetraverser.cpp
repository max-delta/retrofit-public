#include "stdafx.h"

// TODO: Remove dependency on Example.h, and move these tests to core tests
#include "RFType/Example.h"

#include "core_rftype/TypeTraverser.h"

#include "rftl/unordered_set"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

void EnsureAllFunctionsFound( rftl::unordered_set<void const*> expectedLocations, rftl::vector<TypeTraverser::MemberVariableInstance> const& members )
{
	ASSERT_EQ( expectedLocations.size(), members.size() );
	for( TypeTraverser::MemberVariableInstance const& varInst : members )
	{
		void const* const varLoc = varInst.mMemberVariableLocation;
		ASSERT_TRUE( expectedLocations.count( varLoc ) == 1 );
		expectedLocations.erase( varLoc );
	}
	ASSERT_EQ( expectedLocations.size(), 0 );
}



void EnsureAllFunctionsFound( rftl::vector<void const*> expectedLocations, rftl::vector<TypeTraverser::MemberVariableInstance> const& members )
{
	ASSERT_EQ( expectedLocations.size(), members.size() );
	for( TypeTraverser::MemberVariableInstance const& varInst : members )
	{
		void const* const varLoc = varInst.mMemberVariableLocation;
		bool found = false;
		for( rftl::vector<void const*>::const_iterator iter = expectedLocations.begin(); iter != expectedLocations.end(); iter++ )
		{
			if( *iter == varLoc )
			{
				found = true;
				expectedLocations.erase( iter );
				break;
			}
		}
		ASSERT_TRUE( found );
	}
	ASSERT_EQ( expectedLocations.size(), 0 );
}



TEST( RFType, TraverseSingleInheritance )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	rftype_example::ExampleDerivedClass classInstance;
	TypeTraverser::TraverseVariablesT(
		*classInstance.GetVirtualClassInfo(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
	ASSERT_EQ( depth, 0 );

	rftl::unordered_set<void const*> expectedLocations;
	expectedLocations.emplace( &classInstance.mExampleNonStaticVariable );
	expectedLocations.emplace( &classInstance.mExampleDerivedNonStaticVariable );
	EnsureAllFunctionsFound( expectedLocations, members );
}



TEST( RFType, TraverseNullSingleInheritance )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleDerivedClass>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
	ASSERT_EQ( depth, 0 );
}



TEST( RFType, TraverseMultipleInheritance )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	rftype_example::ExamplePoorLifeDecision classInstance;
	TypeTraverser::TraverseVariablesT(
		*classInstance.GetVirtualClassInfo(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
	ASSERT_EQ( depth, 0 );

	rftl::unordered_set<void const*> expectedLocations;
	expectedLocations.emplace( &classInstance.mExampleNonStaticVariable );
	expectedLocations.emplace( &classInstance.mExampleSecondaryNonStaticVariable );
	EnsureAllFunctionsFound( expectedLocations, members );
}



TEST( RFType, TraverseNullMultipleInheritance )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExamplePoorLifeDecision>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
	ASSERT_EQ( depth, 0 );
}



TEST( RFType, TraverseNesting )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	rftype_example::ExampleWithClassAsMember classInstance;
	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleWithClassAsMember>(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 1 );
	ASSERT_EQ( depth, 0 );

	rftl::vector<void const*> expectedLocations;
	expectedLocations.emplace_back( &classInstance.mExampleClassAsMember );
	expectedLocations.emplace_back( &classInstance.mExampleClassAsMember.mExampleNonStaticVariable );
	EnsureAllFunctionsFound( expectedLocations, members );
}



TEST( RFType, TraverseNullNesting )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::TraversalVariableInstance> nested;
	size_t depth = 0;
	auto onNestedTypeFoundFunc = [&nested, &depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
		depth++;
	};

	auto onReturnFromNestedTypeFunc = [&depth](
		TypeTraverser::TraversalType traversalType,
		TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		depth--;
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleWithClassAsMember>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 1 );
	ASSERT_EQ( depth, 0 );
}



TEST( RFType, TraverseVecInts )
{
	// TODO
}



TEST( RFType, TraverseNullVecInts )
{
	// TODO
}



TEST( RFType, TraverseVecNested )
{
	// TODO
}



TEST( RFType, TraverseNullVecNested )
{
	// TODO
}



TEST( RFType, TraverseVecVecInts )
{
	// TODO
}



TEST( RFType, TraverseNullVecVecInts )
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////
}}

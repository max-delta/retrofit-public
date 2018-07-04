#include "stdafx.h"

// TODO: Remove dependency on Example.h, and move these tests to core tests
#include "RFType/Example.h"

#include "core_rftype/TypeTraverser.h"

#include "rftl/unordered_set"


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

TEST( RFType, TraverseSingleInheritance )
{
	rftl::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
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

	rftl::unordered_set<void const*> expectedLocations;
	expectedLocations.emplace( &classInstance.mExampleNonStaticVariable );
	expectedLocations.emplace( &classInstance.mExampleDerivedNonStaticVariable );
	ASSERT_EQ( expectedLocations.size(), 2 );
	for( TypeTraverser::MemberVariableInstance const& varInst : members )
	{
		void const* const varLoc = varInst.mMemberVariableLocation;
		ASSERT_TRUE( expectedLocations.count( varLoc ) == 1 );
		expectedLocations.erase( varLoc );
	}
	ASSERT_EQ( expectedLocations.size(), 0 );
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

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleDerivedClass>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
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

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
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

	rftl::unordered_set<void const*> expectedLocations;
	expectedLocations.emplace( &classInstance.mExampleNonStaticVariable );
	expectedLocations.emplace( &classInstance.mExampleSecondaryNonStaticVariable );
	ASSERT_EQ( expectedLocations.size(), 2 );
	for( TypeTraverser::MemberVariableInstance const& varInst : members )
	{
		void const* const varLoc = varInst.mMemberVariableLocation;
		ASSERT_TRUE( expectedLocations.count( varLoc ) == 1 );
		expectedLocations.erase( varLoc );
	}
	ASSERT_EQ( expectedLocations.size(), 0 );
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

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExamplePoorLifeDecision>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );
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

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
	};

	rftype_example::ExampleWithClassAsMember classInstance;
	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleWithClassAsMember>(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 1 );
	ASSERT_EQ( nested.size(), 1 );

	rftl::unordered_set<void const*> expectedLocations;
	expectedLocations.emplace( &classInstance.mExampleClassAsMember.mExampleNonStaticVariable );
	ASSERT_EQ( expectedLocations.size(), 1 );
	for( TypeTraverser::MemberVariableInstance const& varInst : members )
	{
		void const* const varLoc = varInst.mMemberVariableLocation;
		ASSERT_TRUE( expectedLocations.count( varLoc ) == 1 );
		expectedLocations.erase( varLoc );
	}
	ASSERT_EQ( expectedLocations.size(), 0 );
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

	rftl::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		//
	};

	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleWithClassAsMember>(),
		nullptr,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );
	ASSERT_EQ( members.size(), 1 );
	ASSERT_EQ( nested.size(), 1 );
}

///////////////////////////////////////////////////////////////////////////////
}}

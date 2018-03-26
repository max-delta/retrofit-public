#include "stdafx.h"

#include "RFType/TypeDatabase.h"
#include "RFType/TypeTraverser.h"
#include "core_rftype/ClassInfoAccessor.h"

#include "RFType/Example.h"

#include <unordered_set>


namespace RF { namespace rftype {
///////////////////////////////////////////////////////////////////////////////

TEST( RFType, TraverseSingleInheritance )
{
	std::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	std::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	rftype_example::ExampleDerivedClass classInstance;
	TypeTraverser::TraverseVariablesT(
		*classInstance.GetVirtualClassInfo(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );

	std::unordered_set<void const*> expectedLocations;
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



TEST( RFType, TraverseMultipleInheritance )
{
	std::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	std::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	rftype_example::ExamplePoorLifeDecision classInstance;
	TypeTraverser::TraverseVariablesT(
		*classInstance.GetVirtualClassInfo(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
	ASSERT_EQ( members.size(), 2 );
	ASSERT_EQ( nested.size(), 0 );

	std::unordered_set<void const*> expectedLocations;
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



TEST( RFType, TraverseNesting )
{
	std::vector<TypeTraverser::MemberVariableInstance> members;
	auto onMemberVariableFunc = [&members](
		TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	std::vector<TypeTraverser::MemberVariableInstance> nested;
	auto onNestedTypeFoundFunc = [&nested](
		TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		nested.emplace_back( varInst );
		shouldRecurse = true;
	};

	rftype_example::ExampleWithClassAsMember classInstance;
	TypeTraverser::TraverseVariablesT(
		GetClassInfo<rftype_example::ExampleWithClassAsMember>(),
		&classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc );
	ASSERT_EQ( members.size(), 1 );
	ASSERT_EQ( nested.size(), 1 );

	std::unordered_set<void const*> expectedLocations;
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

///////////////////////////////////////////////////////////////////////////////
}}

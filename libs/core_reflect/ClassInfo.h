#pragma once

#include "core_reflect/Value.h"
#include <vector>


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

struct ClassInfo;



struct BaseClassInfo
{
	// Inheritance is implementation-specifc, so must use compiler-generated
	//  functions to perform the hop, even though all major compilers are
	//  reasonably predictable at time of writing
	using FuncPtrGetBasePointerFromDerived = void* ( void* );
	FuncPtrGetBasePointerFromDerived mGetBasePointerFromDerived;

	ClassInfo* mBaseClassInfo;
};



struct FreeStandingVariableInfo
{
	void const* mAddress;
	bool mMutable;
	size_t mSize;
	Value::Type mValueType;
};



struct MemberVariableInfo
{
	ptrdiff_t mOffset;
	bool mMutable;
	size_t mSize;
	Value::Type mValueType;
};



struct ReturnInfo
{
	Value::Type mValueType;
};



struct ParameterInfo
{
	Value::Type mValueType;
};



struct FreeStandingFunctionInfo
{
	void* mAddress;
	ReturnInfo mReturn;
	std::vector<ParameterInfo> Parameters;
};



struct MemberFunctionInfo
{
	bool mRequiresConst;
	ReturnInfo mReturn;
	std::vector<ParameterInfo> Parameters;
};



struct ClassInfo
{
	// Has or inherits virtual functions
	bool mIsPolymorphic : 1;

	// Has or inherits pure virtual functions
	bool mIsAbstract : 1;

	// Construction capabilities
	bool mIsDefaultConstructible : 1;
	bool mIsCopyConstructible : 1;
	bool mIsMoveConstructible : 1;

	// Destruction capabilities
	bool mIsDestructible : 1;
	bool mHasVirtualDestructor : 1;

	// The minimum alignment required
	size_t mMinimumAlignment;

	std::vector<BaseClassInfo> mBaseTypes;

	std::vector<FreeStandingVariableInfo> mStaticVariables;
	std::vector<MemberVariableInfo> mNonStaticVariables;

	std::vector<FreeStandingFunctionInfo> mStaticFunctions;
	std::vector<MemberFunctionInfo> mNonStaticFunctions;
};

///////////////////////////////////////////////////////////////////////////////
}}

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
	using FuncPtrGetBasePointerFromDerived = void const* (*)( void const* );
	FuncPtrGetBasePointerFromDerived mGetBasePointerFromDerived;

	ClassInfo const* mBaseClassInfo;
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

	// May be null if not known
	ClassInfo const* mClassInfo;
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
	using BaseTypes = std::vector<BaseClassInfo>;
	using StaticVariables = std::vector<FreeStandingVariableInfo>;
	using NonStaticVariables = std::vector<MemberVariableInfo>;
	using StaticFunctions = std::vector<FreeStandingFunctionInfo>;
	using NonStaticFunctions = std::vector<MemberFunctionInfo>;

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

	BaseTypes mBaseTypes;

	StaticVariables mStaticVariables;
	NonStaticVariables mNonStaticVariables;

	StaticFunctions mStaticFunctions;
	NonStaticFunctions mNonStaticFunctions;
};

///////////////////////////////////////////////////////////////////////////////
}}

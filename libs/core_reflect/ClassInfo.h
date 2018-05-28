#pragma once

#include "core_reflect/Value.h"
#include "core/ConstructorOverload.h"
#include "rftl/vector"
#include "rftl/deque"
#include "rftl/extension/immutable_string.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

struct ClassInfo;
struct ExtensionAccessor;



struct VariableTypeInfo
{
	Value::Type mValueType = Value::Type::Invalid;

	// May be null if not known
	ClassInfo const* mClassInfo = nullptr;

	// May be null if not known
	ExtensionAccessor const* mAccessor = nullptr;
};



struct BaseClassInfo
{
	// Inheritance is implementation-specifc, so must use compiler-generated
	//  functions to perform the hop, even though all major compilers are
	//  reasonably predictable at time of writing
	using FuncPtrGetBasePointerFromDerived = void const* (*)( void const* );
	FuncPtrGetBasePointerFromDerived mGetBasePointerFromDerived = nullptr;

	ClassInfo const* mBaseClassInfo = nullptr;
};



struct FreeStandingVariableInfo
{
	char const* mIdentifier = nullptr;
	void const* mAddress = nullptr;
	bool mMutable = false;
	size_t mSize = 0;
	VariableTypeInfo mVariableTypeInfo = {};
};



struct MemberVariableInfo
{
	char const* mIdentifier = nullptr;
	ptrdiff_t mOffset = 0;
	bool mMutable = false;
	size_t mSize = 0;
	VariableTypeInfo mVariableTypeInfo = {};
};



struct ReturnInfo
{
	Value::Type mValueType = Value::Type::Invalid;
};



struct ParameterInfo
{
	Value::Type mValueType = Value::Type::Invalid;
};



struct FreeStandingFunctionInfo
{
	char const* mIdentifier = nullptr;
	void* mAddress = nullptr;
	ReturnInfo mReturn = {};
	rftl::vector<ParameterInfo> Parameters = {};
};



struct MemberFunctionInfo
{
	char const* mIdentifier = nullptr;
	bool mRequiresConst = false;
	ReturnInfo mReturn = {};
	rftl::vector<ParameterInfo> Parameters = {};
};



// A 'root' pointer is interpreted as some type of structure that contains
//  key-value pairs, which the extension accessor knows how to extract
struct ExtensionAccessor
{
	using RootInst = void*;
	using ClassInst = void*;
	using ExtenInst = void*;

	// Contents must remain stable during access, it is the responsibility of
	//  the accessor to negotiate with the accessed structure if need be
	// NOTE: Null is permitted, indicates the accessor doesn't care
	using FuncPtrBeginAccess = void( *)( RootInst root );
	using FuncPtrEndAccess = void( *)( RootInst root );
	FuncPtrBeginAccess mBeginAccess = nullptr;
	FuncPtrEndAccess mEndAccess = nullptr;

	// The number of variables, which will be treated as an indexed list of
	//  keys, and each key queried before accessing the target variables
	// NOTE: Null is forbidden
	using FuncPtrGetNumVariables = size_t( *)( RootInst root );
	FuncPtrGetNumVariables mGetNumVariables = nullptr;

	// Different variables may have different key types, so the accessor will
	//  need to be queried for each individual variable
	// NOTE: Null is forbidden
	using FuncPtrGetVariableInfoByIndex = VariableTypeInfo( *)( RootInst root, size_t index );
	FuncPtrGetVariableInfoByIndex mGetVariableKeyInfoByIndex = nullptr;

	// Access key using the type provided by the key info
	// NOTE: Calling an incorrect function will result in a null/invalid return
	// NOTE: Null is permitted, if the given key type can never exist
	using FuncPtrGetValueByIndex = Value( *)( RootInst root, size_t index );
	using FuncPtrGetClassByIndex = ClassInst( *)( RootInst root, size_t index );
	using FuncPtrGetExtenByIndex = ExtenInst( *)( RootInst root, size_t index );
	FuncPtrGetValueByIndex mGetVariableKeyAsValueByIndex = nullptr;
	FuncPtrGetClassByIndex mGetVariableKeyAsClassByIndex = nullptr;
	FuncPtrGetExtenByIndex mGetVariableKeyAsExtenByIndex = nullptr;

	// Different variables may have different target types, so the accessor
	//  will need to be queried for each individual variable
	// NOTE: Calling an incorrect function will result in a null/invalid return
	// NOTE: Null is permitted, if the given key type can never exist
	using FuncPtrGetVariableInfoByValue = VariableTypeInfo( *)( RootInst root, Value key );
	using FuncPtrGetVariableInfoByClass = VariableTypeInfo( *)( RootInst root, ClassInst key, ClassInfo const* info );
	using FuncPtrGetVariableInfoByExten = VariableTypeInfo( *)( RootInst root, ExtensionAccessor const* info );
	FuncPtrGetVariableInfoByValue mGetVariableTargetInfoByValue = nullptr;
	FuncPtrGetVariableInfoByClass mGetVariableTargetInfoByClass = nullptr;
	FuncPtrGetVariableInfoByExten mGetVariableTargetInfoByExten = nullptr;

	// Access target using the type provided by the target info
	// NOTE: Calling an incorrect function will result in a null/invalid return
	// NOTE: Null is permitted, if the given key type can never exist
	using FuncPtrGetValueByValue = Value( *)( RootInst root, Value key );
	using FuncPtrGetValueByClass = Value( *)( RootInst root, ClassInst key, ClassInfo const* info );
	using FuncPtrGetValueByExten = Value( *)( RootInst root, ExtenInst key, ExtensionAccessor const* info );
	using FuncPtrGetClassByValue = ClassInst( *)( RootInst root, Value key );
	using FuncPtrGetClassByClass = ClassInst( *)( RootInst root, ClassInst key, ClassInfo const* info );
	using FuncPtrGetClassByExten = ClassInst( *)( RootInst root, ExtenInst key, ExtensionAccessor const* info );
	using FuncPtrGetExtenByValue = ExtenInst( *)( RootInst root, Value key );
	using FuncPtrGetExtenByClass = ExtenInst( *)( RootInst root, ClassInst key, ClassInfo const* info );
	using FuncPtrGetExtenByExten = ExtenInst( *)( RootInst root, ExtenInst key, ExtensionAccessor const* info );
	FuncPtrGetValueByValue mGetVariableTargetAsValueByKeyAsValue = nullptr;
	FuncPtrGetValueByClass mGetVariableTargetAsValueByKeyAsClass = nullptr;
	FuncPtrGetValueByExten mGetVariableTargetAsValueByKeyAsExten = nullptr;
	FuncPtrGetClassByValue mGetVariableTargetAsClassByKeyAsValue = nullptr;
	FuncPtrGetClassByClass mGetVariableTargetAsClassByKeyAsClass = nullptr;
	FuncPtrGetClassByExten mGetVariableTargetAsClassByKeyAsExten = nullptr;
	FuncPtrGetExtenByValue mGetVariableTargetAsExtenByKeyAsValue = nullptr;
	FuncPtrGetExtenByClass mGetVariableTargetAsExtenByKeyAsClass = nullptr;
	FuncPtrGetExtenByExten mGetVariableTargetAsExtenByKeyAsExten = nullptr;
};



struct ClassInfo
{
	using BaseTypes = rftl::vector<BaseClassInfo>;
	using StaticVariables = rftl::vector<FreeStandingVariableInfo>;
	using NonStaticVariables = rftl::vector<MemberVariableInfo>;
	using StaticFunctions = rftl::vector<FreeStandingFunctionInfo>;
	using NonStaticFunctions = rftl::vector<MemberFunctionInfo>;

	// IMPORTANT: Growing this storage cannot be permitted to invalidate
	//  references to data within the storage. Check your local C++ standard
	//  before considering changes to this type!
	// NOTE: See VariableTypeInfo for an example reference
	using ExtensionStorage = rftl::deque<ExtensionAccessor>;
	using IdentifierStorage = rftl::deque<rftl::immutable_string>;

	// IMPORTANT: Be careful where ClassInfo's are allowed to construct! Subtle
	//  CRT-initialization bugs are rampant when crossing module boundaries.
	// NOTE: As a general rule, never construct a ClassInfo at namespace-scope,
	//  only at block-level. This means if you want static storage duration,
	//  you will need to use static local variables in block scope to be safe.
	ClassInfo() = delete;
	explicit ClassInfo( ExplicitDefaultConstruct )
	{
	}

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

	// Extension information not stored in the variable information structures,
	//  since it is a rare case, but requires a large amount of data if present
	ExtensionStorage mExtensionStorage;

	// Identifier not stored in the variable information structures, since it
	//  is re-used in several locations
	char const* StoreString( char const* string );
	IdentifierStorage mIdentifierStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}

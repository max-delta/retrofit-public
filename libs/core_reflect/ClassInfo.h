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
	using UntypedInst = void*;
	using UntypedConstInst = void const*;

	// Contents must remain stable during access, it is the responsibility of
	//  the accessor to negotiate with the accessed structure if need be
	// NOTE: Null is permitted, indicates the accessor doesn't care
	using FuncPtrBeginAccess = void( *)( RootInst root );
	using FuncPtrEndAccess = void( *)( RootInst root );
	FuncPtrBeginAccess mBeginAccess = nullptr;
	FuncPtrEndAccess mEndAccess = nullptr;

	// One or more users may be trying to mutate the contents, it is the
	//  responsibility of the accessor to negotiate with the accessed structure
	//  if this could lead to errors, or violated the stability gaurantees from
	//  the begin/end access calls
	// NOTE: Null is permitted, indicates the accessor doesn't care
	using FuncPtrBeginMutation = void( *)( RootInst root );
	using FuncPtrEndMutation = void( *)( RootInst root );
	FuncPtrBeginMutation mBeginMutation = nullptr;
	FuncPtrEndMutation mEndMutation = nullptr;

	// The number of variables, which will be treated as an indexed list of
	//  keys, and each key queried before accessing the target variables
	// NOTE: Null is forbidden
	using FuncPtrGetNumVariables = size_t( *)( RootInst root );
	FuncPtrGetNumVariables mGetNumVariables = nullptr;

	// Different variables may have different key types, so the accessor will
	//  need to be queried for info on a per-key basis
	// NOTE: Null is forbidden
	using FuncPtrGetVariableInfoByIndex = VariableTypeInfo( *)( RootInst root, size_t index );
	FuncPtrGetVariableInfoByIndex mGetVariableKeyInfoByIndex = nullptr;

	// Access key using the type provided by the key info
	// NOTE: Returns false on caller failure
	// NOTE: Null is forbidden
	using FuncPtrGetKeyByIndex = bool( *)( RootInst root, size_t index, UntypedConstInst& key, VariableTypeInfo& keyInfo );
	FuncPtrGetKeyByIndex mGetVariableKeyByIndex = nullptr;

	// Different variables may have different target types, so the accessor
	//  will need to be queried for info on a per-variable basis
	// NOTE: Null is forbidden
	using FuncPtrGetVariableInfoByKey = VariableTypeInfo( *)( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo );
	FuncPtrGetVariableInfoByKey mGetVariableTargetInfoByKey = nullptr;

	// Access target by key
	// NOTE: Null is forbidden
	using FuncPtrGetTargetByKey = bool( *)( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst& value, VariableTypeInfo& valueInfo );
	FuncPtrGetTargetByKey mGetVariableTargetByKey = nullptr;
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

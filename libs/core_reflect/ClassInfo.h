#pragma once

#include "core_reflect/Value.h"

#include "core/meta/ConstructorOverload.h"
#include "core/ptr/ptr_fwd.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/deque"
#include "rftl/optional"
#include "rftl/tuple"
#include "rftl/vector"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

struct VariableTypeInfo
{
	Value::Type mValueType = Value::Type::Invalid;

	// May be null if not known
	ClassInfo const* mClassInfo = nullptr;

	// May be null if not known
	ExtensionAccessor const* mAccessor = nullptr;
};



struct VirtualRootInfo
{
	// Inheritance is implementation-specifc, so must use compiler-generated
	//  functions to perform the hop, even though all major compilers are
	//  reasonably predictable at time of writing
	// NOTE: If null, then it should be assumed that no adjustment is needed
	using FuncPtrGetRootNonDestructingPointerFromCurrent = VirtualClassWithoutDestructor const* (*)( void const* );
	FuncPtrGetRootNonDestructingPointerFromCurrent mGetRootNonDestructingPointerFromCurrent = nullptr;
	using FuncPtrGetRootPointerFromCurrent = VirtualClass const* (*)( void const* );
	FuncPtrGetRootPointerFromCurrent mGetRootPointerFromCurrent = nullptr;

	bool mDerivesFromVirtualClassWithoutDestructor : 1;
	bool mDerivesFromVirtualClass : 1;
};



struct BaseClassInfo
{
	// Inheritance is implementation-specifc, so must use compiler-generated
	//  functions to perform the hop, even though all major compilers are
	//  reasonably predictable at time of writing
	// NOTE: If null, then it should be assumed that no adjustment is needed
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
	void const* mAddress = nullptr;
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



struct IndirectionInfo
{
	// For a memory lookup, such as a pointer
	// NOTE: It's possible that the instance type is known to be different
	//  from the pointer type, such as when the pointer type is to a virtual
	//  base class, and the instance is a derived class
	rftl::optional<VariableTypeInfo> mMemoryLookupPointerType = rftl::nullopt;
	rftl::optional<VariableTypeInfo> mMemoryLookupInstanceType = rftl::nullopt;

	// TODO: Other more exotic indirection types?
};



// A 'root' pointer is interpreted as some type of structure that contains
//  key-value pairs, which the extension accessor knows how to extract
struct ExtensionAccessor
{
	using RootInst = void*;
	using RootConstInst = void const*;
	using ClassInst = void*;
	using ExtenInst = void*;
	using UntypedInst = void*;
	using UntypedConstInst = void const*;

	// Contents must remain stable during access, it is the responsibility of
	//  the accessor to negotiate with the accessed structure if need be
	// NOTE: Null is permitted, indicates the accessor doesn't care
	using FuncPtrBeginAccess = void ( * )( RootConstInst root );
	using FuncPtrEndAccess = void ( * )( RootConstInst root );
	FuncPtrBeginAccess mBeginAccess = nullptr;
	FuncPtrEndAccess mEndAccess = nullptr;

	// One or more users may be trying to mutate the contents, it is the
	//  responsibility of the accessor to negotiate with the accessed structure
	//  if this could lead to errors, or violated the stability gaurantees from
	//  the begin/end access calls
	// NOTE: Null is permitted, indicates the accessor doesn't care
	using FuncPtrBeginMutation = void ( * )( RootInst root );
	using FuncPtrEndMutation = void ( * )( RootInst root );
	FuncPtrBeginMutation mBeginMutation = nullptr;
	FuncPtrEndMutation mEndMutation = nullptr;

	// Some extensions have a simplified key system such that the index is
	//  itself the key (ex: array, vector, deque, list)
	// NOTE: Normal index->key->value pattern is still always supported, but
	//  this direct index characteristic has value to serializers since thay
	//  may have a keyless storage option that is more efficient or practical
	// NOTE: Null indicates a lack of this characteristic
	using FuncPtrGetDirectKeyInfo = VariableTypeInfo ( * )( RootConstInst root );
	FuncPtrGetDirectKeyInfo mGetDirectKeyInfo = nullptr;

	// Some extensions have a simplified key system such that all keys share
	//  the same type information (ex: all non-variant STL containers)
	// NOTE: Normal index->key->value pattern is still always supported, but
	//  this shared type characteristic has value to serializers since thay
	//  may have a simpler storage option that is more efficient or practical
	// NOTE: Null indicates a lack of this characteristic
	using FuncPtrGetSharedKeyInfo = VariableTypeInfo ( * )( RootConstInst root );
	FuncPtrGetSharedKeyInfo mGetSharedKeyInfo = nullptr;

	// Some extensions have a simplified value system such that all values
	//  share the same type information (ex: all non-variant STL containers)
	// NOTE: Normal index->key->value pattern is still always supported, but
	//  this shared type characteristic has value to serializers since thay
	//  may have a simpler storage option that is more efficient or practical
	// NOTE: Null indicates a lack of this characteristic
	using FuncPtrGetSharedTargetInfo = VariableTypeInfo ( * )( RootConstInst root );
	FuncPtrGetSharedTargetInfo mGetSharedTargetInfo = nullptr;

	// The number of variables, which will be treated as an indexed list of
	//  keys, and each key queried before accessing the target variables
	// NOTE: Null is forbidden
	using FuncPtrGetNumVariables = size_t ( * )( RootConstInst root );
	FuncPtrGetNumVariables mGetNumVariables = nullptr;

	// Different variables may have different key types, so the accessor will
	//  need to be queried for info on a per-key basis
	// NOTE: Null is forbidden
	using FuncPtrGetKeyInfoByIndex = VariableTypeInfo ( * )( RootConstInst root, size_t index );
	FuncPtrGetKeyInfoByIndex mGetKeyInfoByIndex = nullptr;

	// Access key using the type provided by the key info
	// NOTE: Returns false on caller failure
	// NOTE: Null is forbidden
	using FuncPtrGetKeyByIndex = bool ( * )( RootConstInst root, size_t index, UntypedConstInst& key, VariableTypeInfo& keyInfo );
	FuncPtrGetKeyByIndex mGetKeyByIndex = nullptr;

	// Different variables may have different target types, so the accessor
	//  will need to be queried for info on a per-variable basis
	// NOTE: Null is forbidden
	using FuncPtrGetTargetInfoByKey = VariableTypeInfo ( * )( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo );
	FuncPtrGetTargetInfoByKey mGetTargetInfoByKey = nullptr;

	// Some variables may be an indirection, and the accessor may be aware of
	//  this and able to provide some information about the indirection
	// NOTE: Null indicates lack of knowledge of indirection
	using FuncPtrGetTargetIndirectionInfoByKey = rftl::optional<IndirectionInfo> ( * )( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo );
	FuncPtrGetTargetIndirectionInfoByKey mGetTargetIndirectionInfoByKey = nullptr;

	// Access target by key
	// NOTE: Null is forbidden
	using FuncPtrGetTargetByKey = bool ( * )( RootConstInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst& value, VariableTypeInfo& valueInfo );
	FuncPtrGetTargetByKey mGetTargetByKey = nullptr;

	// Access mutable target by key
	// NOTE: Null indicates lack of support for this operation
	using FuncPtrGetMutableTargetByKey = bool ( * )( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedInst& value, VariableTypeInfo& valueInfo );
	FuncPtrGetMutableTargetByKey mGetMutableTargetByKey = nullptr;

	// Insert target by key
	// NOTE: Returns false on caller failure
	// NOTE: Null indicates lack of support for that operation
	using FunctPtrInsertVariableDefault = bool ( * )( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo );
	using FunctPtrInsertVariableViaMove = bool ( * )( RootInst root, UntypedInst key, VariableTypeInfo const& keyInfo, UntypedInst value, VariableTypeInfo const& valueInfo );
	using FunctPtrInsertVariableViaCopy = bool ( * )( RootInst root, UntypedConstInst key, VariableTypeInfo const& keyInfo, UntypedConstInst value, VariableTypeInfo const& valueInfo );
	using FunctPtrInsertVariableViaUPtr = bool ( * )( RootInst root, UniquePtr<void>&& key, VariableTypeInfo const& keyInfo, UniquePtr<void>&& value, VariableTypeInfo const& valueInfo );
	FunctPtrInsertVariableDefault mInsertVariableDefault = nullptr;
	FunctPtrInsertVariableViaMove mInsertVariableViaMove = nullptr;
	FunctPtrInsertVariableViaCopy mInsertVariableViaCopy = nullptr;
	FunctPtrInsertVariableViaUPtr mInsertVariableViaUPtr = nullptr;
};



struct ClassInfo
{
	// IMPORTANT: Implicit copying of class infos is usually a sign of a bug,
	//  as a common pattern is to have only one instance of a given type's
	//  class info in the entire process, so that types can be strongly
	//  compared by their addresses, instead of weakly compared by their data
	RF_TODO_ANNOTATION( "Add an explicit clone helper? Is that needed?" );
	RF_NO_COPY( ClassInfo );

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
		: mIsPolymorphic{}
		, mIsAbstract{}
		, mIsDefaultConstructible{}
		, mIsCopyConstructible{}
		, mIsMoveConstructible{}
		, mIsDestructible{}
		, mHasVirtualDestructor{}
	{
		// In C++20, you can theoretically do 'bool field : 1 = false'
		static_assert( __cplusplus < 202002L, "Replace bitfield initializers" );
	}

	// Walk inheritance chain, optionally resolving a pointer upwards to the
	//  ancestor along the way
	bool IsSameOrDerivedFrom( ClassInfo const& ancestor ) const;
	void const* AttemptUpcastWalk( ClassInfo const& ancestor, void const* source ) const;

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
	size_t mMinimumAlignment = 0;

	// If present, a known virtual root is a special form of base class that is
	//  valuable to always be quickly reachable, as it can be used to access
	//  polymorphic ClassInfo via a virtual function, to figure out the true
	//  type of something
	VirtualRootInfo mVirtualRootInfo = {};

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

inline bool operator==( VariableTypeInfo const& lhs, VariableTypeInfo const& rhs )
{
	return rftl::tie( lhs.mValueType, lhs.mClassInfo, lhs.mAccessor ) == rftl::tie( rhs.mValueType, rhs.mClassInfo, rhs.mAccessor );
}
inline bool operator!=( VariableTypeInfo const& lhs, VariableTypeInfo const& rhs )
{
	return ( lhs == rhs ) == false;
}

///////////////////////////////////////////////////////////////////////////////
}

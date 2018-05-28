#pragma once
#include "project.h"

#include "core_reflect/Value.h"

#include "core/compiler.h"

#include "rftl/variant"
#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/string"

//
// Forwards
struct SQVM;


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////

class SCRIPTINGSQUIRREL_API SquirrelVM
{
	//
	// Forwards
public:
	struct ArrayTagType;
	struct InstanceTagType;


	//
	// Types
public:
	#if (RF_PLATFORM_POINTER_BYTES >= 8)
		using Integer = int64_t;
	#else
		using Integer = int32_t;
	#endif
	using FloatingPoint = float;
	using Boolean = bool;
	using Pointer = void*;
	using String = rftl::string;
	using ArrayTag = ArrayTagType const*;
	using InstanceTag = InstanceTagType const*;

	// WARNING: STL variant is flawed, and the first index has special meaning,
	//  so it's important that the first entry be the RF reflection value type,
	//  since it has internal logic that compensates for the issues with the
	//  standard C++ variant
	// NOTE: Observed effect to users is that a default-constructed element
	//  will contain an explicitly invalid RF reflection value
	using Element = rftl::variant<
		reflect::Value,
		String,
		ArrayTag,
		InstanceTag>;

	using ElementArray = rftl::vector<Element>;
	using ElementMap = rftl::unordered_map<Element, Element>;
private:
	using HSQUIRRELVM = SQVM * ;
	using ElementNameCharType = char;


	//
	// Public methods
public:
	SquirrelVM();
	~SquirrelVM();

	bool AddSourceFromBuffer( rftl::string const& buffer );

	bool InjectSimpleStruct( ElementNameCharType const* name, ElementNameCharType const* const* memberNames, size_t numMembers );

	Element GetGlobalVariable( rftl::string const& name );
	Element GetGlobalVariable( char const* name );
	ElementArray GetGlobalVariableAsArray( rftl::string const& name );
	ElementArray GetGlobalVariableAsArray( char const* name );
	ElementMap GetGlobalVariableAsInstance( rftl::string const& name );
	ElementMap GetGlobalVariableAsInstance( char const* name );


	//
	// Private methods
private:
	Element GetGlobalVariable( ElementNameCharType const* name, size_t nameLen );
	ElementArray GetGlobalVariableAsArray( ElementNameCharType const* name, size_t nameLen );
	ElementMap GetGlobalVariableAsInstance( ElementNameCharType const* name, size_t nameLen );


	//
	// Private data
private:
	HSQUIRRELVM m_Vm;
};

///////////////////////////////////////////////////////////////////////////////
}}

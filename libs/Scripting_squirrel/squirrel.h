#pragma once
#include "project.h"

#include "core_reflect/Value.h"

#include "core/compiler.h"

#include "rftl/variant"
#include "rftl/vector"
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
	struct ArrayTag;


	//
	// Types
public:
	#if (RF_PLATFORM_POINTER_BYTES >= 8)
		typedef int64_t Integer;
	#else
		typedef int32_t Integer;
	#endif
	typedef float FloatingPoint;
	typedef bool Boolean;
	typedef void* Pointer;
	typedef rftl::string String;

	typedef rftl::variant<
		reflect::Value,
		String,
		ArrayTag> Element;

	typedef rftl::vector<Element> ElementArray;
private:
	typedef SQVM* HSQUIRRELVM;
	typedef char ElementNameCharType;


	//
	// Structs
public:
	struct ArrayTag
	{
		//
	};


	//
	// Public methods
public:
	SquirrelVM();
	~SquirrelVM();

	bool AddSourceFromBuffer( rftl::string const& buffer );

	Element GetGlobalVariable( rftl::string const& name );
	Element GetGlobalVariable( char const* name );
	ElementArray GetGlobalVariableAsArray( rftl::string const& name );
	ElementArray GetGlobalVariableAsArray( char const* name );


	//
	// Private methods
private:
	Element GetGlobalVariable( ElementNameCharType const* name, size_t nameLen );
	ElementArray GetGlobalVariableAsArray( ElementNameCharType const* name, size_t nameLen );


	//
	// Private data
private:
	HSQUIRRELVM m_Vm;
};

///////////////////////////////////////////////////////////////////////////////
}}

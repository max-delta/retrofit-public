#pragma once
#include "project.h"

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
	typedef rftl::wstring String;
	typedef void* Pointer;
	typedef nullptr_t Null;
	typedef rftl::wstring ElementName;
	typedef rftl::variant<
		Integer,
		FloatingPoint,
		Boolean,
		String,
		Pointer,
		ArrayTag,
		Null> Element;
	typedef rftl::vector<Element> ElementArray;
private:
	typedef SQVM* HSQUIRRELVM;


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

	bool AddSourceFromBuffer( rftl::wstring const& buffer );

	Element GetGlobalVariable( ElementName const& name );
	ElementArray GetGlobalVariableAsArray( ElementName const& name );


	//
	// Private methods
private:


	//
	// Private data
private:
	HSQUIRRELVM m_Vm;
};

///////////////////////////////////////////////////////////////////////////////
}}

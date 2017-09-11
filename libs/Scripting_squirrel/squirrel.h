#pragma once
#include "project.h"

#include <variant>
#include <vector>

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
	// TODO: Move check to core_platform
	#if (defined(_WIN64) || defined(_LP64))
		typedef int64_t Integer;
	#else
		typedef int32_t Integer;
	#endif
	typedef float FloatingPoint;
	typedef bool Boolean;
	typedef std::wstring String;
	typedef void* Pointer;
	typedef nullptr_t Null;
	typedef std::wstring ElementName;
	typedef std::variant<
		Integer,
		FloatingPoint,
		Boolean,
		String,
		Pointer,
		ArrayTag,
		Null> Element;
	typedef std::vector<Element> ElementArray;
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

	bool AddSourceFromBuffer( std::wstring const& buffer );

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

#pragma once
#include "project.h"

#include "GameScripting/ScriptFwd.h"

#include "Scripting_squirrel/squirrel.h"

#include "core_reflect/ClassInfo.h"

#include "rftl/string_view"


namespace RF::script {
///////////////////////////////////////////////////////////////////////////////

// OO files are not able to contain executable logic that interacts with the
//  engine in any practical way. They may use isolated execution logic during
//  load to prepare their data, but this will all be stripped post-load.
class GAMESCRIPTING_API OOLoader
{
	RF_NO_COPY( OOLoader );

	//
	// Structs
public:
	struct InjectedClass
	{
		reflect::ClassInfo const* mClassInfo = nullptr;
		rftl::string mName;
	};
	using InjectedClasses = rftl::vector<InjectedClass>;


	//
	// Public methods
public:
	OOLoader();
	~OOLoader();

	// Only explicitly injected types can be loaded, other types will cause
	//  load errors when referenced
	template<typename ReflectedClass>
	bool InjectReflectedClassByCompileType( char const* name );
	bool InjectReflectedClassByClassInfo( reflect::ClassInfo const& classInfo, char const* name );

	// Source must be added before the loader can populate data
	bool AddSourceFromBuffer( rftl::string const& buffer );
	bool AddSourceFromBuffer( char const* buffer, size_t len );
	bool AddSourceFromBuffer( rftl::string_view buffer );

	// Reflected classes can be populated
	template<typename ReflectedClass>
	bool PopulateClass(
		char const* rootVariableName,
		ReflectedClass& classInstance );
	template<typename ReflectedClass>
	bool PopulateClass(
		SquirrelVM::NestedTraversalPath scriptPath,
		ReflectedClass& classInstance );
	bool PopulateClass(
		char const* rootVariableName,
		reflect::ClassInfo const& classInfo,
		void* classInstance );
	bool PopulateClass(
		SquirrelVM::NestedTraversalPath scriptPath,
		reflect::ClassInfo const& classInfo,
		void* classInstance );


	//
	// Private data
private:
	SquirrelVM mVm;

	// Used for being able to tie Squirrel instances back to ClassInfo's, by
	//  tracking what types have been injected for later lookup
	// NOTE: Can't just store the ClassInfo pointer into instances and blindly
	//  defereference them later, because malicious script could modify the
	//  pointers and attack the dereferencing code
	InjectedClasses mInjectedClasses;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "OOLoader.inl"

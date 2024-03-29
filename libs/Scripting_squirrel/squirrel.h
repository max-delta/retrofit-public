#pragma once
#include "project.h"

#include "Logging/Logging.h"
RF_TODO_ANNOTATION( "Provide a narrower include for just specializing contexts" );

#include "core_reflect/Value.h"

#include "core/macros.h"

#include "rftl/variant"
#include "rftl/vector"
#include "rftl/unordered_map"
#include "rftl/string"
#include "rftl/string_view"

//
// Forwards
struct SQVM;


namespace RF::script {
///////////////////////////////////////////////////////////////////////////////

class SCRIPTINGSQUIRREL_API SquirrelVM
{
	RF_NO_COPY( SquirrelVM );

	//
	// Forwards
public:
	struct NestedTraversalNode;
	struct TableTagType;
	struct ArrayTagType;
	struct InstanceTagType;


	//
	// Types and constants
public:
	#if( RF_PLATFORM_POINTER_BYTES >= 8 )
		using Integer = int64_t;
	#else
		using Integer = int32_t;
	#endif
	using FloatingPoint = float;
	using Boolean = bool;
	using Pointer = void*;
	using String = rftl::string;
	using TableTag = TableTagType const*;
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
		TableTag,
		ArrayTag,
		InstanceTag>;

	using ElementArray = rftl::vector<Element>;
	using ElementMap = rftl::unordered_map<Element, Element>;
	using NestedTraversalPath = rftl::vector<NestedTraversalNode>;

	// HACK: Special reserved member name
	static constexpr char kReservedClassNameMemberName[] = "__sqname";
private:
	using HSQUIRRELVM = SQVM*;
	using ElementNameCharType = char;


	//
	// Structs
public:
	struct SCRIPTINGSQUIRREL_API NestedTraversalNode
	{
		NestedTraversalNode() = default;
		NestedTraversalNode( rftl::string_view const& identifier );
		NestedTraversalNode( rftl::string const& identifier );
		NestedTraversalNode( char const* identifier );

		rftl::string mIdentifier;
	};

	struct VMStackGuard
	{
		RF_NO_COPY( VMStackGuard );
		VMStackGuard( HSQUIRRELVM vm );
		~VMStackGuard();

	protected:
		HSQUIRRELVM const mVm;
		Integer mOriginalStackLocation;
	};

private:
	struct VMRootStackGuard : public VMStackGuard
	{
		RF_NO_COPY( VMRootStackGuard );
		VMRootStackGuard( HSQUIRRELVM vm );
	};


	//
	// Public methods
public:
	SquirrelVM();
	~SquirrelVM();

	bool AddSourceFromBuffer( rftl::string const& buffer );
	bool AddSourceFromBuffer( char const* buffer, size_t len );

	bool InjectSimpleStruct( char const* name, char const* const* memberNames, size_t numMembers );

	Element GetGlobalVariable( rftl::string_view const& name );
	ElementArray GetGlobalVariableAsArray( rftl::string_view const& name );
	ElementMap GetGlobalVariableAsInstance( rftl::string_view const& name );
	rftl::string GetGlobalInstanceClassName( rftl::string_view const& name );

	Element GetNestedVariable( NestedTraversalPath const& path );
	ElementArray GetNestedVariableAsArray( NestedTraversalPath const& path );
	ElementMap GetNestedVariableAsInstance( NestedTraversalPath const& path );
	rftl::string GetNestedInstanceClassName( NestedTraversalPath const& path );


	//
	// Private methods
private:
	bool NoCleanup_GetNestedVariable( VMStackGuard const&, NestedTraversalPath const& path, Element& currentElement );


	//
	// Private data
private:
	HSQUIRRELVM mVm;
};

///////////////////////////////////////////////////////////////////////////////
}

namespace RF::logging {
template<>
void SCRIPTINGSQUIRREL_API WriteContextString( RF::script::SquirrelVM::NestedTraversalPath const& context, Utf8LogContextBuffer& buffer );
}

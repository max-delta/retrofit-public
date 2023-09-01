#include "stdafx.h"
#include "squirrel.h"

#include "Logging/Logging.h"

#include "core/macros.h"
#include "core_math/math_casts.h"

#include "squirrel3/include/squirrel.h"

#include "rftl/extension/string_copy.h"


template<>
void RF::logging::WriteContextString( RF::script::SquirrelVM::NestedTraversalPath const& context, Utf8LogContextBuffer& buffer )
{
	rftl::string out;
	for( RF::script::SquirrelVM::NestedTraversalNode const& node : context )
	{
		if( out.empty() == false )
		{
			out += "::";
		}
		out += node.mIdentifier;
	}
	rftl::string_copy( buffer, out );
}

namespace RF::script {
///////////////////////////////////////////////////////////////////////////////
namespace {

static rftl::string GetLastError( HSQUIRRELVM vm )
{
	SquirrelVM::VMStackGuard const stackGuard( vm );

	sq_getlasterror( vm );
	SQChar const* rawString;
	SQRESULT const result = sq_getstring( vm, -1, &rawString );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	rftl::string retVal( rawString );
	return retVal;
}



static void NotifyLastError( HSQUIRRELVM vm )
{
	rftl::string const error = GetLastError( vm );
	RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Squirrel error: %s", error.c_str() );
}



static void LogCompileError( HSQUIRRELVM vm, SQChar const* desc, SQChar const* source, SQInteger line, SQInteger column )
{
	RFLOG_ERROR( nullptr, RFCAT_SQUIRREL, "Compile error: \"%s\" at \"%s\", %lli, %lli", desc, source, line, column );
}



template<typename... Args>
static void AssertStackTypes( HSQUIRRELVM vm, SQInteger startDepth )
{
	// Termination case
}



template<typename... Args>
static void AssertStackTypes( HSQUIRRELVM vm, SQInteger startDepth, SQObjectType type, Args... deeperTypes )
{
	SQObjectType const stackType = sq_gettype( vm, startDepth );
	RF_ASSERT( stackType == type );
	AssertStackTypes( vm, startDepth - 1, deeperTypes... );
}



SquirrelVM::Element GetElementFromStack( HSQUIRRELVM vm, SQInteger depth )
{
	SquirrelVM::Element retVal;
	SQRESULT result;
	SQObjectType type;

	type = sq_gettype( vm, depth );
	switch( type )
	{
		case OT_NULL:
		{
			retVal = reflect::Value( static_cast<SquirrelVM::Pointer>( nullptr ) );
			break;
		}
		case OT_INTEGER:
		{
			SQInteger integer;
			result = sq_getinteger( vm, depth, &integer );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = reflect::Value( static_cast<SquirrelVM::Integer>( integer ) );
			break;
		}
		case OT_FLOAT:
		{
			SQFloat floating;
			result = sq_getfloat( vm, depth, &floating );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = reflect::Value( static_cast<SquirrelVM::FloatingPoint>( floating ) );
			break;
		}
		case OT_BOOL:
		{
			SQBool boolean;
			result = sq_getbool( vm, depth, &boolean );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = reflect::Value( static_cast<SquirrelVM::Boolean>( boolean ) );
			break;
		}
		case OT_STRING:
		{
			SQChar const* string;
			result = sq_getstring( vm, depth, &string );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = rftl::string( string );
			break;
		}
		case OT_USERPOINTER:
		{
			SQUserPointer ptr;
			result = sq_getuserpointer( vm, depth, &ptr );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = reflect::Value( static_cast<SquirrelVM::Pointer>( ptr ) );
			break;
		}
		case OT_TABLE:
		{
			retVal = SquirrelVM::TableTag();
			break;
		}
		case OT_ARRAY:
		{
			retVal = SquirrelVM::ArrayTag();
			break;
		}
		case OT_INSTANCE:
		{
			retVal = SquirrelVM::InstanceTag();
			break;
		}
		case OT_USERDATA:
		case OT_CLOSURE:
		case OT_NATIVECLOSURE:
		case OT_GENERATOR:
		case OT_THREAD:
		case OT_FUNCPROTO:
		case OT_CLASS:
		case OT_WEAKREF:
		case OT_OUTER:
		default:
		{
			RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Type not currently supported" );
			retVal = reflect::Value{};
			break;
		}
	}

	return retVal;
}



SquirrelVM::ElementArray GetElementArrayFromStack( HSQUIRRELVM vm )
{
	SquirrelVM::VMStackGuard const stackGuard( vm );

	SquirrelVM::ElementArray retVal;

	SQObjectType const arrayType = sq_gettype( vm, -1 );
	if( arrayType != OT_ARRAY )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Not an array" );
		return retVal;
	}

	// Null stack slot for start iterator
	sq_pushnull( vm );
	AssertStackTypes( vm, -1, OT_NULL, OT_ARRAY );
	while( SQ_SUCCEEDED( sq_next( vm, -2 ) ) )
	{
		SquirrelVM::Element const key = GetElementFromStack( vm, -2 );
		SquirrelVM::Element const value = GetElementFromStack( vm, -1 );

		retVal.emplace_back( rftl::move( value ) );

		sq_pop( vm, 2 );
	}

	return retVal;
}



SquirrelVM::ElementMap GetElementMapFromStack( HSQUIRRELVM vm )
{
	SquirrelVM::VMStackGuard const stackGuard( vm );

	SquirrelVM::ElementMap retVal;

	SQObjectType const instanceType = sq_gettype( vm, -1 );
	if( instanceType != OT_INSTANCE )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Not an instance" );
		return retVal;
	}

	// Determine the class
	SQRESULT const classResult = sq_getclass( vm, -1 );
	RF_ASSERT( SQ_SUCCEEDED( classResult ) );
	AssertStackTypes( vm, -1, OT_CLASS, OT_INSTANCE );

	// Iterate members
	sq_pushnull( vm );
	AssertStackTypes( vm, -1, OT_NULL, OT_CLASS, OT_INSTANCE );
	while( SQ_SUCCEEDED( sq_next( vm, -2 ) ) )
	{
		// Pop 'value', which is just the default value for new instances since
		//  we're iterating the class definition, not the actual instance
		sq_pop( vm, 1 );

		SquirrelVM::Element const key = GetElementFromStack( vm, -1 );

		// Pop key in a get operation against the instance
		// Stack: KEY, ITER, CLASS, INSTANCE
		SQRESULT const foundResult = sq_get( vm, -4 );
		RF_ASSERT( SQ_SUCCEEDED( foundResult ) );
		// Stack: VALUE, ITER, CLASS, INSTANCE
		SquirrelVM::Element const value = GetElementFromStack( vm, -1 );

		retVal[key] = value;

		sq_pop( vm, 1 );
	}

	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

SquirrelVM::NestedTraversalNode::NestedTraversalNode( rftl::string const& identifier )
	: mIdentifier( identifier )
{
	//
}



SquirrelVM::NestedTraversalNode::NestedTraversalNode( char const* identifier )
	: mIdentifier( identifier )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

SquirrelVM::VMStackGuard::VMStackGuard( HSQUIRRELVM vm )
	: mVm( vm )
{
	mOriginalStackLocation = sq_gettop( vm );
}



SquirrelVM::VMStackGuard::~VMStackGuard()
{
	sq_settop( mVm, mOriginalStackLocation );
}

///////////////////////////////////////////////////////////////////////////////

SquirrelVM::VMRootStackGuard::VMRootStackGuard( HSQUIRRELVM vm )
	: VMStackGuard( vm )
{
	RF_ASSERT( mOriginalStackLocation == 0 );
}

///////////////////////////////////////////////////////////////////////////////

SquirrelVM::SquirrelVM()
	: mVm( nullptr )
{
	mVm = sq_open( 1024 );
}



SquirrelVM::~SquirrelVM()
{
	sq_close( mVm );
}



bool SquirrelVM::AddSourceFromBuffer( rftl::string const& buffer )
{
	return AddSourceFromBuffer( buffer.c_str(), buffer.length() );
}



bool SquirrelVM::AddSourceFromBuffer( char const* buffer, size_t len )
{
	VMRootStackGuard const stackGuard( mVm );

	SQRESULT result;

	sq_setcompilererrorhandler( mVm, LogCompileError );
	result = sq_compilebuffer( mVm, buffer, math::integer_cast<SQInteger>( len ), "SOURCE", true );
	if( SQ_FAILED( result ) )
	{
		NotifyLastError( mVm );
		return false;
	}
	AssertStackTypes( mVm, -1, OT_CLOSURE );

	sq_pushroottable( mVm );
	result = sq_call( mVm, 1, false, true );
	if( SQ_FAILED( result ) )
	{
		NotifyLastError( mVm );
		return false;
	}

	return true;
}



bool SquirrelVM::InjectSimpleStruct( char const* name, char const* const* memberNames, size_t numMembers )
{
	VMRootStackGuard const stackGuard( mVm );

	SQRESULT result;

	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );

	// WARNING: Documentation is incorrect for sq_newslot(...), the stack is
	//  not read in the documented order
	sq_pushstring( mVm, name, math::integer_cast<SQInteger>( strlen( name ) ) );
	AssertStackTypes( mVm, -1, OT_STRING, OT_TABLE );

	// Create class
	result = sq_newclass( mVm, false );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	AssertStackTypes( mVm, -1, OT_CLASS, OT_STRING, OT_TABLE );

	// Add members
	if( numMembers > 0 )
	{
		RF_ASSERT( memberNames != nullptr );
		for( size_t i = 0; i < numMembers; i++ )
		{
			ElementNameCharType const* const memberName = memberNames[i];

			// Add member with null as the default value, and no attribute
			// NOTE: Using sq_rawnemmember(...) to prevent callbacks firing
			// WARNING: Documentation is incorrect for sq_rawnemmember(...),
			//  the stack behavior is not popped, and not read in the
			//  documented order
			sq_pushstring( mVm, memberName, math::integer_cast<SQInteger>( strlen( memberName ) ) );
			sq_pushnull( mVm );
			sq_pushnull( mVm );
			AssertStackTypes( mVm, -1, OT_NULL, OT_NULL, OT_STRING, OT_CLASS );
			result = sq_rawnewmember( mVm, -4, false );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			AssertStackTypes( mVm, -1, OT_NULL, OT_NULL, OT_STRING, OT_CLASS );
			sq_pop( mVm, 3 );
		}
	}

	// Store in root table
	AssertStackTypes( mVm, -1, OT_CLASS, OT_STRING, OT_TABLE );
	result = sq_newslot( mVm, -3, true );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	AssertStackTypes( mVm, -1, OT_TABLE );

	return true;
}



SquirrelVM::Element SquirrelVM::GetGlobalVariable( rftl::string const& name )
{
	return GetGlobalVariable( name.c_str(), name.length() );
}



SquirrelVM::Element SquirrelVM::GetGlobalVariable( char const* name )
{
	return GetGlobalVariable( name, strlen( name ) );
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( rftl::string const& name )
{
	return GetGlobalVariableAsArray( name.c_str(), name.length() );
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( char const* name )
{
	return GetGlobalVariableAsArray( name, strlen( name ) );
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( rftl::string const& name )
{
	return GetGlobalVariableAsInstance( name.c_str(), name.length() );
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( char const* name )
{
	return GetGlobalVariableAsInstance( name, strlen( name ) );
}



SquirrelVM::Element SquirrelVM::GetNestedVariable( NestedTraversalPath const& path )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );
	Element currentElement = GetElementFromStack( mVm, -1 );

	// Lookup and preserve stack
	bool const foundSuccess = NoCleanup_GetNestedVariable( stackGuard, path, currentElement );
	if( foundSuccess == false )
	{
		RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find variable" );
		return {};
	}

	return currentElement;
}



SquirrelVM::ElementArray SquirrelVM::GetNestedVariableAsArray( NestedTraversalPath const& path )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );
	Element currentElement = GetElementFromStack( mVm, -1 );

	// Lookup and preserve stack
	bool const foundSuccess = NoCleanup_GetNestedVariable( stackGuard, path, currentElement );
	if( foundSuccess == false )
	{
		RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find variable" );
		return {};
	}

	return GetElementArrayFromStack( mVm );
}



SquirrelVM::ElementMap SquirrelVM::GetNestedVariableAsInstance( NestedTraversalPath const& path )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );
	Element currentElement = GetElementFromStack( mVm, -1 );

	// Lookup and preserve stack
	bool const foundSuccess = NoCleanup_GetNestedVariable( stackGuard, path, currentElement );
	if( foundSuccess == false )
	{
		RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find variable" );
		return {};
	}

	return GetElementMapFromStack( mVm );
}

///////////////////////////////////////////////////////////////////////////////

SquirrelVM::Element SquirrelVM::GetGlobalVariable( ElementNameCharType const* name, size_t nameLen )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );

	// Lookup
	sq_pushstring( mVm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( mVm, -1, OT_STRING, OT_TABLE );
	SQRESULT const result = sq_get( mVm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	return GetElementFromStack( mVm, -1 );
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( ElementNameCharType const* name, size_t nameLen )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );

	// Lookup
	sq_pushstring( mVm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( mVm, -1, OT_STRING, OT_TABLE );
	SQRESULT const result = sq_get( mVm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	return GetElementArrayFromStack( mVm );
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( ElementNameCharType const* name, size_t nameLen )
{
	VMRootStackGuard const stackGuard( mVm );

	// Start from root
	sq_pushroottable( mVm );
	AssertStackTypes( mVm, -1, OT_TABLE );

	// Lookup
	sq_pushstring( mVm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( mVm, -1, OT_STRING, OT_TABLE );
	SQRESULT const result = sq_get( mVm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	return GetElementMapFromStack( mVm );
}



bool SquirrelVM::NoCleanup_GetNestedVariable( VMStackGuard const&, NestedTraversalPath const& path, Element& currentElement )
{
	// Walk path
	for( NestedTraversalNode const& node : path )
	{
		rftl::string const& identifier = node.mIdentifier;

		// Step in
		if( rftl::get_if<SquirrelVM::TableTag>( &currentElement ) != nullptr )
		{
			// Table
			sq_pushstring( mVm, identifier.c_str(), math::integer_cast<SQInteger>( identifier.length() ) );
			AssertStackTypes( mVm, -1, OT_STRING, OT_TABLE );
			SQRESULT const foundResult = sq_get( mVm, -2 );
			if( SQ_FAILED( foundResult ) )
			{
				RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find identifier '%s'", identifier.c_str() );
				return false;
			}
		}
		else if( rftl::get_if<SquirrelVM::InstanceTag>( &currentElement ) != nullptr )
		{
			// Instance
			sq_pushstring( mVm, identifier.c_str(), math::integer_cast<SQInteger>( identifier.length() ) );
			AssertStackTypes( mVm, -1, OT_STRING, OT_INSTANCE );
			SQRESULT const foundResult = sq_get( mVm, -2 );
			if( SQ_FAILED( foundResult ) )
			{
				RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find identifier '%s'", identifier.c_str() );
				return false;
			}
		}
		else if( rftl::get_if<SquirrelVM::ArrayTag>( &currentElement ) != nullptr )
		{
			// Array
			// NOTE: atoi(...) means non-numeric identifiers will be treated
			//  as 0, which may be surprising
			// TODO: Detect and alert on this case
			SQInteger const index = math::integer_cast<SQInteger>( atoi( identifier.c_str() ) );
			sq_pushinteger( mVm, index );
			AssertStackTypes( mVm, -1, OT_INTEGER, OT_ARRAY );
			SQRESULT const foundResult = sq_get( mVm, -2 );
			if( SQ_FAILED( foundResult ) )
			{
				RFLOG_NOTIFY( path, RFCAT_SQUIRREL, "Failed to find identifier '%s'", identifier.c_str() );
				return false;
			}
		}
		else
		{
			// Unsupported type, or user error?
			RF_TODO_BREAK();
		}

		// Fetch element
		currentElement = GetElementFromStack( mVm, -1 );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "squirrel.h"

#include "Logging/Logging.h"

#include "core/macros.h"
#include "core_math/math_casts.h"

#include "squirrel3/include/squirrel.h"


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////
namespace{

static rftl::string GetLastError( HSQUIRRELVM vm )
{
	SQInteger const top = sq_gettop( vm );

	sq_getlasterror( vm );
	SQChar const* rawString;
	SQRESULT const result = sq_getstring( vm, -1, &rawString );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	rftl::string retVal( rawString );
	sq_settop( vm, top );
	return retVal;
}



static void NotifyLastError( HSQUIRRELVM vm )
{
	rftl::string const error = GetLastError( vm );
	RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "%s", error.c_str() );
}



static void LogCompileError( HSQUIRRELVM vm, SQChar const* desc, SQChar const* source, SQInteger line, SQInteger column )
{
	RFLOG_ERROR( nullptr, RFCAT_SQUIRREL, "Compile error: \"%s\" at \"%s\", %lli, %lli", desc, source, line, column );
}



template <typename ...Args>
static void AssertStackTypes( HSQUIRRELVM vm, SQInteger startDepth )
{
	// Termination case
}



template <typename ...Args>
static void AssertStackTypes( HSQUIRRELVM vm, SQInteger startDepth, SQObjectType type, Args ...deeperTypes )
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
		case OT_TABLE:
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

}
///////////////////////////////////////////////////////////////////////////////

SquirrelVM::SquirrelVM()
	: m_Vm( nullptr )
{
	m_Vm = sq_open( 1024 );
}



SquirrelVM::~SquirrelVM()
{
	sq_close( m_Vm );
}



bool SquirrelVM::AddSourceFromBuffer( rftl::string const& buffer )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	SQRESULT result;

	sq_setcompilererrorhandler( m_Vm, LogCompileError );
	result = sq_compilebuffer( m_Vm, buffer.c_str(), math::integer_cast<SQInteger>( buffer.length() ), "SOURCE", true );
	if( SQ_FAILED( result ) )
	{
		NotifyLastError( m_Vm );
		return false;
	}
	AssertStackTypes( m_Vm, -1, OT_CLOSURE );

	sq_pushroottable( m_Vm );
	result = sq_call( m_Vm, 1, false, true );
	if( SQ_FAILED( result ) )
	{
		NotifyLastError( m_Vm );
		return false;
	}

	sq_settop( m_Vm, top );
	return true;
}



bool SquirrelVM::InjectSimpleStruct( ElementNameCharType const* name, ElementNameCharType const* const* memberNames, size_t numMembers )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	SQRESULT result;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	// WARNING: Documentation is incorrect for sq_newslot(...), the stack is
	//  not read in the documented order
	sq_pushstring( m_Vm, name, math::integer_cast<SQInteger>( strlen( name ) ) );
	AssertStackTypes( m_Vm, -1, OT_STRING, OT_TABLE );

	// Create class
	result = sq_newclass( m_Vm, false );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	AssertStackTypes( m_Vm, -1, OT_CLASS, OT_STRING, OT_TABLE );

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
			sq_pushstring( m_Vm, memberName, math::integer_cast<SQInteger>( strlen( memberName ) ) );
			sq_pushnull( m_Vm );
			sq_pushnull( m_Vm );
			AssertStackTypes( m_Vm, -1, OT_NULL, OT_NULL, OT_STRING, OT_CLASS );
			result = sq_rawnewmember( m_Vm, -4, false );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			AssertStackTypes( m_Vm, -1, OT_NULL, OT_NULL, OT_STRING, OT_CLASS );
			sq_pop( m_Vm, 3 );
		}
	}

	// Store in root table
	AssertStackTypes( m_Vm, -1, OT_CLASS, OT_STRING, OT_TABLE );
	result = sq_newslot( m_Vm, -3, true );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	sq_settop( m_Vm, top );
	return true;
}



SquirrelVM::Element SquirrelVM::GetGlobalVariable( rftl::string const & name )
{
	return GetGlobalVariable( name.c_str(), name.length() );
}



SquirrelVM::Element SquirrelVM::GetGlobalVariable( char const * name )
{
	return GetGlobalVariable( name, strlen( name ) );
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( rftl::string const & name )
{
	return GetGlobalVariableAsArray( name.c_str(), name.length() );
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( char const * name )
{
	return GetGlobalVariableAsArray( name, strlen( name ) );
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( rftl::string const & name )
{
	return GetGlobalVariableAsInstance( name.c_str(), name.length() );
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( char const * name )
{
	return GetGlobalVariableAsInstance( name, strlen( name ) );
}

///////////////////////////////////////////////////////////////////////////////

SquirrelVM::Element SquirrelVM::GetGlobalVariable( ElementNameCharType const * name, size_t nameLen )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	Element retVal;
	SQRESULT result;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	sq_pushstring( m_Vm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( m_Vm, -1, OT_STRING, OT_TABLE );

	result = sq_get( m_Vm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	retVal = GetElementFromStack( m_Vm, -1 );

	sq_settop( m_Vm, top );
	return retVal;
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( ElementNameCharType const * name, size_t nameLen )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	ElementArray retVal;
	SQRESULT result;
	SQObjectType type;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	sq_pushstring( m_Vm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( m_Vm, -1, OT_STRING, OT_TABLE );

	result = sq_get( m_Vm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	type = sq_gettype( m_Vm, -1 );
	if( type != OT_ARRAY )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Not an array" );
		sq_settop( m_Vm, top );
		return retVal;
	}

	// Null stack slot for start iterator
	sq_pushnull( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_NULL, OT_ARRAY );
	while( SQ_SUCCEEDED( sq_next( m_Vm, -2 ) ) )
	{
		Element const key = GetElementFromStack( m_Vm, -2 );
		Element const value = GetElementFromStack( m_Vm, -1 );

		retVal.emplace_back( rftl::move( value ) );

		sq_pop( m_Vm, 2 );
	}

	sq_settop( m_Vm, top );
	return retVal;
}



SquirrelVM::ElementMap SquirrelVM::GetGlobalVariableAsInstance( ElementNameCharType const * name, size_t nameLen )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	ElementMap retVal;
	SQRESULT result;
	SQObjectType type;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	sq_pushstring( m_Vm, name, math::integer_cast<SQInteger>( nameLen ) );
	AssertStackTypes( m_Vm, -1, OT_STRING, OT_TABLE );

	result = sq_get( m_Vm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	type = sq_gettype( m_Vm, -1 );
	if( type != OT_INSTANCE )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Not an instance" );
		sq_settop( m_Vm, top );
		return retVal;
	}

	// Determine the class
	result = sq_getclass( m_Vm, -1 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	AssertStackTypes( m_Vm, -1, OT_CLASS, OT_INSTANCE );

	// Iterate members
	sq_pushnull( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_NULL, OT_CLASS, OT_INSTANCE );
	while( SQ_SUCCEEDED( sq_next( m_Vm, -2 ) ) )
	{
		// Pop 'value', which is just the default value for new instances since
		//  we're iterating the class definition, not the actual instance
		sq_pop( m_Vm, 1 );

		Element const key = GetElementFromStack( m_Vm, -1 );

		// Pop key in a get operation against the instance
		// Stack: KEY, ITER, CLASS, INSTANCE
		result = sq_get( m_Vm, -4 );
		RF_ASSERT( SQ_SUCCEEDED( result ) );
		// Stack: VALUE, ITER, CLASS, INSTANCE
		Element const value = GetElementFromStack( m_Vm, -1 );

		retVal[key] = value;

		sq_pop( m_Vm, 1 );
	}

	sq_settop( m_Vm, top );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

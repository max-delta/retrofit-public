#include "stdafx.h"
#include "squirrel.h"

#include "Logging/Logging.h"

#include "core/macros.h"
#include "core_math/math_casts.h"

#include "squirrel3/include/squirrel.h"


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////
namespace{

static std::string GetLastError( HSQUIRRELVM vm )
{
	SQInteger const top = sq_gettop( vm );

	sq_getlasterror( vm );
	SQChar const* rawString;
	SQRESULT const result = sq_getstring( vm, -1, &rawString );
	RF_ASSERT( SQ_SUCCEEDED( result ) );
	std::wstring const wString( rawString );
	std::string retVal;
	for( wchar_t const& wch : wString )
	{
		retVal.push_back( math::integer_truncast<char>( wch ) );
	}

	sq_settop( vm, top );
	return retVal;
}



static void NotifyLastError( HSQUIRRELVM vm )
{
	std::string const error = GetLastError( vm );
	RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "%s", error.c_str() );
}



template <typename ...Args>
static void AssertStackTypes( HSQUIRRELVM vm, size_t startDepth )
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

	type = sq_gettype( vm, -1 );
	switch( type )
	{
		case OT_NULL:
		{
			retVal = nullptr;
			break;
		}
		case OT_INTEGER:
		{
			SQInteger integer;
			result = sq_getinteger( vm, -1, &integer );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = integer;
			break;
		}
		case OT_FLOAT:
		{
			SQFloat floating;
			result = sq_getfloat( vm, -1, &floating );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = floating;
			break;
		}
		case OT_BOOL:
		{
			SQBool boolean;
			result = sq_getbool( vm, -1, &boolean );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = (bool)boolean;
			break;
		}
		case OT_STRING:
		{
			SQChar const* string;
			result = sq_getstring( vm, -1, &string );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = std::wstring( string );
			break;
		}
		case OT_USERPOINTER:
		{
			SQUserPointer ptr;
			result = sq_getuserpointer( vm, -1, &ptr );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = ptr;
			break;
		}
		case OT_ARRAY:
		{
			retVal = SquirrelVM::ArrayTag();
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
		case OT_INSTANCE:
		case OT_WEAKREF:
		case OT_OUTER:
		default:
		{
			RFLOG_NOTIFY( nullptr, RFCAT_SQUIRREL, "Type not currently supported" );
			retVal = nullptr;
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



bool SquirrelVM::AddSourceFromBuffer( std::wstring const& buffer )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	SQRESULT result;

	result = sq_compilebuffer( m_Vm, buffer.c_str(), buffer.length(), L"SOURCE", true );
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



SquirrelVM::Element SquirrelVM::GetGlobalVariable( ElementName const & name )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	Element retVal;
	SQRESULT result;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE);

	sq_pushstring( m_Vm, name.c_str(), name.length() );
	AssertStackTypes( m_Vm, -1, OT_STRING, OT_TABLE );

	result = sq_get( m_Vm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	retVal = GetElementFromStack( m_Vm, -1 );

	sq_settop( m_Vm, top );
	return retVal;
}



SquirrelVM::ElementArray SquirrelVM::GetGlobalVariableAsArray( ElementName const & name )
{
	SQInteger const top = sq_gettop( m_Vm );
	RF_ASSERT( top == 0 );

	ElementArray retVal;
	SQRESULT result;
	SQObjectType type;

	sq_pushroottable( m_Vm );
	AssertStackTypes( m_Vm, -1, OT_TABLE );

	sq_pushstring( m_Vm, name.c_str(), name.length() );
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

		retVal.emplace_back( std::move( value ) );

		sq_pop( m_Vm, 2 );
	}

	sq_settop( m_Vm, top );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

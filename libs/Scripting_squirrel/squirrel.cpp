#include "stdafx.h"
#include "squirrel.h"

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
	RF_ASSERT_MSG( false, error.c_str() );
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
	SQObjectType type;

	result = sq_compilebuffer( m_Vm, buffer.c_str(), buffer.length(), L"SOURCE", true );
	if( SQ_FAILED( result ) )
	{
		NotifyLastError( m_Vm );
		return false;
	}

	type = sq_gettype( m_Vm, -1 );
	RF_ASSERT( type == OT_CLOSURE );

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

	SQRESULT result;
	SQObjectType type;

	sq_pushroottable( m_Vm );

	type = sq_gettype( m_Vm, -1 );
	RF_ASSERT( type == OT_TABLE );

	sq_pushstring( m_Vm, name.c_str(), name.length() );

	type = sq_gettype( m_Vm, -1 );
	RF_ASSERT( type == OT_STRING );
	type = sq_gettype( m_Vm, -2 );
	RF_ASSERT( type == OT_TABLE );

	result = sq_get( m_Vm, -2 );
	RF_ASSERT( SQ_SUCCEEDED( result ) );

	type = sq_gettype( m_Vm, -1 );

	Element retVal;
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
			result = sq_getinteger( m_Vm, -1, &integer );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = integer;
			break;
		}
		case OT_FLOAT:
		{
			SQFloat floating;
			result = sq_getfloat( m_Vm, -1, &floating );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = floating;
			break;
		}
		case OT_BOOL:
		{
			SQBool boolean;
			result = sq_getbool( m_Vm, -1, &boolean );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = (bool)boolean;
			break;
		}
		case OT_STRING:
		{
			SQChar const* string;
			result = sq_getstring( m_Vm, -1, &string );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = std::wstring( string );
			break;
		}
		case OT_USERPOINTER:
		{
			SQUserPointer ptr;
			result = sq_getuserpointer( m_Vm, -1, &ptr );
			RF_ASSERT( SQ_SUCCEEDED( result ) );
			retVal = ptr;
			break;
		}
		case OT_ARRAY:
		{
			retVal = ArrayTag();
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
			RF_ASSERT_MSG( false, "Type not currently supported" );
			retVal = nullptr;
			break;
		}
	}
	sq_settop( m_Vm, top );

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

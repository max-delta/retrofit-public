#include "stdafx.h"

#include "Value.h"
#include "core/macros.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

Value::Value()
	: m_InternalStorage( InvalidType() )
{
	//
}



Value::Type Value::GetStoredType() const
{
	size_t const index = m_InternalStorage.index();
	static_assert( static_cast<size_t>( Type::Invalid ) == std::variant_npos, "Standards violation?" );
	constexpr size_t kInvalidTypeHackIndex = std::variant_size<InternalStorage>::value - 1;
	if( index == kInvalidTypeHackIndex )
	{
		return Type::Invalid;
	}
	return static_cast<Type>( index );
}



void const * Value::GetBytes() const
{
	Type const type = GetStoredType();
	// TODO: Template recursion instead on the typelist
	switch( type )
	{
		case Type::Bool:		 return reinterpret_cast<void const*>( GetAs<bool>() );
		case Type::VoidPtr:		 return reinterpret_cast<void const*>( GetAs<void*>() );
		case Type::VoidConstPtr: return reinterpret_cast<void const*>( GetAs<void const*>() );
		case Type::VirtualClassPtr:		 return reinterpret_cast<VirtualClass const*>( GetAs<VirtualClass*>() );
		case Type::VirtualClassConstPtr: return reinterpret_cast<VirtualClass const*>( GetAs<VirtualClass const*>() );
		case Type::Char:		 return reinterpret_cast<void const*>( GetAs<char>() );
		case Type::WChar:		 return reinterpret_cast<void const*>( GetAs<wchar_t>() );
		case Type::Char16:		 return reinterpret_cast<void const*>( GetAs<char16_t>() );
		case Type::Char32:		 return reinterpret_cast<void const*>( GetAs<char32_t>() );
		case Type::Float:		 return reinterpret_cast<void const*>( GetAs<float>() );
		case Type::Double:		 return reinterpret_cast<void const*>( GetAs<double>() );
		case Type::LongDouble:	 return reinterpret_cast<void const*>( GetAs<long double>() );
		case Type::UInt8:		 return reinterpret_cast<void const*>( GetAs<uint8_t>() );
		case Type::Int8:		 return reinterpret_cast<void const*>( GetAs<int8_t>() );
		case Type::UInt16:		 return reinterpret_cast<void const*>( GetAs<uint16_t>() );
		case Type::Int16:		 return reinterpret_cast<void const*>( GetAs<int16_t>() );
		case Type::UInt32:		 return reinterpret_cast<void const*>( GetAs<uint32_t>() );
		case Type::Int32:		 return reinterpret_cast<void const*>( GetAs<int32_t>() );
		case Type::UInt64:		 return reinterpret_cast<void const*>( GetAs<uint64_t>() );
		case Type::Int64:		 return reinterpret_cast<void const*>( GetAs<int64_t>() );
		case Type::Invalid:		 return nullptr;
		default:
		{
			RF_DBGFAIL();
			return nullptr;
		}
	}
}



size_t Value::GetNumBytes() const
{
	Type const type = GetStoredType();
	// TODO: Template recursion instead on the typelist
	switch( type )
	{
		case Type::Bool:		 return sizeof( bool );
		case Type::VoidPtr:		 return sizeof( void* );
		case Type::VoidConstPtr: return sizeof( void const* );
		case Type::VirtualClassPtr:		 return sizeof( VirtualClass* );
		case Type::VirtualClassConstPtr: return sizeof( VirtualClass const* );
		case Type::Char:		 return sizeof( char );
		case Type::WChar:		 return sizeof( wchar_t );
		case Type::Char16:		 return sizeof( char16_t );
		case Type::Char32:		 return sizeof( char32_t );
		case Type::Float:		 return sizeof( float );
		case Type::Double:		 return sizeof( double );
		case Type::LongDouble:	 return sizeof( long double );
		case Type::UInt8:		 return sizeof( uint8_t );
		case Type::Int8:		 return sizeof( int8_t );
		case Type::UInt16:		 return sizeof( uint16_t );
		case Type::Int16:		 return sizeof( int16_t );
		case Type::UInt32:		 return sizeof( uint32_t );
		case Type::Int32:		 return sizeof( int32_t );
		case Type::UInt64:		 return sizeof( uint64_t );
		case Type::Int64:		 return sizeof( int64_t );
		case Type::Invalid:		 return 0;
		default:
		{
			RF_DBGFAIL();
			return 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

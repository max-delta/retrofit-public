#include "stdafx.h"

#include "Value.h"
#include "core/macros.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

Value::Value()
	: mInternalStorage( InvalidTag() )
{
	//
}



Value::Value( Type type, void const * bytes )
{
	// TODO: Template recursion instead on the typelist
	switch( type )
	{
		case Type::Bool:				mInternalStorage = *reinterpret_cast<bool const*>( bytes ); break;
		case Type::VoidPtr:				mInternalStorage = *reinterpret_cast<void* const*>( bytes ); break;
		case Type::VoidConstPtr:		mInternalStorage = *reinterpret_cast<void const* const*>( bytes ); break;
		case Type::VirtualClassPtr:		mInternalStorage = *reinterpret_cast<VirtualClass* const*>( bytes ); break;
		case Type::VirtualClassConstPtr:mInternalStorage = *reinterpret_cast<VirtualClass const* const*>( bytes ); break;
		case Type::Char:				mInternalStorage = *reinterpret_cast<char const*>( bytes ); break;
		case Type::WChar:				mInternalStorage = *reinterpret_cast<wchar_t const*>( bytes ); break;
		case Type::Char16:				mInternalStorage = *reinterpret_cast<char16_t const*>( bytes ); break;
		case Type::Char32:				mInternalStorage = *reinterpret_cast<char32_t const*>( bytes ); break;
		case Type::Float:				mInternalStorage = *reinterpret_cast<float const*>( bytes ); break;
		case Type::Double:				mInternalStorage = *reinterpret_cast<double const*>( bytes ); break;
		case Type::LongDouble:			mInternalStorage = *reinterpret_cast<long double const*>( bytes ); break;
		case Type::UInt8:				mInternalStorage = *reinterpret_cast<uint8_t const*>( bytes ); break;
		case Type::UInt16:				mInternalStorage = *reinterpret_cast<uint16_t const*>( bytes ); break;
		case Type::UInt32:				mInternalStorage = *reinterpret_cast<uint32_t const*>( bytes ); break;
		case Type::UInt64:				mInternalStorage = *reinterpret_cast<uint64_t const*>( bytes ); break;
		case Type::Int8:				mInternalStorage = *reinterpret_cast<int8_t const*>( bytes ); break;
		case Type::Int16:				mInternalStorage = *reinterpret_cast<int16_t const*>( bytes ); break;
		case Type::Int32:				mInternalStorage = *reinterpret_cast<int32_t const*>( bytes ); break;
		case Type::Int64:				mInternalStorage = *reinterpret_cast<int64_t const*>( bytes ); break;
		case Type::Invalid:				mInternalStorage = InvalidTag(); break;
		default:
		{
			RF_DBGFAIL();
			mInternalStorage = InvalidTag();
			break;
		}
	}
}



Value::Type Value::GetStoredType() const
{
	size_t const index = mInternalStorage.index();
	static_assert( static_cast<size_t>( Type::Invalid ) == rftl::variant_npos, "Standards violation?" );
	constexpr size_t kInvalidTypeHackIndex = rftl::variant_size<InternalStorage>::value - 1;
	if( index == kInvalidTypeHackIndex )
	{
		return Type::Invalid;
	}
	return static_cast<Type>( index );
}



char const* Value::GetStoredTypeName() const
{
	Type const type = GetStoredType();
	size_t const typeAsIndex = static_cast<size_t>( type );
	return kTypeNameStrings[typeAsIndex];
}



char const* Value::GetTypeName( Type type )
{
	size_t const typeAsIndex = static_cast<size_t>( type );
	return kTypeNameStrings[typeAsIndex];
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

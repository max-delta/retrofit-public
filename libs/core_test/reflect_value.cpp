#include "stdafx.h"

#include "core_reflect/Value.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

TEST( Reflect, ValueInvalid )
{
	Value vInvalid;
	ASSERT_TRUE( vInvalid.GetStoredType() == Value::Type::Invalid );
	ASSERT_TRUE( vInvalid.GetAs<bool>() == nullptr );
	ASSERT_TRUE( vInvalid.GetBytes() == nullptr );
	ASSERT_TRUE( vInvalid.GetNumBytes() == 0 );
}



TEST( Reflect, ValueBasics )
{
	Value vBool( static_cast<bool>( false ) );
	Value vVoidPtr( static_cast<void*>( nullptr ) );
	Value vVoidConstPtr( static_cast<void const*>( nullptr ) );
	Value vChar( static_cast<char>( '\0' ) );
	Value vWChar( static_cast<wchar_t>( L'\0' ) );
	Value vChar16( static_cast<char16_t>( u'\0' ) );
	Value vChar32( static_cast<char32_t>( U'\0' ) );
	Value vFloat( static_cast<float>( 0.f ) );
	Value vDouble( static_cast<double>( 0.0 ) );
	Value vLongDouble( static_cast<long double>( 0.0l ) );
	Value vUInt8( static_cast<uint8_t>( 0u ) );
	Value vInt8( static_cast<int8_t>( 0 ) );
	Value vUInt16( static_cast<uint16_t>( 0u ) );
	Value vInt16( static_cast<int16_t>( 0 ) );
	Value vUInt32( static_cast<uint32_t>( 0ul ) );
	Value vInt32( static_cast<int32_t>( 0l ) );
	Value vUInt64( static_cast<uint64_t>( 0ull ) );
	Value vInt64( static_cast<int64_t>( 0ll ) );

	static_assert( Value::DetermineType<bool>() == Value::Type::Bool );
	static_assert( Value::DetermineType<void*>() == Value::Type::VoidPtr );
	static_assert( Value::DetermineType<void const*>() == Value::Type::VoidConstPtr );
	static_assert( Value::DetermineType<char>() == Value::Type::Char );
	static_assert( Value::DetermineType<wchar_t>() == Value::Type::WChar );
	static_assert( Value::DetermineType<char16_t>() == Value::Type::Char16 );
	static_assert( Value::DetermineType<char32_t>() == Value::Type::Char32 );
	static_assert( Value::DetermineType<float>() == Value::Type::Float );
	static_assert( Value::DetermineType<double>() == Value::Type::Double );
	static_assert( Value::DetermineType<long double>() == Value::Type::LongDouble );
	static_assert( Value::DetermineType<uint8_t>() == Value::Type::UInt8 );
	static_assert( Value::DetermineType<int8_t>() == Value::Type::Int8 );
	static_assert( Value::DetermineType<uint16_t>() == Value::Type::UInt16 );
	static_assert( Value::DetermineType<int16_t>() == Value::Type::Int16 );
	static_assert( Value::DetermineType<uint32_t>() == Value::Type::UInt32 );
	static_assert( Value::DetermineType<int32_t>() == Value::Type::Int32 );
	static_assert( Value::DetermineType<uint64_t>() == Value::Type::UInt64 );
	static_assert( Value::DetermineType<int64_t>() == Value::Type::Int64 );

	ASSERT_TRUE( vBool.GetStoredType() == Value::Type::Bool );
	ASSERT_TRUE( vVoidPtr.GetStoredType() == Value::Type::VoidPtr );
	ASSERT_TRUE( vVoidConstPtr.GetStoredType() == Value::Type::VoidConstPtr );
	ASSERT_TRUE( vChar.GetStoredType() == Value::Type::Char );
	ASSERT_TRUE( vWChar.GetStoredType() == Value::Type::WChar );
	ASSERT_TRUE( vChar16.GetStoredType() == Value::Type::Char16 );
	ASSERT_TRUE( vChar32.GetStoredType() == Value::Type::Char32 );
	ASSERT_TRUE( vFloat.GetStoredType() == Value::Type::Float );
	ASSERT_TRUE( vDouble.GetStoredType() == Value::Type::Double );
	ASSERT_TRUE( vLongDouble.GetStoredType() == Value::Type::LongDouble );
	ASSERT_TRUE( vUInt8.GetStoredType() == Value::Type::UInt8 );
	ASSERT_TRUE( vInt8.GetStoredType() == Value::Type::Int8 );
	ASSERT_TRUE( vUInt16.GetStoredType() == Value::Type::UInt16 );
	ASSERT_TRUE( vInt16.GetStoredType() == Value::Type::Int16 );
	ASSERT_TRUE( vUInt32.GetStoredType() == Value::Type::UInt32 );
	ASSERT_TRUE( vInt32.GetStoredType() == Value::Type::Int32 );
	ASSERT_TRUE( vUInt64.GetStoredType() == Value::Type::UInt64 );
	ASSERT_TRUE( vInt64.GetStoredType() == Value::Type::Int64 );

	ASSERT_TRUE( vBool.GetAs<bool>() != nullptr );
	ASSERT_TRUE( vVoidPtr.GetAs<void*>() != nullptr );
	ASSERT_TRUE( vVoidConstPtr.GetAs<void const*>() != nullptr );
	ASSERT_TRUE( vChar.GetAs<char>() != nullptr );
	ASSERT_TRUE( vWChar.GetAs<wchar_t>() != nullptr );
	ASSERT_TRUE( vChar16.GetAs<char16_t>() != nullptr );
	ASSERT_TRUE( vChar32.GetAs<char32_t>() != nullptr );
	ASSERT_TRUE( vFloat.GetAs<float>() != nullptr );
	ASSERT_TRUE( vDouble.GetAs<double>() != nullptr );
	ASSERT_TRUE( vLongDouble.GetAs<long double>() != nullptr );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t>() != nullptr );
	ASSERT_TRUE( vInt8.GetAs<int8_t>() != nullptr );
	ASSERT_TRUE( vUInt16.GetAs<uint16_t>() != nullptr );
	ASSERT_TRUE( vInt16.GetAs<int16_t>() != nullptr );
	ASSERT_TRUE( vUInt32.GetAs<uint32_t>() != nullptr );
	ASSERT_TRUE( vInt32.GetAs<int32_t>() != nullptr );
	ASSERT_TRUE( vUInt64.GetAs<uint64_t>() != nullptr );
	ASSERT_TRUE( vInt64.GetAs<int64_t>() != nullptr );

	ASSERT_TRUE( vBool.GetAs<bool const>() != nullptr );
	ASSERT_TRUE( vVoidPtr.GetAs<void* const>() != nullptr );
	ASSERT_TRUE( vVoidConstPtr.GetAs<void const* const>() != nullptr );
	ASSERT_TRUE( vChar.GetAs<char const>() != nullptr );
	ASSERT_TRUE( vWChar.GetAs<wchar_t const>() != nullptr );
	ASSERT_TRUE( vChar16.GetAs<char16_t const>() != nullptr );
	ASSERT_TRUE( vChar32.GetAs<char32_t const>() != nullptr );
	ASSERT_TRUE( vFloat.GetAs<float const>() != nullptr );
	ASSERT_TRUE( vDouble.GetAs<double const>() != nullptr );
	ASSERT_TRUE( vLongDouble.GetAs<long double const>() != nullptr );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t const>() != nullptr );
	ASSERT_TRUE( vInt8.GetAs<int8_t const>() != nullptr );
	ASSERT_TRUE( vUInt16.GetAs<uint16_t const>() != nullptr );
	ASSERT_TRUE( vInt16.GetAs<int16_t const>() != nullptr );
	ASSERT_TRUE( vUInt32.GetAs<uint32_t const>() != nullptr );
	ASSERT_TRUE( vInt32.GetAs<int32_t const>() != nullptr );
	ASSERT_TRUE( vUInt64.GetAs<uint64_t const>() != nullptr );
	ASSERT_TRUE( vInt64.GetAs<int64_t const>() != nullptr );

	ASSERT_TRUE( vBool.GetAs<bool>() == vBool.GetBytes() );
	ASSERT_TRUE( vVoidPtr.GetAs<void*>() == vVoidPtr.GetBytes() );
	ASSERT_TRUE( vVoidConstPtr.GetAs<void const*>() == vVoidConstPtr.GetBytes() );
	ASSERT_TRUE( vChar.GetAs<char>() == vChar.GetBytes() );
	ASSERT_TRUE( vWChar.GetAs<wchar_t>() == vWChar.GetBytes() );
	ASSERT_TRUE( vChar16.GetAs<char16_t>() == vChar16.GetBytes() );
	ASSERT_TRUE( vChar32.GetAs<char32_t>() == vChar32.GetBytes() );
	ASSERT_TRUE( vFloat.GetAs<float>() == vFloat.GetBytes() );
	ASSERT_TRUE( vDouble.GetAs<double>() == vDouble.GetBytes() );
	ASSERT_TRUE( vLongDouble.GetAs<long double>() == vLongDouble.GetBytes() );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t>() == vUInt8.GetBytes() );
	ASSERT_TRUE( vInt8.GetAs<int8_t>() == vInt8.GetBytes() );
	ASSERT_TRUE( vUInt16.GetAs<uint16_t>() == vUInt16.GetBytes() );
	ASSERT_TRUE( vInt16.GetAs<int16_t>() == vInt16.GetBytes() );
	ASSERT_TRUE( vUInt32.GetAs<uint32_t>() == vUInt32.GetBytes() );
	ASSERT_TRUE( vInt32.GetAs<int32_t>() == vInt32.GetBytes() );
	ASSERT_TRUE( vUInt64.GetAs<uint64_t>() == vUInt64.GetBytes() );
	ASSERT_TRUE( vInt64.GetAs<int64_t>() == vInt64.GetBytes() );

	ASSERT_TRUE( vBool.GetNumBytes() == sizeof( bool ) );
	ASSERT_TRUE( vVoidPtr.GetNumBytes() == sizeof( void* ) );
	ASSERT_TRUE( vVoidConstPtr.GetNumBytes() == sizeof( void const* ) );
	ASSERT_TRUE( vChar.GetNumBytes() == sizeof( char ) );
	ASSERT_TRUE( vWChar.GetNumBytes() == sizeof( wchar_t ) );
	ASSERT_TRUE( vChar16.GetNumBytes() == sizeof( char16_t ) );
	ASSERT_TRUE( vChar32.GetNumBytes() == sizeof( char32_t ) );
	ASSERT_TRUE( vFloat.GetNumBytes() == sizeof( float ) );
	ASSERT_TRUE( vDouble.GetNumBytes() == sizeof( double ) );
	ASSERT_TRUE( vLongDouble.GetNumBytes() == sizeof( long double ) );
	ASSERT_TRUE( vUInt8.GetNumBytes() == sizeof( uint8_t ) );
	ASSERT_TRUE( vInt8.GetNumBytes() == sizeof( int8_t ) );
	ASSERT_TRUE( vUInt16.GetNumBytes() == sizeof( uint16_t ) );
	ASSERT_TRUE( vInt16.GetNumBytes() == sizeof( int16_t ) );
	ASSERT_TRUE( vUInt32.GetNumBytes() == sizeof( uint32_t ) );
	ASSERT_TRUE( vInt32.GetNumBytes() == sizeof( int32_t ) );
	ASSERT_TRUE( vUInt64.GetNumBytes() == sizeof( uint64_t ) );
	ASSERT_TRUE( vInt64.GetNumBytes() == sizeof( int64_t ) );
}



TEST( Reflect, ValueMismatch )
{
	Value vUInt8( static_cast<uint8_t>( 0u ) );
	Value vInt8( static_cast<int8_t>( 0 ) );

	ASSERT_TRUE( vUInt8.GetAs<int8_t>() == nullptr );
	ASSERT_TRUE( vInt8.GetAs<uint8_t>() == nullptr );

	ASSERT_TRUE( vUInt8.GetAs<int8_t const>() == nullptr );
	ASSERT_TRUE( vInt8.GetAs<uint8_t const>() == nullptr );
}



TEST( Reflect, ValueCopy )
{
	Value vInvalid;
	Value vUInt8( static_cast<uint8_t>( 1u ) );
	Value vInt8( static_cast<int8_t>( 2 ) );

	ASSERT_TRUE( vInvalid.GetAs<uint8_t>() == nullptr );
	ASSERT_TRUE( vInvalid.GetAs<int8_t>() == nullptr );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t>() != nullptr );
	ASSERT_TRUE( vInt8.GetAs<int8_t>() != nullptr );
	ASSERT_TRUE( *vUInt8.GetAs<uint8_t>() == 1u );
	ASSERT_TRUE( *vInt8.GetAs<int8_t>() == 2 );

	vInvalid = vUInt8;
	ASSERT_TRUE( vInvalid.GetAs<uint8_t>() != nullptr );
	ASSERT_TRUE( vInvalid.GetAs<int8_t>() == nullptr );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t>() != nullptr );
	ASSERT_TRUE( vInt8.GetAs<int8_t>() != nullptr );
	ASSERT_TRUE( *vUInt8.GetAs<uint8_t>() == 1u );
	ASSERT_TRUE( *vInvalid.GetAs<uint8_t>() == 1u );

	vInvalid = vInt8;
	ASSERT_TRUE( vInvalid.GetAs<uint8_t>() == nullptr );
	ASSERT_TRUE( vInvalid.GetAs<int8_t>() != nullptr );
	ASSERT_TRUE( vUInt8.GetAs<uint8_t>() != nullptr );
	ASSERT_TRUE( vInt8.GetAs<int8_t>() != nullptr );
	ASSERT_TRUE( *vInt8.GetAs<int8_t>() == 2 );
	ASSERT_TRUE( *vInvalid.GetAs<int8_t>() == 2 );
}

///////////////////////////////////////////////////////////////////////////////
}}

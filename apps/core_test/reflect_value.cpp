#include "stdafx.h"

#include "core_reflect/Value.h"

#include "core_math/math_compare.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

TEST( Reflect, ValueInvalid )
{
	Value const vInvalid;
	ASSERT_TRUE( vInvalid.GetStoredType() == Value::Type::Invalid );
	ASSERT_TRUE( vInvalid.GetAs<bool>() == nullptr );
	ASSERT_TRUE( vInvalid.GetBytes() == nullptr );
	ASSERT_TRUE( vInvalid.GetNumBytes() == 0 );
}



TEST( Reflect, ValueBasics )
{
	Value const vBool( static_cast<bool>( false ) );
	Value const vVoidPtr( static_cast<void*>( nullptr ) );
	Value const vVoidConstPtr( static_cast<void const*>( nullptr ) );
	Value const vVirtualClassPtr( static_cast<VirtualClass*>( nullptr ) );
	Value const vVirtualClassConstPtr( static_cast<VirtualClass const*>( nullptr ) );
	Value const vChar( static_cast<char>( '\0' ) );
	Value const vWChar( static_cast<wchar_t>( L'\0' ) );
	Value const vChar16( static_cast<char16_t>( u'\0' ) );
	Value const vChar32( static_cast<char32_t>( U'\0' ) );
	Value const vFloat( static_cast<float>( 0.f ) );
	Value const vDouble( static_cast<double>( 0.0 ) );
	Value const vLongDouble( static_cast<long double>( 0.0l ) );
	Value const vUInt8( static_cast<uint8_t>( 0u ) );
	Value const vInt8( static_cast<int8_t>( 0 ) );
	Value const vUInt16( static_cast<uint16_t>( 0u ) );
	Value const vInt16( static_cast<int16_t>( 0 ) );
	Value const vUInt32( static_cast<uint32_t>( 0ul ) );
	Value const vInt32( static_cast<int32_t>( 0l ) );
	Value const vUInt64( static_cast<uint64_t>( 0ull ) );
	Value const vInt64( static_cast<int64_t>( 0ll ) );

	static_assert( Value::DetermineType<bool>() == Value::Type::Bool );
	static_assert( Value::DetermineType<void*>() == Value::Type::VoidPtr );
	static_assert( Value::DetermineType<void const*>() == Value::Type::VoidConstPtr );
	static_assert( Value::DetermineType<VirtualClass*>() == Value::Type::VirtualClassPtr );
	static_assert( Value::DetermineType<VirtualClass const*>() == Value::Type::VirtualClassConstPtr );
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
	ASSERT_TRUE( vVirtualClassPtr.GetStoredType() == Value::Type::VirtualClassPtr );
	ASSERT_TRUE( vVirtualClassConstPtr.GetStoredType() == Value::Type::VirtualClassConstPtr );
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
	ASSERT_TRUE( vVirtualClassPtr.GetAs<VirtualClass*>() != nullptr );
	ASSERT_TRUE( vVirtualClassConstPtr.GetAs<VirtualClass const*>() != nullptr );
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
	ASSERT_TRUE( vVirtualClassPtr.GetAs<VirtualClass* const>() != nullptr );
	ASSERT_TRUE( vVirtualClassConstPtr.GetAs<VirtualClass const* const>() != nullptr );
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
	ASSERT_TRUE( vVirtualClassPtr.GetAs<VirtualClass*>() == vVirtualClassPtr.GetBytes() );
	ASSERT_TRUE( vVirtualClassConstPtr.GetAs<VirtualClass const*>() == vVirtualClassConstPtr.GetBytes() );
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
	ASSERT_TRUE( vVirtualClassPtr.GetNumBytes() == sizeof( VirtualClass* ) );
	ASSERT_TRUE( vVirtualClassConstPtr.GetNumBytes() == sizeof( VirtualClass const* ) );
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
	Value const vUInt8( static_cast<uint8_t>( 0u ) );
	Value const vInt8( static_cast<int8_t>( 0 ) );

	ASSERT_TRUE( vUInt8.GetAs<int8_t>() == nullptr );
	ASSERT_TRUE( vInt8.GetAs<uint8_t>() == nullptr );

	ASSERT_TRUE( vUInt8.GetAs<int8_t const>() == nullptr );
	ASSERT_TRUE( vInt8.GetAs<uint8_t const>() == nullptr );
}



TEST( Reflect, ValueCopy )
{
	Value vInvalid;
	Value const vUInt8( static_cast<uint8_t>( 1u ) );
	Value const vInt8( static_cast<int8_t>( 2 ) );

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



TEST( Reflect, ValueConstructFromBytes )
{
	bool const Bool{ false };
	void* const VoidPtr{ nullptr };
	void const* const VoidConstPtr{ nullptr };
	VirtualClass* const VirtualClassPtr{ nullptr };
	VirtualClass const* const VirtualClassConstPtr{ nullptr };
	char const Char{ '\0' };
	wchar_t const WChar{ L'\0' };
	char16_t const Char16{ u'\0' };
	char32_t const Char32{ U'\0' };
	float const Float{ 0.f };
	double const Double{ 0.0 };
	long double const LongDouble{ 0.0l };
	uint8_t const UInt8{ 0u };
	int8_t const Int8{ 0 };
	uint16_t const UInt16{ 0u };
	int16_t const Int16{ 0 };
	uint32_t const UInt32{ 0ul };
	int32_t const Int32{ 0l };
	uint64_t const UInt64{ 0ull };
	int64_t const Int64{ 0ll };

	Value const vBool( Value::Type::Bool, &Bool );
	Value const vVoidPtr( Value::Type::VoidPtr, &VoidPtr );
	Value const vVoidConstPtr( Value::Type::VoidConstPtr, &VoidConstPtr );
	Value const vVirtualClassPtr( Value::Type::VirtualClassPtr, &VirtualClassPtr );
	Value const vVirtualClassConstPtr( Value::Type::VirtualClassConstPtr, &VirtualClassConstPtr );
	Value const vChar( Value::Type::Char, &Char );
	Value const vWChar( Value::Type::WChar, &WChar );
	Value const vChar16( Value::Type::Char16, &Char16 );
	Value const vChar32( Value::Type::Char32, &Char32 );
	Value const vFloat( Value::Type::Float, &Float );
	Value const vDouble( Value::Type::Double, &Double );
	Value const vLongDouble( Value::Type::LongDouble, &LongDouble );
	Value const vUInt8( Value::Type::UInt8, &UInt8 );
	Value const vInt8( Value::Type::Int8, &Int8 );
	Value const vUInt16( Value::Type::UInt16, &UInt16 );
	Value const vInt16( Value::Type::Int16, &Int16 );
	Value const vUInt32( Value::Type::UInt32, &UInt32 );
	Value const vInt32( Value::Type::Int32, &Int32 );
	Value const vUInt64( Value::Type::UInt64, &UInt64 );
	Value const vInt64( Value::Type::Int64, &Int64 );

	ASSERT_TRUE( *vBool.GetAs<bool>() == Bool );
	ASSERT_TRUE( *vVoidPtr.GetAs<void*>() == VoidPtr );
	ASSERT_TRUE( *vVoidConstPtr.GetAs<void const*>() == VoidConstPtr );
	ASSERT_TRUE( *vVirtualClassPtr.GetAs<VirtualClass*>() == VirtualClassPtr );
	ASSERT_TRUE( *vVirtualClassConstPtr.GetAs<VirtualClass const*>() == VirtualClassConstPtr );
	ASSERT_TRUE( *vChar.GetAs<char>() == Char );
	ASSERT_TRUE( *vWChar.GetAs<wchar_t>() == WChar );
	ASSERT_TRUE( *vChar16.GetAs<char16_t>() == Char16 );
	ASSERT_TRUE( *vChar32.GetAs<char32_t>() == Char32 );
	ASSERT_TRUE( math::UncheckedEquals( *vFloat.GetAs<float>(), Float ) );
	ASSERT_TRUE( math::UncheckedEquals( *vDouble.GetAs<double>(), Double ) );
	ASSERT_TRUE( math::UncheckedEquals( *vLongDouble.GetAs<long double>(), LongDouble ) );
	ASSERT_TRUE( *vUInt8.GetAs<uint8_t>() == UInt8 );
	ASSERT_TRUE( *vInt8.GetAs<int8_t>() == Int8 );
	ASSERT_TRUE( *vUInt16.GetAs<uint16_t>() == UInt16 );
	ASSERT_TRUE( *vInt16.GetAs<int16_t>() == Int16 );
	ASSERT_TRUE( *vUInt32.GetAs<uint32_t>() == UInt32 );
	ASSERT_TRUE( *vInt32.GetAs<int32_t>() == Int32 );
	ASSERT_TRUE( *vUInt64.GetAs<uint64_t>() == UInt64 );
	ASSERT_TRUE( *vInt64.GetAs<int64_t>() == Int64 );
}



TEST( Reflect, StandardValueConvert )
{
	// Pointers can cast around
	ASSERT_EQ( *Value( reinterpret_cast<void*>( 0x8 ) ).ConvertTo<void*>().GetAs<void*>(), reinterpret_cast<void*>( 0x8 ) );
	ASSERT_EQ( *Value( reinterpret_cast<void*>( 0x8 ) ).ConvertTo<void const*>().GetAs<void const*>(), reinterpret_cast<void const*>( 0x8 ) );
	ASSERT_EQ( *Value( reinterpret_cast<void*>( 0x8 ) ).ConvertTo<VirtualClass*>().GetAs<VirtualClass*>(), reinterpret_cast<VirtualClass*>( 0x8 ) );
	ASSERT_EQ( *Value( reinterpret_cast<void*>( 0x8 ) ).ConvertTo<VirtualClass const*>().GetAs<VirtualClass const*>(), reinterpret_cast<VirtualClass const*>( 0x8 ) );

	// Chars can cast around
	ASSERT_EQ( *Value( static_cast<char>( 'a' ) ).ConvertTo<char>().GetAs<char>(), 'a' );
	ASSERT_EQ( *Value( static_cast<char>( 'a' ) ).ConvertTo<wchar_t>().GetAs<wchar_t>(), L'a' );
	ASSERT_EQ( *Value( static_cast<char>( 'a' ) ).ConvertTo<char16_t>().GetAs<char16_t>(), u'a' );
	ASSERT_EQ( *Value( static_cast<char>( 'a' ) ).ConvertTo<char32_t>().GetAs<char32_t>(), U'a' );

	// Floats can cast around
	ASSERT_EQ( *Value( static_cast<float>( 0.5f ) ).ConvertTo<float>().GetAs<float>(), 0.5 );
	ASSERT_EQ( *Value( static_cast<float>( 0.5f ) ).ConvertTo<double>().GetAs<double>(), 0.5 );
	ASSERT_EQ( *Value( static_cast<float>( 0.5f ) ).ConvertTo<long double>().GetAs<long double>(), 0.5l );

	// Ints can cast around
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<uint8_t>().GetAs<uint8_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<int8_t>().GetAs<int8_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<uint16_t>().GetAs<uint16_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<int16_t>().GetAs<int16_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<uint32_t>().GetAs<uint32_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<int32_t>().GetAs<int32_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<uint64_t>().GetAs<uint64_t>(), 80 );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 80 ) ).ConvertTo<int64_t>().GetAs<int64_t>(), 80 );
}



TEST( Reflect, SketchyValueConvert )
{
	// Converting ints to chars is dangerous business, but Squirrel does it, so
	//  we need to support it
	ASSERT_EQ( *Value( static_cast<uint8_t>( 'a' ) ).ConvertTo<char>().GetAs<char>(), 'a' );
	ASSERT_EQ( *Value( static_cast<uint16_t>( 'a' ) ).ConvertTo<char>().GetAs<char>(), 'a' );
	ASSERT_EQ( *Value( static_cast<uint32_t>( 'a' ) ).ConvertTo<char>().GetAs<char>(), 'a' );
	ASSERT_EQ( *Value( static_cast<uint64_t>( 'a' ) ).ConvertTo<char>().GetAs<char>(), 'a' );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 'a' ) ).ConvertTo<wchar_t>().GetAs<wchar_t>(), L'a' );
	ASSERT_EQ( *Value( static_cast<uint16_t>( 'a' ) ).ConvertTo<wchar_t>().GetAs<wchar_t>(), L'a' );
	ASSERT_EQ( *Value( static_cast<uint32_t>( 'a' ) ).ConvertTo<wchar_t>().GetAs<wchar_t>(), L'a' );
	ASSERT_EQ( *Value( static_cast<uint64_t>( 'a' ) ).ConvertTo<wchar_t>().GetAs<wchar_t>(), L'a' );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 'a' ) ).ConvertTo<char16_t>().GetAs<char16_t>(), u'a' );
	ASSERT_EQ( *Value( static_cast<uint16_t>( 'a' ) ).ConvertTo<char16_t>().GetAs<char16_t>(), u'a' );
	ASSERT_EQ( *Value( static_cast<uint32_t>( 'a' ) ).ConvertTo<char16_t>().GetAs<char16_t>(), u'a' );
	ASSERT_EQ( *Value( static_cast<uint64_t>( 'a' ) ).ConvertTo<char16_t>().GetAs<char16_t>(), u'a' );
	ASSERT_EQ( *Value( static_cast<uint8_t>( 'a' ) ).ConvertTo<char32_t>().GetAs<char32_t>(), U'a' );
	ASSERT_EQ( *Value( static_cast<uint16_t>( 'a' ) ).ConvertTo<char32_t>().GetAs<char32_t>(), U'a' );
	ASSERT_EQ( *Value( static_cast<uint32_t>( 'a' ) ).ConvertTo<char32_t>().GetAs<char32_t>(), U'a' );
	ASSERT_EQ( *Value( static_cast<uint64_t>( 'a' ) ).ConvertTo<char32_t>().GetAs<char32_t>(), U'a' );
}



TEST( Reflect, ValueHash )
{
	// Really, this just needs to compile
	// TODO: More exhaustive testing?
	rftl::hash<Value>()( Value{} );
}

///////////////////////////////////////////////////////////////////////////////
}}

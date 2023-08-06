#include "stdafx.h"

#include "Value.h"

#include "core_math/math_casts.h"
#include "core/macros.h"

#include "rftl/sstream"
#include "rftl/string"
#include "rftl/extension/string_traits.h"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename InT,
	typename rftl::enable_if<rftl::is_char<InT>::value == false, int>::type = 0>
rftl::stringstream ConvertTempToSS( InT const& source )
{
	rftl::stringstream ss;
	ss << source;
	return ss;
}

template<typename InT,
	typename rftl::enable_if<rftl::is_char<InT>::value, int>::type = 0>
rftl::stringstream ConvertTempToSS( InT const& source )
{
	rftl::stringstream ss;
	ss << math::char_integer_bitcast( source );
	return ss;
}



template<typename OutT,
	typename rftl::enable_if<rftl::is_char<OutT>::value == false, int>::type = 0>
OutT ConvertSSToTemp( rftl::stringstream&& ss )
{
	OutT temp = OutT();
	ss >> temp;
	return temp;
}

template<typename OutT,
	typename rftl::enable_if<rftl::is_char<OutT>::value, int>::type = 0>
OutT ConvertSSToTemp( rftl::stringstream&& ss )
{
	using Temp = decltype( math::char_integer_bitcast( OutT() ) );
	Temp temp = 0;
	ss >> temp;
	return static_cast<OutT>( temp );
}



template<> void* ConvertSSToTemp<void*>( rftl::stringstream && ss )
{
	return reinterpret_cast<void*>( ConvertSSToTemp<uint64_t>( rftl::move( ss ) ) );
}
template<> void const* ConvertSSToTemp<void const*>( rftl::stringstream && ss )
{
	return ConvertSSToTemp<void*>( rftl::move( ss ) );
}
template<> VirtualClass* ConvertSSToTemp<VirtualClass*>( rftl::stringstream && ss )
{
	return reinterpret_cast<VirtualClass*>( ConvertSSToTemp<uint64_t>( rftl::move( ss ) ) );
}
template<> VirtualClass const* ConvertSSToTemp<VirtualClass const*>( rftl::stringstream && ss )
{
	return ConvertSSToTemp<VirtualClass*>( rftl::move( ss ) );
}
template<> char ConvertSSToTemp<char>( rftl::stringstream && ss )
{
	rftl::string const asStr = ss.str();
	if( asStr.size() == 1 )
	{
		// WARNING: (ss >> ch) causes (' ' -> '\0'), so don't use '>>' for char
		return asStr.at( 0 );
	}
	else
	{
		// If you're trying to convert odd types to characters, you're going to
		//  get quirky effects, so avoid doing it and remove the disambiguation
		//  before trying to convert
		return math::integer_cast<char>( ConvertSSToTemp<uint64_t>( rftl::move( ss ) ) );
	}
}
template<> wchar_t ConvertSSToTemp<wchar_t>( rftl::stringstream && ss )
{
	// HACK: Treat as char
	return static_cast<wchar_t>( ConvertSSToTemp<char>( rftl::move( ss ) ) );
}
template<> char16_t ConvertSSToTemp<char16_t>( rftl::stringstream && ss )
{
	// HACK: Treat as char
	return static_cast<char16_t>( ConvertSSToTemp<char>( rftl::move( ss ) ) );
}
template<> char32_t ConvertSSToTemp<char32_t>( rftl::stringstream && ss )
{
	// HACK: Treat as char
	return static_cast<char32_t>( ConvertSSToTemp<char>( rftl::move( ss ) ) );
}



template<typename InT, typename OutT,
	typename rftl::enable_if<
		rftl::conjunction<
			Value::IntegralTypes::Contains<InT>,
			Value::IntegralTypes::Contains<OutT>>::value == false,
		int>::type = 0>
OutT ConvertTemps( InT const& source )
{
	rftl::stringstream ss = ConvertTempToSS<InT>( source );
	OutT const outVal = ConvertSSToTemp<OutT>( rftl::move( ss ) );
	return outVal;
}



template<typename InT, typename OutT,
	typename rftl::enable_if<
		rftl::conjunction<
			Value::IntegralTypes::Contains<InT>,
			Value::IntegralTypes::Contains<OutT>>::value,
		int>::type = 0>
OutT ConvertTemps( InT const& source )
{
	return math::integer_cast<OutT, InT>( source );
}



template<Value::Type inID, Value::Type outID>
Value ConvertValue( Value const& source )
{
	using InT = typename Value::ValueTypes::ByIndex<static_cast<size_t>( inID )>::type;
	using OutT = typename Value::ValueTypes::ByIndex<static_cast<size_t>( outID )>::type;

	InT const* temp = source.GetAs<InT>();
	if( temp == nullptr )
	{
		RF_DBGFAIL_MSG( "Failed to convert type" );
		return Value();
	}

	OutT const outVal = ConvertTemps<InT, OutT>( *temp );
	return Value( outVal );
}

}
///////////////////////////////////////////////////////////////////////////////

Value::Value()
	: mInternalStorage( InvalidTag() )
{
	//
}



Value::Value( Type type )
{
	// TODO: Template recursion instead on the typelist
	switch( type )
	{
		case Type::Bool:				mInternalStorage = static_cast<bool>( false ); break;
		case Type::VoidPtr:				mInternalStorage = static_cast<void*>( nullptr ); break;
		case Type::VoidConstPtr:		mInternalStorage = static_cast<void const*>( nullptr ); break;
		case Type::VirtualClassPtr:		mInternalStorage = static_cast<VirtualClass*>( nullptr ); break;
		case Type::VirtualClassConstPtr:mInternalStorage = static_cast<VirtualClass const*>( nullptr ); break;
		case Type::Char:				mInternalStorage = static_cast<char>( 0 ); break;
		case Type::WChar:				mInternalStorage = static_cast<wchar_t>( 0 ); break;
		case Type::Char16:				mInternalStorage = static_cast<char16_t>( 0 ); break;
		case Type::Char32:				mInternalStorage = static_cast<char32_t>( 0 ); break;
		case Type::Float:				mInternalStorage = static_cast<float>( 0 ); break;
		case Type::Double:				mInternalStorage = static_cast<double>( 0 ); break;
		case Type::LongDouble:			mInternalStorage = static_cast<long double>( 0 ); break;
		case Type::UInt8:				mInternalStorage = static_cast<uint8_t>( 0 ); break;
		case Type::UInt16:				mInternalStorage = static_cast<int8_t>( 0 ); break;
		case Type::UInt32:				mInternalStorage = static_cast<uint16_t>( 0 ); break;
		case Type::UInt64:				mInternalStorage = static_cast<int16_t>( 0 ); break;
		case Type::Int8:				mInternalStorage = static_cast<uint32_t>( 0 ); break;
		case Type::Int16:				mInternalStorage = static_cast<int32_t>( 0 ); break;
		case Type::Int32:				mInternalStorage = static_cast<uint64_t>( 0 ); break;
		case Type::Int64:				mInternalStorage = static_cast<int64_t>( 0 ); break;
		case Type::Invalid:				mInternalStorage = InvalidTag(); break;
		default:
		{
			RF_DBGFAIL();
			mInternalStorage = InvalidTag();
			break;
		}
	}
}



Value::Value( Type type, void const* bytes )
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


bool Value::operator==( Value const& rhs ) const
{
	if( GetStoredType() != rhs.GetStoredType() )
	{
		return false;
	}

	size_t const numBytes = GetNumBytes();
	if( numBytes != rhs.GetNumBytes() )
	{
		return false;
	}

	uint8_t const* const leftBytes = static_cast<uint8_t const*>( GetBytes() );
	uint8_t const* const rightBytes = static_cast<uint8_t const*>( rhs.GetBytes() );
	RF_ASSERT( leftBytes != nullptr );
	RF_ASSERT( rightBytes != nullptr );
	for( size_t i = 0; i < numBytes; i++ )
	{
		uint8_t const& leftByte = leftBytes[i];
		uint8_t const& rightByte = rightBytes[i];
		if(leftByte != rightByte)
		{
			return false;
		}
	}

	return true;
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
	RF_ASSERT( type != Type::Invalid );
	size_t const typeAsIndex = static_cast<size_t>( type );
	RF_ASSERT( typeAsIndex < rftl::extent<decltype( kTypeNameStrings )>::value );
	return kTypeNameStrings[typeAsIndex];
}


void const* Value::GetBytes() const
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



Value Value::ConvertTo( Type target ) const
{
	Type const source = GetStoredType();

	using convertFunc = Value(*)( Value const&);
	#define Q(I,O) details::ConvertValue<static_cast<Value::Type>(I),static_cast<Value::Type>(O)>
	#define QL(I) \
		Q(I, 0), \
		Q(I, 1), \
		Q(I, 2), \
		Q(I, 3), \
		Q(I, 4), \
		Q(I, 5), \
		Q(I, 6), \
		Q(I, 7), \
		Q(I, 8), \
		Q(I, 9), \
		Q(I,10), \
		Q(I,11), \
		Q(I,12), \
		Q(I,13), \
		Q(I,14), \
		Q(I,15), \
		Q(I,16), \
		Q(I,17), \
		Q(I,18), \
		Q(I,19)
	static constexpr convertFunc convertTable[ValueTypes::kNumTypes][ValueTypes::kNumTypes] =
	{
		{ QL(  0 ) },
		{ QL(  1 ) },
		{ QL(  2 ) },
		{ QL(  3 ) },
		{ QL(  4 ) },
		{ QL(  5 ) },
		{ QL(  6 ) },
		{ QL(  7 ) },
		{ QL(  8 ) },
		{ QL(  9 ) },
		{ QL( 10 ) },
		{ QL( 11 ) },
		{ QL( 12 ) },
		{ QL( 13 ) },
		{ QL( 14 ) },
		{ QL( 15 ) },
		{ QL( 16 ) },
		{ QL( 17 ) },
		{ QL( 18 ) },
		{ QL( 19 ) } };
	#undef QL
	#undef Q

	return convertTable[static_cast<int>( source )][static_cast<int>( target )]( *this );
}

///////////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "core_reflect/ReflectFwd.h"

#include "core/meta/TypeList.h"
#include "core/macros.h"

#include "rftl/extension/byte_view.h"
#include "rftl/variant"
#include "rftl/cstdint"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

// WARNING: It's a pretty bad idea to be trying to store Unicode information
//  into reflected values. Since reflected values don't support storing strings
//  or other variable-length data, they can't contain data encodied in UTF-8,
//  UTF-16, UTF-32, and so on. The support for characters here is thus intended
//  for data transfer purposes only.
// NOTE: Conversion of types to characters is ESPECIALLY shady and error-prone

#define RF_REFLECT_VALUE_TYPELIST \
	bool, \
		void*, void const*, \
		::RF::reflect::VirtualClass*, ::RF::reflect::VirtualClass const*, \
		char, wchar_t, char8_t, char16_t, char32_t, \
		float, double, long double, \
		uint8_t, int8_t, \
		uint16_t, int16_t, \
		uint32_t, int32_t, \
		uint64_t, int64_t

#define RF_REFLECT_VALUE_TYPELIST_NAMES \
	Bool, \
		VoidPtr, VoidConstPtr, \
		VirtualClassPtr, VirtualClassConstPtr, \
		Char, WChar, Char8, Char16, Char32, \
		Float, Double, LongDouble, \
		UInt8, Int8, \
		UInt16, Int16, \
		UInt32, Int32, \
		UInt64, Int64

#define RF_REFLECT_VALUE_TYPELIST_STRINGS \
	"Bool", \
		"VoidPtr", "VoidConstPtr", \
		"VirtualClassPtr", "VirtualClassConstPtr", \
		"Char", "WChar", "Char8", "Char16", "Char32", \
		"Float", "Double", "LongDouble", \
		"UInt8", "Int8", \
		"UInt16", "Int16", \
		"UInt32", "Int32", \
		"UInt64", "Int64"

class Value
{
	RF_DEFAULT_COPY( Value );

	//
	// Forwards
private:
	struct InvalidTagType;


	//
	// Enums
public:
	enum class Type : int8_t
	{
		Invalid = -1,
		RF_REFLECT_VALUE_TYPELIST_NAMES
	};


	//
	// Types
public:
	using ValueTypes = TypeList<RF_REFLECT_VALUE_TYPELIST>;
	using CharacterTypes = TypeList<char, wchar_t, char8_t, char16_t, char32_t>;
	using IntegralTypes = TypeList<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t>;

	// Someone on the C++ standards committee fucked up! You can't make an
	//  empty variant unless you throw exceptions! What the fuck? So, we'll
	//  use a broken type as a hack
	using InvalidTag = InvalidTagType const*;
	using InternalStorage = rftl::variant<RF_REFLECT_VALUE_TYPELIST, InvalidTag>;


	//
	// Constants
public:
	static constexpr char const* kTypeNameStrings[] = { RF_REFLECT_VALUE_TYPELIST_STRINGS };


	//
	// Public methods
public:
	Value();
	template<typename T>
	explicit Value( T const& value );
	explicit Value( Type type );
	explicit Value( Type type, void const* bytes );

	// This is a very explicit equals, and will not do conversions
	bool operator==( Value const& rhs ) const;

	Type GetStoredType() const;
	char const* GetStoredTypeName() const;
	static char const* GetTypeName( Type type );

	template<typename T>
	typename rftl::remove_const<T>::type const* GetAs() const;
	rftl::byte_view GetBytes() const;
	void const* GetRawBytes() const;
	size_t GetNumBytes() const;
	static size_t GetNumBytesNeeded( Type type );

	template<typename T>
	static constexpr Type DetermineType();

	template<typename T>
	Value ConvertTo() const;
	Value ConvertTo( Type target ) const;


	//
	// Private data
private:
	InternalStorage mInternalStorage;
};

#undef RF_REFLECT_VALUE_TYPELIST
#undef RF_REFLECT_VALUE_TYPELIST_NAMES
#undef RF_REFLECT_VALUE_TYPELIST_STRINGS

///////////////////////////////////////////////////////////////////////////////
}

namespace std {
template<> struct hash<RF::reflect::Value>
{
	size_t operator()( RF::reflect::Value const& val ) const
	{
		hash<RF::reflect::Value::InternalStorage> hasher = hash<RF::reflect::Value::InternalStorage>();
		// TODO: Friend
		return hasher( *reinterpret_cast<RF::reflect::Value::InternalStorage const*>( &val ) );
	}
};
}

namespace std {
template<> struct equal_to<RF::reflect::Value>
{
	bool operator()( RF::reflect::Value const& lhs, RF::reflect::Value const& rhs ) const
	{
		equal_to<RF::reflect::Value::InternalStorage> equalizer = equal_to<RF::reflect::Value::InternalStorage>();
		// TODO: Friend
		return equalizer(
			*reinterpret_cast<RF::reflect::Value::InternalStorage const*>( &lhs ),
			*reinterpret_cast<RF::reflect::Value::InternalStorage const*>( &rhs ) );
	}
};
}

#include "Value.inl"

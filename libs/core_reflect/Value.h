#pragma once

#include "core/meta/TypeList.h"
#include "rftl/variant"
#include "rftl/cstdint"


// Forwards
namespace RF { namespace reflect {
	class VirtualClass;
}}

namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

#define RF_REFLECT_VALUE_TYPELIST \
		bool, \
		void*, void const*, \
		VirtualClass*, VirtualClass const*, \
		char, wchar_t, char16_t, char32_t, \
		float, double, long double, \
		uint8_t, int8_t, \
		uint16_t, int16_t, \
		uint32_t, int32_t, \
		uint64_t, int64_t

#define RF_REFLECT_VALUE_TYPELIST_NAMES \
		Bool, \
		VoidPtr, VoidConstPtr, \
		VirtualClassPtr, VirtualClassConstPtr, \
		Char, WChar, Char16, Char32, \
		Float, Double, LongDouble, \
		UInt8, Int8, \
		UInt16, Int16, \
		UInt32, Int32, \
		UInt64, Int64

#define RF_REFLECT_VALUE_TYPELIST_STRINGS \
		"Bool", \
		"VoidPtr", "VoidConstPtr", \
		"VirtualClassPtr", "VirtualClassConstPtr", \
		"Char", "WChar", "Char16", "Char32", \
		"Float", "Double", "LongDouble", \
		"UInt8", "Int8", \
		"UInt16", "Int16", \
		"UInt32", "Int32", \
		"UInt64", "Int64"

class Value
{
	//
	// Forwards
private:
	struct InvalidType;


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
	using InternalStorage = rftl::variant<RF_REFLECT_VALUE_TYPELIST, InvalidType>;


	//
	// Constants
public:
	static constexpr char const* kTypeNameStrings[] = { RF_REFLECT_VALUE_TYPELIST_STRINGS };


	//
	// Structs
private:
	struct InvalidType
	{
		// Someone on the C++ standards committee fucked up! You can't make an
		//  empty variant unless you throw exceptions! What the fuck? So, we'll
		//  use a broken type as a hack
		InvalidType() = default;
	};


	//
	// Public methods
public:
	Value();
	template<typename T>
	explicit Value( T const& value );
	Value( Value const& ) = default;
	Value& operator =( Value const& ) = default;
	Type GetStoredType() const;
	template<typename T>
	typename rftl::remove_const<T>::type const* GetAs() const;
	void const* GetBytes() const;
	size_t GetNumBytes() const;
	template<typename T>
	static constexpr Type DetermineType();


	//
	// Private data
private:
	InternalStorage m_InternalStorage;
};

#undef RF_REFLECT_VALUE_TYPELIST
#undef RF_REFLECT_VALUE_TYPELIST_NAMES
#undef RF_REFLECT_VALUE_TYPELIST_STRINGS

///////////////////////////////////////////////////////////////////////////////
}}

#include "Value.inl"

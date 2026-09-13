#pragma once
#include "rftl/optional"
#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Like a optional<...>, but instead of a bool it uses an invalid value and
//  constructs the object even when invalid, and thus requires relatively
//  simple types
template<typename T, T InvalidVal>
class inline_optional
{
	//
	// Types
public:
	using value_type = T;
	static_assert( is_trivially_constructible<value_type>::value );
	static_assert( is_trivially_destructible<value_type>::value );
private:
	static constexpr value_type kInvalid = InvalidVal;


	//
	// Public methods
public:
	constexpr inline_optional() = default;
	constexpr inline_optional( nullopt_t );
	constexpr inline_optional( value_type&& value );

	template<typename... ArgsT>
	constexpr inline_optional( in_place_t, ArgsT&&... args );

	constexpr value_type const* operator->() const;
	constexpr value_type* operator->();
	constexpr value_type const& operator*() const&;
	constexpr value_type& operator*() &;
	constexpr value_type const&& operator*() const&&;
	constexpr value_type&& operator*() &&;

	constexpr explicit operator bool() const;
	constexpr bool has_value() const;

	constexpr value_type const& value() const&;
	constexpr value_type& value() &;
	constexpr value_type const&& value() const&&;
	constexpr value_type&& value() &&;

	constexpr value_type value_or( value_type&& fallback ) const&;
	constexpr value_type value_or( value_type&& fallback ) &&;


	//
	// Private data
private:
	value_type mVal = kInvalid;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "inline_optional.inl"

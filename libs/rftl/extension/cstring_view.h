#pragma once
#include "rftl/format"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Like a basic_string_view<...>, but can only be made from things that are
//  null-terminated
// NOTE: C++ might eventually get this support for real
// SEE: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p3655r0.html
template<typename CharT>
class basic_cstring_view
{
	//
	// Types
public:
	typedef CharT value_type;


	//
	// Public methods
public:
	basic_cstring_view() = default;
	basic_cstring_view( value_type const* ptr );

	// WARNING: Will fatal if the underlying pointer is null, since it is not
	//  null-terminated
	operator basic_string_view<value_type>() const;

	// WARNING: May expose a null pointer
	value_type const* c_str() const;


	//
	// Private data
private:
	value_type const* mPtr = nullptr;
};


using cstring_view = basic_cstring_view<char>;
using wcstring_view = basic_cstring_view<wchar_t>;
using u8cstring_view = basic_cstring_view<char8_t>;
using u16cstring_view = basic_cstring_view<char16_t>;
using u32cstring_view = basic_cstring_view<char32_t>;

///////////////////////////////////////////////////////////////////////////////
}

// Formats as a pointer
template<typename CharT, typename CtxCharT>
struct rftl::formatter<rftl::basic_cstring_view<CharT>, CtxCharT> : rftl::formatter<CharT const*, CtxCharT>
{
	using Input = rftl::basic_cstring_view<CharT>;
	using Shim = CharT const*;
	using Base = rftl::formatter<Shim, CtxCharT>;

	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		return Base::parse( ctx );
	}

	template<class FmtContext>
	typename FmtContext::iterator format( Input const& arg, FmtContext& ctx ) const
	{
		return Base::format( static_cast<Shim>( arg.c_str() ), ctx );
	}
};

#include "cstring_view.inl"

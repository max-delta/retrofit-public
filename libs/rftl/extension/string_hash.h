#pragma once
#include "rftl/string"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// The intent of this is to allow string keys to be used in unordered maps, but
//  for string views to be used for the lookups without needing to allocate a
//  temporary string, as would be required with the default hash
// EXAMPLE:
//  rftl::unordered_map<rftl::string, VAL, rftl::string_hash, rftl::equal_to<>> const map;
// 	rftl::string_view const key = "key";
//  map.find( RFTL_STR_V_HASH( key ) );
struct string_hash
{
private:
	using hasher = rftl::hash<rftl::string_view>;

public:
	// NOTE: Not really sure what this is, conceptually, but it makes it work,
	//  and is required for templates that check for the existence of it
	// NOTE: Type doesn't appear to be used, just needs to be present
	using is_transparent = void;

public:
	size_t operator()( rftl::string_view const& str ) const
	{
		return hasher{}( str );
	};

	size_t operator()( rftl::string const& str ) const
	{
		return operator()( rftl::string_view( str ) );
	};

	size_t operator()( char const* str ) const
	{
		return operator()( rftl::string_view( str ) );
	};
};

///////////////////////////////////////////////////////////////////////////////

// HACK: Workaround for Intellisense not working well with templatized find
#if defined( __INTELLISENSE__ )
	#define RFTL_STR_V_HASH( x ) rftl::string( x )
#else
	#define RFTL_STR_V_HASH( x ) x
#endif

///////////////////////////////////////////////////////////////////////////////
}

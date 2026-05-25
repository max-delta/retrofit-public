#pragma once
#include "rftl/extension/fatal_exception.h"
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

// The intent of this is to allow string keys to be used in unordered maps, but
//  for pointers to be used for the lookups without actually inspecting the
//  character data, for esoteric cases where the pointer should already be
//  known during the lookup
// EXAMPLE:
//  rftl::unordered_map<
//     rftl::string_view,
//     VAL,
//     rftl::string_ptr_only_hash_equals,
//     rftl::string_ptr_only_hash_equals> const map;
// 	void const* const key = "key";
//  map.find( key );
struct string_ptr_only_hash_equals
{
private:
	using hasher = rftl::hash<void const*>;

public:
	// NOTE: Not really sure what this is, conceptually, but it makes it work,
	//  and is required for templates that check for the existence of it
	// NOTE: Type doesn't appear to be used, just needs to be present
	using is_transparent = void;

public:
	size_t operator()( rftl::string_view const& str ) const
	{
		return operator()( str.data() );
	};

	size_t operator()( rftl::string const& str ) const
	{
		// IMPORTANT: It is unsafe to use these with strings, as the potential
		//  for small-string-optimization means that the pointer may change on
		//  move-assignment, meaning it would re-key itself as part of the
		//  emplacement process, resulting in a key that could never be found
		//  again due to being in the wrong hash once inserted
		fatal_exception( str );
	};

	size_t operator()( void const* str ) const
	{
		return hasher{}( str );
	};

	static void const* asPtr( void const* str )
	{
		return str;
	}
	static void const* asPtr( rftl::string_view const& str )
	{
		return str.data();
	}

	template<typename LHS, typename RHS>
	size_t operator()( LHS const& lhs, RHS const& rhs ) const
	{
		return asPtr( lhs ) == asPtr( rhs );
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

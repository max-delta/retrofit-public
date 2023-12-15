#include "stdafx.h"

#include "rftl/extension/static_string.h"

#include "rftl/string"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( StaticString, BasicEmpty )
{
	rftl::static_string<5> empty;
	static_assert( rftl::static_string<5>::fixed_capacity == 5, "" );
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == 5 );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
}



TEST( StaticString, NopSizeChanges )
{
	rftl::static_string<5> empty;
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == 5 );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	empty.shrink_to_fit();
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == 5 );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	empty.reserve( 5 );
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == 5 );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
}



TEST( StaticString, BasicLifetime )
{
	rftl::static_string<5> sa;
	rftl::static_string<5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.empty() );
	ASSERT_TRUE( csa.empty() );
	char const copyVal = '2';
	char moveVal = '7';
	sa.push_back( copyVal );
	sa.push_back( rftl::move( moveVal ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.empty() == false );
	ASSERT_TRUE( csa.empty() == false );
	ASSERT_TRUE( sa.at( 0 ) == '2' );
	ASSERT_TRUE( csa.at( 0 ) == '2' );
	ASSERT_TRUE( sa[0] == '2' );
	ASSERT_TRUE( csa[0] == '2' );
	ASSERT_TRUE( sa.at( 1 ) == '7' );
	ASSERT_TRUE( csa.at( 1 ) == '7' );
	ASSERT_TRUE( sa[1] == '7' );
	ASSERT_TRUE( csa[1] == '7' );
	sa.pop_back();
	sa.pop_back();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.empty() );
	ASSERT_TRUE( csa.empty() );
}



TEST( StaticString, BasicBookendAccess )
{
	rftl::static_string<5> sa;
	rftl::static_string<5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( '2' );
	sa.push_back( '7' );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.front() == '2' );
	ASSERT_TRUE( csa.front() == '2' );
	ASSERT_TRUE( sa.back() == '7' );
	ASSERT_TRUE( csa.back() == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StaticString, BasicDataAccess )
{
	rftl::static_string<5> sa;
	rftl::static_string<5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( '2' );
	sa.push_back( '7' );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( csa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( csa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StaticString, BasicIteratorAccess )
{
	rftl::static_string<5> sa;
	rftl::static_string<5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.begin() == sa.end() );
	ASSERT_TRUE( sa.begin() == csa.end() );
	ASSERT_TRUE( csa.begin() == sa.end() );
	ASSERT_TRUE( csa.begin() == csa.end() );
	ASSERT_TRUE( sa.cbegin() == sa.cend() );
	ASSERT_TRUE( sa.cbegin() == csa.cend() );
	ASSERT_TRUE( csa.cbegin() == sa.cend() );
	ASSERT_TRUE( csa.cbegin() == csa.cend() );
	ASSERT_TRUE( sa.rbegin() == sa.rend() );
	ASSERT_TRUE( sa.rbegin() == csa.rend() );
	ASSERT_TRUE( csa.rbegin() == sa.rend() );
	ASSERT_TRUE( csa.rbegin() == csa.rend() );
	ASSERT_TRUE( sa.crbegin() == sa.crend() );
	ASSERT_TRUE( sa.crbegin() == csa.crend() );
	ASSERT_TRUE( csa.crbegin() == sa.crend() );
	ASSERT_TRUE( csa.crbegin() == csa.crend() );
	sa.push_back( '2' );
	sa.push_back( '7' );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( *sa.begin() == '2' );
	ASSERT_TRUE( *csa.begin() == '2' );
	ASSERT_TRUE( *sa.rbegin() == '7' );
	ASSERT_TRUE( *csa.rbegin() == '7' );
	ASSERT_TRUE( csa.begin() + 1 == sa.end() - 1 );
	ASSERT_TRUE( csa.begin() + 1 == csa.end() - 1 );
	ASSERT_TRUE( sa.cbegin() + 1 == sa.cend() - 1 );
	ASSERT_TRUE( sa.cbegin() + 1 == csa.cend() - 1 );
	ASSERT_TRUE( csa.cbegin() + 1 == sa.cend() - 1 );
	ASSERT_TRUE( csa.cbegin() + 1 == csa.cend() - 1 );
	ASSERT_TRUE( csa.rbegin() + 1 == sa.rend() - 1 );
	ASSERT_TRUE( csa.rbegin() + 1 == csa.rend() - 1 );
	ASSERT_TRUE( sa.crbegin() + 1 == sa.crend() - 1 );
	ASSERT_TRUE( sa.crbegin() + 1 == csa.crend() - 1 );
	ASSERT_TRUE( csa.crbegin() + 1 == sa.crend() - 1 );
	ASSERT_TRUE( csa.crbegin() + 1 == csa.crend() - 1 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.begin() == sa.end() );
	ASSERT_TRUE( sa.begin() == csa.end() );
	ASSERT_TRUE( csa.begin() == sa.end() );
	ASSERT_TRUE( csa.begin() == csa.end() );
	ASSERT_TRUE( sa.cbegin() == sa.cend() );
	ASSERT_TRUE( sa.cbegin() == csa.cend() );
	ASSERT_TRUE( csa.cbegin() == sa.cend() );
	ASSERT_TRUE( csa.cbegin() == csa.cend() );
	ASSERT_TRUE( sa.rbegin() == sa.rend() );
	ASSERT_TRUE( sa.rbegin() == csa.rend() );
	ASSERT_TRUE( csa.rbegin() == sa.rend() );
	ASSERT_TRUE( csa.rbegin() == csa.rend() );
	ASSERT_TRUE( sa.crbegin() == sa.crend() );
	ASSERT_TRUE( sa.crbegin() == csa.crend() );
	ASSERT_TRUE( csa.crbegin() == sa.crend() );
	ASSERT_TRUE( csa.crbegin() == csa.crend() );
}



TEST( StaticString, InitializerListConstruct )
{
	rftl::static_string<5> sa = { '2', '7' };
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, ViewConstruct )
{
	rftl::static_string<5> sa = rftl::static_string<5>( rftl::string_view( "27" ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, IterConstruct )
{
	char const source[] = { '2', '7' };
	rftl::static_string<5> sa( &source[0], &source[2] );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, CopyConstruct )
{
	rftl::static_string<5> source;
	source.push_back( '2' );
	source.push_back( '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );
	rftl::static_string<5> const& input = source;

	rftl::static_string<5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, CopyConstructCrossCapacity )
{
	rftl::static_string<2> source;
	source.push_back( '2' );
	source.push_back( '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );
	rftl::static_string<5> const& input = source;

	rftl::static_string<5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, MoveConstruct )
{
	rftl::static_string<5> source;
	source.push_back( '2' );
	source.push_back( '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );

	rftl::static_string<5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, MoveConstructCrossCapacity )
{
	rftl::static_string<2> source;
	source.push_back( '2' );
	source.push_back( '7' );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == '2' );
	ASSERT_TRUE( source.data()[1] == '7' );

	rftl::static_string<5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, ValueConstruct )
{
	// At time of writing 'sa(2,2)' is unclear, and templatization into
	//  iterators takes precedence before the implicit constructor of the
	//  proxy. This would be solved by a harder iterator type, but seems
	//  not worth the effort at this time, since users can clarify their
	//  implicit call with minimal effort.
	//rftl::static_string<5> sa( 2, 2 ); // Similar case on wipe test
	rftl::static_string<5> sa( 2, char( '2' ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '2' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, ValueWipe )
{
	rftl::static_string<5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	//sa.assign( 2, 2 ); // See notes on construct test
	sa.assign( 2, char( '2' ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '2' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, Insert )
{
	rftl::static_string<3> sa;
	ASSERT_TRUE( sa.size() == 0 );
	sa.insert( sa.end(), '2' );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	sa.insert( sa.end(), '7' );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, Erase )
{
	rftl::static_string<4> sa;
	sa.push_back( '2' );
	sa.push_back( '5' );
	sa.push_back( '7' );
	sa.push_back( 'A' );
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '5' );
	ASSERT_TRUE( sa.data()[2] == '7' );
	ASSERT_TRUE( sa.data()[3] == 'A' );

	rftl::static_string<4>::iterator iter;

	iter = sa.erase( sa.begin() + 2 );
	ASSERT_TRUE( iter == sa.begin() + 2 );
	ASSERT_TRUE( sa.size() == 3 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '5' );
	ASSERT_TRUE( sa.data()[2] == 'A' );

	iter = sa.erase( sa.begin() + 2 );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '5' );

	iter = sa.erase( sa.begin() );
	ASSERT_TRUE( iter == sa.begin() );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == '5' );

	iter = sa.erase( sa.begin() );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, Add )
{
	rftl::static_string<5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	sa += decltype( sa )( { '2' } );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	sa += rftl::static_string<1>( { '7' } );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa += '5';
	ASSERT_TRUE( sa.size() == 3 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( sa.data()[2] == '5' );
	sa += { 'A' };
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( sa.data()[2] == '5' );
	ASSERT_TRUE( sa.data()[3] == 'A' );
	sa += rftl::string_view( "B" );
	ASSERT_TRUE( sa.size() == 5 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	ASSERT_TRUE( sa.data()[2] == '5' );
	ASSERT_TRUE( sa.data()[3] == 'A' );
	ASSERT_TRUE( sa.data()[4] == 'B' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, BackInsertCompat )
{
	rftl::static_string<5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	rftl::back_insert_iterator<rftl::static_string<5>> inserter( sa );
	*inserter = '2';
	*inserter = '7';
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == '2' );
	ASSERT_TRUE( sa.data()[1] == '7' );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticString, RangeForCompatibility )
{
	rftl::static_string<5> sa;
	rftl::static_string<5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( '2' );
	sa.push_back( '7' );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	for( char const& elem : sa )
	{
		ASSERT_TRUE( elem == '2' || elem == '7' );
	}
	for( char const& elem : csa )
	{
		ASSERT_TRUE( elem == '2' || elem == '7' );
	}
	sa.clear();
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StaticString, StringConversion )
{
	rftl::static_string<5> sa;
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "" );
	sa.push_back( '2' );
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "2" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "2" );
	sa.push_back( '7' );
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "27" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "27" );
	sa.push_back( 'A' );
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "27A" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "27A" );
	sa.push_back( 'B' );
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "27AB" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "27AB" );
	sa.push_back( 'C' );
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "27ABC" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "27ABC" );
	sa.clear();
	ASSERT_TRUE( sa.c_str() == rftl::string_view( "" ) );
	ASSERT_TRUE( static_cast<rftl::string_view>( sa ) == "" );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"

#include "rftl/extension/stretch_vector.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// For testing that moves are destructive when available
struct Proxy
{
	static constexpr int kDefault = 83;
	static constexpr int kDestroyed = 46;
	Proxy()
		: val( kDefault )
	{
		//
	}

	Proxy( int val )
		: val( val )
	{
		//
	}

	Proxy( Proxy const& other )
		: Proxy( other.val )
	{
		//
	}
	Proxy( Proxy&& other )
		: Proxy( other.val )
	{
		other.val = kDestroyed;
	}

	~Proxy()
	{
		val = kDestroyed;
	}

	Proxy& operator=( Proxy const& ) = delete;

	Proxy& operator=( Proxy&& other )
	{
		val = other.val;
		other.val = kDestroyed;
		return *this;
	}

	operator int() const
	{
		return val;
	}

	int val;
};



TEST( StretchVector, Alignment )
{
	// This adds padding
	static_assert(
		sizeof( rftl::stretch_vector<uint16_t, 5> ) < sizeof( rftl::stretch_vector<uint16_t, 5, alignof( uint32_t )> ),
		"Stricter alignment doesn't appear to have had an effect" );

	// This has no effect, since the backing array is already free of padding
	static_assert(
		sizeof( rftl::stretch_vector<uint16_t, 5> ) == sizeof( rftl::stretch_vector<uint16_t, 5, alignof( uint8_t )> ),
		"Meaninglessly loose alignment somehow affected size" );
}



TEST( StretchVector, BasicEmpty )
{
	rftl::stretch_vector<Proxy, 5> empty;
	static_assert( rftl::stretch_vector<Proxy, 5>::fast_capacity == 5 );
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == rftl::numeric_limits<size_t>::max() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
}



TEST( StretchVector, NopSizeChanges )
{
	rftl::stretch_vector<Proxy, 5> empty;
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == rftl::numeric_limits<size_t>::max() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	empty.shrink_to_fit();
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == rftl::numeric_limits<size_t>::max() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	empty.reserve( 5 );
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == rftl::numeric_limits<size_t>::max() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
}



TEST( StretchVector, BasicLifetime )
{
	rftl::stretch_vector<Proxy, 5> sa;
	rftl::stretch_vector<Proxy, 5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.empty() );
	ASSERT_TRUE( csa.empty() );
	Proxy const copyVal = 2;
	Proxy moveVal = 7;
	sa.push_back( copyVal );
	sa.push_back( rftl::move( moveVal ) );
	ASSERT_TRUE( copyVal == 2 );
	ASSERT_TRUE( moveVal == Proxy::kDestroyed );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.empty() == false );
	ASSERT_TRUE( csa.empty() == false );
	ASSERT_TRUE( sa.at( 0 ) == 2 );
	ASSERT_TRUE( csa.at( 0 ) == 2 );
	ASSERT_TRUE( sa[0] == 2 );
	ASSERT_TRUE( csa[0] == 2 );
	ASSERT_TRUE( sa.at( 1 ) == 7 );
	ASSERT_TRUE( csa.at( 1 ) == 7 );
	ASSERT_TRUE( sa[1] == 7 );
	ASSERT_TRUE( csa[1] == 7 );
	sa.pop_back();
	sa.pop_back();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	ASSERT_TRUE( sa.empty() );
	ASSERT_TRUE( csa.empty() );
}



TEST( StretchVector, BasicBookendAccess )
{
	rftl::stretch_vector<Proxy, 5> sa;
	rftl::stretch_vector<Proxy, 5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( 2 );
	sa.push_back( 7 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.front() == 2 );
	ASSERT_TRUE( csa.front() == 2 );
	ASSERT_TRUE( sa.back() == 7 );
	ASSERT_TRUE( csa.back() == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StretchVector, BasicDataAccess )
{
	rftl::stretch_vector<Proxy, 5> sa;
	rftl::stretch_vector<Proxy, 5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( 2 );
	sa.push_back( 7 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( csa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( csa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StretchVector, BasicIteratorAccess )
{
	rftl::stretch_vector<Proxy, 5> sa;
	rftl::stretch_vector<Proxy, 5> const& csa = sa;
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
	sa.push_back( 2 );
	sa.push_back( 7 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( *sa.begin() == 2 );
	ASSERT_TRUE( *csa.begin() == 2 );
	ASSERT_TRUE( *sa.rbegin() == 7 );
	ASSERT_TRUE( *csa.rbegin() == 7 );
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



TEST( StretchVector, BlindEmplaceGrow )
{
	rftl::stretch_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	sa.emplace_back();
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == Proxy::kDefault );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, InitializerListConstruct )
{
	rftl::stretch_vector<Proxy, 5> sa = { 2, 7 };
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, IterConstruct )
{
	Proxy const source[] = { 2, 7 };
	rftl::stretch_vector<Proxy, 5> sa( &source[0], &source[2] );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, CopyConstruct )
{
	rftl::stretch_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::stretch_vector<Proxy, 5> const& input = source;

	rftl::stretch_vector<Proxy, 5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, CopyConstructCrossCapacity )
{
	rftl::stretch_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::stretch_vector<Proxy, 5> const& input = source;

	rftl::stretch_vector<Proxy, 5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, MoveConstruct )
{
	rftl::stretch_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::stretch_vector<Proxy, 5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, MoveConstructCrossCapacity )
{
	rftl::stretch_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::stretch_vector<Proxy, 5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, SizeConstruct )
{
	rftl::stretch_vector<Proxy, 5> sa( 2 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == Proxy::kDefault );
	ASSERT_TRUE( sa.data()[1] == Proxy::kDefault );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, ValueConstruct )
{
	// At time of writing 'sa(2,2)' is unclear, and templatization into
	//  iterators takes precedence before the implicit constructor of the
	//  proxy. This would be solved by a harder iterator type, but seems
	//  not worth the effort at this time, since users can clarify their
	//  implicit call with minimal effort.
	//rftl::stretch_vector<Proxy, 5> sa( 2, 2 ); // Similar case on wipe test
	rftl::stretch_vector<Proxy, 5> sa( 2, Proxy( 2 ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 2 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, CopyAssign )
{
	rftl::stretch_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::stretch_vector<Proxy, 5> const& input = source;

	rftl::stretch_vector<Proxy, 5> sa;
	sa = input;
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa = input; // Second assign should clear first
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, CopyAssignCrossCapacity )
{
	rftl::stretch_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::stretch_vector<Proxy, 5> const& input = source;

	rftl::stretch_vector<Proxy, 5> sa;
	sa = input;
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa = input; // Second assign should clear first
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, MoveAssign )
{
	rftl::stretch_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::stretch_vector<Proxy, 5> sa;
	sa = rftl::move( source );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, MoveAssignCrossCapacity )
{
	rftl::stretch_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::stretch_vector<Proxy, 5> sa;
	sa = rftl::move( source );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, ValueWipe )
{
	rftl::stretch_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	//sa.assign( 2, 2 ); // See notes on construct test
	sa.assign( 2, Proxy( 2 ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 2 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, Insert )
{
	rftl::stretch_vector<Proxy, 3> sa;
	ASSERT_TRUE( sa.size() == 0 );
	sa.insert( sa.end(), 2 );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	sa.insert( sa.end(), 7 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, Erase )
{
	rftl::stretch_vector<Proxy, 4> sa;
	sa.push_back( 2 );
	sa.push_back( 5 );
	sa.push_back( 7 );
	sa.push_back( 11 );
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );
	ASSERT_TRUE( sa.data()[2] == 7 );
	ASSERT_TRUE( sa.data()[3] == 11 );

	rftl::stretch_vector<Proxy, 4>::iterator iter;

	iter = sa.erase( sa.begin() + 2 );
	ASSERT_TRUE( iter == sa.begin() + 2 );
	ASSERT_TRUE( sa.size() == 3 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );
	ASSERT_TRUE( sa.data()[2] == 11 );

	iter = sa.erase( sa.begin() + 2 );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );

	iter = sa.erase( sa.begin() );
	ASSERT_TRUE( iter == sa.begin() );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == 5 );

	iter = sa.erase( sa.begin() );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, EraseRange )
{
	rftl::stretch_vector<Proxy, 4> sa;
	sa.push_back( 2 );
	sa.push_back( 5 );
	sa.push_back( 7 );
	sa.push_back( 11 );
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );
	ASSERT_TRUE( sa.data()[2] == 7 );
	ASSERT_TRUE( sa.data()[3] == 11 );

	rftl::stretch_vector<Proxy, 4>::iterator iter;

	iter = sa.erase( sa.begin() + 1, sa.begin() + 3 );
	ASSERT_TRUE( iter == sa.begin() + 1 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 11 );

	iter = sa.erase( sa.begin() + 1, sa.end() );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == 2 );

	iter = sa.erase( sa.begin(), sa.begin() );
	ASSERT_TRUE( iter == sa.begin() );
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == 2 );

	iter = sa.erase( sa.begin(), sa.end() );
	ASSERT_TRUE( iter == sa.end() );
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, BackInsertCompat )
{
	rftl::stretch_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	rftl::back_insert_iterator<rftl::stretch_vector<Proxy, 5>> inserter( sa );
	*inserter = 2;
	*inserter = 7;
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StretchVector, RangeForCompatibility )
{
	rftl::stretch_vector<Proxy, 5> sa;
	rftl::stretch_vector<Proxy, 5> const& csa = sa;
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( 2 );
	sa.push_back( 7 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	for( Proxy const& elem : sa )
	{
		ASSERT_TRUE( elem == 2 || elem == 7 );
	}
	for( Proxy const& elem : csa )
	{
		ASSERT_TRUE( elem == 2 || elem == 7 );
	}
	sa.clear();
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StretchVector, BasicStretch )
{
	rftl::stretch_vector<Proxy, 2> sa;
	rftl::stretch_vector<Proxy, 2> const& csa = sa;
	ASSERT_TRUE( csa.capacity() == 2 );
	ASSERT_TRUE( sa.size() == 0 );
	ASSERT_TRUE( csa.size() == 0 );
	sa.push_back( 2 );
	sa.push_back( 7 );
	ASSERT_TRUE( csa.capacity() == 2 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( csa.size() == 2 );
	ASSERT_TRUE( sa.at( 0 ) == 2 );
	ASSERT_TRUE( csa.at( 0 ) == 2 );
	ASSERT_TRUE( sa[0] == 2 );
	ASSERT_TRUE( csa[0] == 2 );
	ASSERT_TRUE( sa.at( 1 ) == 7 );
	ASSERT_TRUE( csa.at( 1 ) == 7 );
	ASSERT_TRUE( sa[1] == 7 );
	ASSERT_TRUE( csa[1] == 7 );
	sa.push_back( 9 );
	ASSERT_TRUE( csa.capacity() > 2 );
	ASSERT_TRUE( sa.size() == 3 );
	ASSERT_TRUE( csa.size() == 3 );
	ASSERT_TRUE( sa.at( 0 ) == 2 );
	ASSERT_TRUE( csa.at( 0 ) == 2 );
	ASSERT_TRUE( sa[0] == 2 );
	ASSERT_TRUE( csa[0] == 2 );
	ASSERT_TRUE( sa.at( 1 ) == 7 );
	ASSERT_TRUE( csa.at( 1 ) == 7 );
	ASSERT_TRUE( sa[1] == 7 );
	ASSERT_TRUE( csa[1] == 7 );
	ASSERT_TRUE( sa.at( 2 ) == 9 );
	ASSERT_TRUE( csa.at( 2 ) == 9 );
	ASSERT_TRUE( sa[2] == 9 );
	ASSERT_TRUE( csa[2] == 9 );
	sa.clear();
	ASSERT_TRUE( csa.size() == 0 );
}



TEST( StretchVector, StretchVariants )
{
	using Vec = rftl::stretch_vector<Proxy, 1>;
	{
		Vec v( 2 );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == Proxy::kDefault );
		ASSERT_TRUE( v.at( 1 ) == Proxy::kDefault );
	}
	{
		Vec v( 2, Proxy( 5 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 5 );
		ASSERT_TRUE( v.at( 1 ) == 5 );
	}
	{
		Proxy arr[] = { 2, 7 };
		Vec v( &arr[0], &arr[2] );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v( { 2, 7 } );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		v.assign( 2, Proxy( 5 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 5 );
		ASSERT_TRUE( v.at( 1 ) == 5 );
	}
	{
		Proxy arr[] = { 2, 7 };
		Vec v;
		v.assign( &arr[0], &arr[2] );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		v.assign( { 2, 7 } );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		Proxy const a2 = 2;
		Proxy const a7 = 7;
		v.push_back( a2 );
		v.push_back( a7 );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		Proxy a2 = 2;
		Proxy a7 = 7;
		v.push_back( rftl::move( a2 ) );
		v.push_back( rftl::move( a7 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		Proxy a2 = 2;
		Proxy a7 = 7;
		v.emplace_back( rftl::move( a2 ) );
		v.emplace_back( rftl::move( a7 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		v.emplace_back( 2 );
		v.emplace_back( 7 );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		Proxy const a2 = 2;
		Proxy const a7 = 7;
		v.insert( v.end(), a2 );
		v.insert( v.end(), a7 );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		Proxy a2 = 2;
		Proxy a7 = 7;
		v.insert( v.end(), rftl::move( a2 ) );
		v.insert( v.end(), rftl::move( a7 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 2 );
		ASSERT_TRUE( v.at( 1 ) == 7 );
	}
	{
		Vec v;
		v.resize( 2 );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == Proxy::kDefault );
		ASSERT_TRUE( v.at( 1 ) == Proxy::kDefault );
	}
	{
		Vec v;
		v.resize( 2, Proxy( 5 ) );
		ASSERT_TRUE( v.capacity() > 1 );
		ASSERT_TRUE( v.size() == 2 );
		ASSERT_TRUE( v.at( 0 ) == 5 );
		ASSERT_TRUE( v.at( 1 ) == 5 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

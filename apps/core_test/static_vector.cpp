#include "stdafx.h"

#include "rftl/extension/static_vector.h"


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

	operator int() const
	{
		return val;
	}

	int val;
};



TEST( StaticVector, Alignment )
{
	// This adds padding
	static_assert(
		sizeof( rftl::static_vector<uint16_t, 5> ) < sizeof( rftl::static_vector<uint16_t, 5, alignof( uint32_t )> ),
		"Stricter alignment doesn't appear to have had an effect" );

	// This has no effect, since the backing array is already free of padding
	static_assert(
		sizeof( rftl::static_vector<uint16_t, 5> ) == sizeof( rftl::static_vector<uint16_t, 5, alignof( uint8_t )> ),
		"Meaninglessly loose alignment somehow affected size" );
}



TEST( StaticVector, BasicEmpty )
{
	rftl::static_vector<Proxy, 5> empty;
	static_assert( rftl::static_vector<Proxy, 5>::fixed_capacity == 5 );
	ASSERT_TRUE( empty.capacity() == 5 );
	ASSERT_TRUE( empty.max_size() == 5 );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
}



TEST( StaticVector, NopSizeChanges )
{
	rftl::static_vector<Proxy, 5> empty;
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



TEST( StaticVector, BasicLifetime )
{
	rftl::static_vector<Proxy, 5> sa;
	rftl::static_vector<Proxy, 5> const& csa = sa;
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



TEST( StaticVector, BasicBookendAccess )
{
	rftl::static_vector<Proxy, 5> sa;
	rftl::static_vector<Proxy, 5> const& csa = sa;
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



TEST( StaticVector, BasicDataAccess )
{
	rftl::static_vector<Proxy, 5> sa;
	rftl::static_vector<Proxy, 5> const& csa = sa;
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



TEST( StaticVector, BasicIteratorAccess )
{
	rftl::static_vector<Proxy, 5> sa;
	rftl::static_vector<Proxy, 5> const& csa = sa;
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



TEST( StaticVector, BlindEmplaceGrow )
{
	rftl::static_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	sa.emplace_back();
	ASSERT_TRUE( sa.size() == 1 );
	ASSERT_TRUE( sa.data()[0] == Proxy::kDefault );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, InitializerListConstruct )
{
	rftl::static_vector<Proxy, 5> sa = { 2, 7 };
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, IterConstruct )
{
	Proxy const source[] = { 2, 7 };
	rftl::static_vector<Proxy, 5> sa( &source[0], &source[2] );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, CopyConstruct )
{
	rftl::static_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::static_vector<Proxy, 5> const& input = source;

	rftl::static_vector<Proxy, 5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, CopyConstructCrossCapacity )
{
	rftl::static_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::static_vector<Proxy, 5> const& input = source;

	rftl::static_vector<Proxy, 5> sa( input );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, MoveConstruct )
{
	rftl::static_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::static_vector<Proxy, 5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, MoveConstructCrossCapacity )
{
	rftl::static_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::static_vector<Proxy, 5> sa( rftl::move( source ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, SizeConstruct )
{
	rftl::static_vector<Proxy, 5> sa( 2 );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == Proxy::kDefault );
	ASSERT_TRUE( sa.data()[1] == Proxy::kDefault );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, ValueConstruct )
{
	// At time of writing 'sa(2,2)' is unclear, and templatization into
	//  iterators takes precedence before the implicit constructor of the
	//  proxy. This would be solved by a harder iterator type, but seems
	//  not worth the effort at this time, since users can clarify their
	//  implicit call with minimal effort.
	//rftl::static_vector<Proxy, 5> sa( 2, 2 ); // Similar case on wipe test
	rftl::static_vector<Proxy, 5> sa( 2, Proxy( 2 ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 2 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, CopyAssign )
{
	rftl::static_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::static_vector<Proxy, 5> const& input = source;

	rftl::static_vector<Proxy, 5> sa;
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



TEST( StaticVector, CopyAssignCrossCapacity )
{
	rftl::static_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );
	rftl::static_vector<Proxy, 5> const& input = source;

	rftl::static_vector<Proxy, 5> sa;
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



TEST( StaticVector, MoveAssign )
{
	rftl::static_vector<Proxy, 5> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::static_vector<Proxy, 5> sa;
	sa = rftl::move( source );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, MoveAssignCrossCapacity )
{
	rftl::static_vector<Proxy, 2> source;
	source.push_back( 2 );
	source.push_back( 7 );
	ASSERT_TRUE( source.size() == 2 );
	ASSERT_TRUE( source.data()[0] == 2 );
	ASSERT_TRUE( source.data()[1] == 7 );

	rftl::static_vector<Proxy, 5> sa;
	sa = rftl::move( source );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	ASSERT_TRUE( source.size() == 0 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, ValueWipe )
{
	rftl::static_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	//sa.assign( 2, 2 ); // See notes on construct test
	sa.assign( 2, Proxy( 2 ) );
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 2 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, Insert )
{
	rftl::static_vector<Proxy, 3> sa;
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



TEST( StaticVector, Erase )
{
	rftl::static_vector<Proxy, 4> sa;
	sa.push_back( 2 );
	sa.push_back( 5 );
	sa.push_back( 7 );
	sa.push_back( 11 );
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );
	ASSERT_TRUE( sa.data()[2] == 7 );
	ASSERT_TRUE( sa.data()[3] == 11 );

	rftl::static_vector<Proxy, 4>::iterator iter;

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



TEST( StaticVector, EraseRange )
{
	rftl::static_vector<Proxy, 4> sa;
	sa.push_back( 2 );
	sa.push_back( 5 );
	sa.push_back( 7 );
	sa.push_back( 11 );
	ASSERT_TRUE( sa.size() == 4 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 5 );
	ASSERT_TRUE( sa.data()[2] == 7 );
	ASSERT_TRUE( sa.data()[3] == 11 );

	rftl::static_vector<Proxy, 4>::iterator iter;

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



TEST( StaticVector, BackInsertCompat )
{
	rftl::static_vector<Proxy, 5> sa;
	ASSERT_TRUE( sa.size() == 0 );
	rftl::back_insert_iterator<rftl::static_vector<Proxy, 5>> inserter( sa );
	*inserter = 2;
	*inserter = 7;
	ASSERT_TRUE( sa.size() == 2 );
	ASSERT_TRUE( sa.data()[0] == 2 );
	ASSERT_TRUE( sa.data()[1] == 7 );
	sa.clear();
	ASSERT_TRUE( sa.size() == 0 );
}



TEST( StaticVector, RangeForCompatibility )
{
	rftl::static_vector<Proxy, 5> sa;
	rftl::static_vector<Proxy, 5> const& csa = sa;
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

///////////////////////////////////////////////////////////////////////////////
}

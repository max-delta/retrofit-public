#include "stdafx.h"

#include "rftl/extension/byte_view.h"

#include "rftl/array"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( ByteView, BasicEmpty )
{
	rftl::byte_view const empty = {};
	ASSERT_TRUE( empty.data() == nullptr );
	ASSERT_TRUE( empty.begin() == empty.data() );
	ASSERT_TRUE( empty.begin() == empty.cbegin() );
	ASSERT_TRUE( empty.begin() == empty.end() );
	ASSERT_TRUE( empty.begin() == empty.cend() );
	ASSERT_TRUE( empty.rbegin() == empty.crbegin() );
	ASSERT_TRUE( empty.rbegin() == empty.rend() );
	ASSERT_TRUE( empty.rbegin() == empty.crend() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	ASSERT_TRUE( empty.size() == empty.length() );
	static_assert( empty.max_size() == rftl::numeric_limits<size_t>::max() );
}



TEST( ByteView, DirtyEmpty )
{
	rftl::byte_view const empty( reinterpret_cast<void const*>( 0x1 ), reinterpret_cast<void const*>( 0x1 ) );
	ASSERT_TRUE( empty.data() == reinterpret_cast<void const*>( 0x1 ) );
	ASSERT_TRUE( empty.begin() == empty.data() );
	ASSERT_TRUE( empty.begin() == empty.cbegin() );
	ASSERT_TRUE( empty.begin() == empty.end() );
	ASSERT_TRUE( empty.begin() == empty.cend() );
	ASSERT_TRUE( empty.rbegin() == empty.crbegin() );
	ASSERT_TRUE( empty.rbegin() == empty.rend() );
	ASSERT_TRUE( empty.rbegin() == empty.crend() );
	ASSERT_TRUE( empty.empty() );
	ASSERT_TRUE( empty.size() == 0 );
	ASSERT_TRUE( empty.size() == empty.length() );
	static_assert( empty.max_size() == rftl::numeric_limits<size_t>::max() );
}



TEST( ByteView, InvalidMemory )
{
	rftl::byte_view const invalid( reinterpret_cast<void const*>( 5 ), reinterpret_cast<void const*>( 10 ) );
	ASSERT_TRUE( invalid.data() == reinterpret_cast<void const*>( 5 ) );
	ASSERT_TRUE( invalid.begin() == invalid.data() );
	ASSERT_TRUE( invalid.begin() == invalid.cbegin() );
	ASSERT_TRUE( invalid.begin() < invalid.end() );
	ASSERT_TRUE( invalid.begin() < invalid.cend() );
	ASSERT_TRUE( invalid.rbegin() == invalid.crbegin() );
	ASSERT_TRUE( invalid.rbegin() < invalid.rend() );
	ASSERT_TRUE( invalid.rbegin() < invalid.crend() );
	ASSERT_TRUE( invalid.empty() == false );
	ASSERT_TRUE( invalid.size() == 5 );
	ASSERT_TRUE( invalid.size() == invalid.length() );
	static_assert( invalid.max_size() == rftl::numeric_limits<size_t>::max() );

	ASSERT_TRUE( invalid.begin() == &invalid.at<uint8_t>( 0 ) );
	ASSERT_TRUE( invalid.begin() == &invalid.operator[]<uint8_t>( 0 ) );
	ASSERT_TRUE( invalid.end() == &invalid.at<uint8_t>( 4 ) + 1 );
	ASSERT_TRUE( invalid.end() == &invalid.operator[]<uint8_t>( 4 ) + 1 );
}



TEST( ByteView, BasicAccess )
{
	uint16_t const data[3] = { 3, 5, 7 };
	rftl::byte_view const view( data, sizeof( data ) );
	ASSERT_TRUE( view.at<uint16_t>( 0 ) == 3 );
	ASSERT_TRUE( view.at<uint16_t>( 2 ) == 5 );
	ASSERT_TRUE( view.at<uint16_t>( 4 ) == 7 );
}



TEST( ByteView, Compare )
{
	uint16_t const data1[3] = { 3, 5, 7 };
	uint16_t const data2[3] = { 3, 5, 7 };
	uint16_t const data3[4] = { 3, 5, 9 };
	uint16_t const data4[4] = { 3, 5, 7, 0 };
	rftl::byte_view view1( data1, sizeof( data1 ) );
	rftl::byte_view view2( data2, sizeof( data2 ) );
	rftl::byte_view view3( data3, sizeof( data3 ) );
	rftl::byte_view view4( data4, sizeof( data4 ) );
	ASSERT_TRUE( view1 == view1 );
	ASSERT_TRUE( view1 == view2 );
	ASSERT_TRUE( view1 != view3 );
	ASSERT_TRUE( view1 != view4 );
}



TEST( ByteView, Constructors )
{
	// Empty
	ASSERT_TRUE( rftl::byte_view() == rftl::byte_view( nullptr, 0 ) );
	ASSERT_TRUE( rftl::byte_view() == rftl::byte_view( reinterpret_cast<void*>( 0x1 ), 0 ) );
	ASSERT_TRUE( rftl::byte_view() == rftl::byte_view( nullptr, nullptr ) );
	ASSERT_TRUE( rftl::byte_view() == rftl::byte_view( reinterpret_cast<void*>( 0x1 ), reinterpret_cast<void*>( 0x1 ) ) );

	// Data
	uint16_t const data[3] = { 3, 5, 7 };
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data ) ) == rftl::byte_view( data, data + 3 ) );

	// Single object
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data[0] ) ) == rftl::byte_view( data ) );
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data[0] ) ) == rftl::byte_view( &data[0] ) );

	// Constexpr
	static constexpr uint16_t kData[3] = { 3, 5, 7 };
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data ) ) == rftl::byte_view( kData, sizeof( kData ) ) );

	// Complex iterators
	rftl::array<uint16_t, 3> arr = { 3, 5, 7 };
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data ) ) == rftl::byte_view( arr.data(), arr.size() * sizeof( arr[0] ) ) );
	ASSERT_TRUE( rftl::byte_view( data, sizeof( data ) ) == rftl::byte_view( arr.begin(), arr.end() ) );
	ASSERT_TRUE( rftl::byte_view( arr.data(), sizeof( arr[0] ) ) == rftl::byte_view( &arr[0] ) );

	// Copy
	rftl::byte_view const ref( data, sizeof( data ) );
	rftl::byte_view copy( ref );
	ASSERT_TRUE( ref == copy );

	// Move
	rftl::byte_view const move( rftl::move( copy ) );
	ASSERT_TRUE( ref == move );
}



TEST( ByteView, Shrink )
{
	uint16_t const data[3] = { 3, 5, 7 };
	rftl::byte_view view( data, sizeof( data ) );
	view.remove_prefix( 2 );
	ASSERT_TRUE( view.size() == 4 );
	ASSERT_TRUE( view.at<uint16_t>( 0 ) == 5 );
	ASSERT_TRUE( view.at<uint16_t>( 2 ) == 7 );
	view.remove_suffix( 2 );
	ASSERT_TRUE( view.size() == 2 );
	ASSERT_TRUE( view.at<uint16_t>( 0 ) == 5 );
}



TEST( ByteView, Substr )
{
	uint16_t const data[3] = { 3, 5, 7 };
	rftl::byte_view view( data, sizeof( data ) );
	view = view.substr( 2 );
	ASSERT_TRUE( view.size() == 4 );
	ASSERT_TRUE( view.at<uint16_t>( 0 ) == 5 );
	ASSERT_TRUE( view.at<uint16_t>( 2 ) == 7 );
	view = view.substr( 0, 2 );
	ASSERT_TRUE( view.size() == 2 );
	ASSERT_TRUE( view.at<uint16_t>( 0 ) == 5 );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "Structure.h"

#include "core_math/BitField.h"
#include "core_math/math_casts.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static uint8_t* Grow( Buffer& bytes, size_t additional )
{
	// NOTE: Resize may re-allocate, use pointer post-grow
	bytes.resize( bytes.size() + additional );
	return &( bytes.at( bytes.size() - additional ) );
}

}
///////////////////////////////////////////////////////////////////////////////

void CommonHeader::Append( Buffer& bytes ) const
{
	uint8_t* const dest = details::Grow( bytes, 8 );
	rftl::memcpy( dest, kMagic1, 4 );
	rftl::memcpy( dest + 4, kMagic2, 4 );
}



ReadResult CommonHeader::TryRead( rftl::byte_view& bytes )
{
	if( bytes.size() < 8 )
	{
		return ReadResult::kTooSmall;
	}

	if( rftl::memcmp( &bytes.at<char>( 0 ), kMagic1, 4 ) != 0 )
	{
		return ReadResult::kWrongProtocol;
	}

	if( rftl::memcmp( &bytes.at<char>( 4 ), kMagic2, 4 ) != 0 )
	{
		return ReadResult::kWrongProtocol;
	}

	bytes.remove_prefix( 8 );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MessageBatch::Append( Buffer& bytes ) const
{
	using Rep = math::BitField<16, 16, 4>;
	Rep rep = {};
	rep.WriteAt<0>( math::integer_cast<uint16_t>( mTotalBytes ) );
	rep.WriteAt<1>( math::integer_cast<uint16_t>( mBatchBytes ) );
	rep.WriteAt<2>( math::enum_bitcast( mEncryption ) );

	uint8_t* const dest = details::Grow( bytes, sizeof( Rep ) );
	memcpy( dest, rep.Data(), sizeof( Rep ) );
}



ReadResult MessageBatch::TryRead( rftl::byte_view& bytes )
{
	using Rep = math::BitField<16, 16, 4>;

	if( bytes.size() < sizeof( Rep ) )
	{
		return ReadResult::kTooSmall;
	}

	Rep const* const rep = reinterpret_cast<Rep const*>( bytes.data() );
	mTotalBytes = rep->ReadAt<0, size_t>();
	mBatchBytes = rep->ReadAt<1, size_t>();
	mEncryption = math::enum_bitcast<EncryptionMode>( rep->ReadAt<2, uint8_t>() );

	bytes.remove_prefix( sizeof( Rep ) );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MessageIdentifier::Append( Buffer& bytes ) const
{
	uint8_t* const dest = details::Grow( bytes, 4 );
	rftl::memcpy( dest, mID.data(), 4 );
}



ReadResult MessageIdentifier::TryRead( rftl::byte_view& bytes )
{
	if( bytes.size() < 4 )
	{
		return ReadResult::kTooSmall;
	}

	rftl::memcpy( mID.data(), bytes.data(), 4 );

	bytes.remove_prefix( 4 );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////
}

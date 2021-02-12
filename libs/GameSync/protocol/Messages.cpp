#include "stdafx.h"
#include "Messages.h"

#include "core_math/math_bits.h"


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

void MsgHello::Append( Buffer& bytes ) const
{
	uint8_t* const dest = details::Grow( bytes, 1 );
	dest[0] = math::enum_bitcast( mModeRequest );
}



ReadResult MsgHello::TryRead( rftl::byte_view& bytes )
{
	if( bytes.size() < 1 )
	{
		return ReadResult::kTooSmall;
	}

	mModeRequest = bytes.at<EncryptionMode>( 0 );

	bytes.remove_prefix( 1 );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MsgWelcome::Append( Buffer& bytes ) const
{
	uint8_t* const dest = details::Grow( bytes, 1 );
	dest[0] = math::enum_bitcast( mModeChange );
}



ReadResult MsgWelcome::TryRead( rftl::byte_view& bytes )
{
	if( bytes.size() < 1 )
	{
		return ReadResult::kTooSmall;
	}

	mModeChange = bytes.at<EncryptionMode>( 0 );

	bytes.remove_prefix( 1 );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////
}

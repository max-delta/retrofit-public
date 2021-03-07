#include "stdafx.h"
#include "Messages.h"

#include "core_math/BitField.h"
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

void MsgSessionList::Append( Buffer& bytes ) const
{
	using IDRep = math::BitField<uint64_t, 64>;
	using EntryRep = math::BitField<uint64_t, 64, 8>;

	uint8_t const numEntries = math::integer_cast<uint8_t>( mConnectionEntries.size() );
	bytes.reserve( sizeof( IDRep ) + 1 + sizeof( EntryRep ) * numEntries );

	// ID
	IDRep idRep = {};
	idRep.WriteAt<0>( mYourConnectionID );
	uint8_t* const id = details::Grow( bytes, sizeof( IDRep ) );
	memcpy( id, idRep.Data(), sizeof( IDRep ) );

	// Count
	uint8_t* const count = details::Grow( bytes, 1 );
	*count = numEntries;

	// Entries
	for( ConnectionEntry const& entry : mConnectionEntries )
	{
		EntryRep entryRep = {};
		entryRep.WriteAt<0>( entry.first );
		entryRep.WriteAt<1>( entry.second );
		uint8_t* const dest = details::Grow( bytes, sizeof( EntryRep ) );
		memcpy( dest, entryRep.Data(), sizeof( EntryRep ) );
	}
}



ReadResult MsgSessionList::TryRead( rftl::byte_view& bytes )
{
	using IDRep = math::BitField<uint64_t, 64>;
	using EntryRep = math::BitField<uint64_t, 64, 8>;

	if( bytes.size() < sizeof( IDRep ) + 1 )
	{
		return ReadResult::kTooSmall;
	}

	// ID
	IDRep const* const idRep = reinterpret_cast<IDRep const*>( bytes.data() );
	mYourConnectionID = idRep->ReadAt<0>();
	bytes.remove_prefix( sizeof( IDRep ) );

	// Count
	uint8_t const count = bytes.at<uint8_t>( 0 );
	bytes.remove_prefix( 1 );

	if( bytes.size() < count * sizeof( EntryRep ) )
	{
		return ReadResult::kTooSmall;
	}

	// Entries
	mConnectionEntries.reserve( count );
	for( size_t i = 0; i < count; i++ )
	{
		ConnectionEntry entry = {};
		EntryRep const* const entryRep = reinterpret_cast<EntryRep const*>( bytes.data() );
		entry.first = entryRep->ReadAt<0>();
		entry.second = math::integer_cast<input::PlayerID>( entryRep->ReadAt<1>() );
		mConnectionEntries.emplace_back( rftl::move( entry ) );
		bytes.remove_prefix( sizeof( EntryRep ) );
	}

	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MsgClaimPlayer::Append( Buffer& bytes ) const
{
	using Rep = math::BitField<uint8_t, 8, 1>;
	Rep rep = {};
	rep.WriteAt<0>( mPlayerID );
	rep.WriteAt<1>( mClaim ? 1u : 0u );

	uint8_t* const dest = details::Grow( bytes, sizeof( Rep ) );
	memcpy( dest, rep.Data(), sizeof( Rep ) );
}



ReadResult MsgClaimPlayer::TryRead( rftl::byte_view& bytes )
{
	using Rep = math::BitField<uint8_t, 8, 1>;

	if( bytes.size() < sizeof( Rep ) )
	{
		return ReadResult::kTooSmall;
	}

	Rep const* const rep = reinterpret_cast<Rep const*>( bytes.data() );
	mPlayerID = rep->ReadAt<0>();
	mClaim = rep->ReadAt<1>() != 0;

	bytes.remove_prefix( sizeof( Rep ) );
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MsgChat::Append( Buffer& bytes ) const
{
	// Count
	uint8_t* const count = details::Grow( bytes, 1 );
	*count = math::integer_cast<uint8_t>( mText.size() );

	// Chars
	uint8_t* const chars = details::Grow( bytes, mText.size() );
	memcpy( chars, mText.data(), mText.size() );
}



ReadResult MsgChat::TryRead( rftl::byte_view& bytes )
{
	if( bytes.size() < sizeof( uint8_t ) + 1 )
	{
		return ReadResult::kTooSmall;
	}

	// Count
	uint8_t const count = bytes.at<uint8_t>( 0 );
	bytes.remove_prefix( 1 );
	if( count == 0 )
	{
		return ReadResult::kLogicError;
	}

	if( bytes.size() < count )
	{
		return ReadResult::kTooSmall;
	}

	// Chars
	mText.clear();
	mText.resize( count, '?' );
	memcpy( mText.data(), &bytes.at<char>( 0 ), count );
	bytes.remove_prefix( count );

	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Msg>
void MsgProxyT<Msg>::Append( Buffer& bytes ) const
{
	using IDRep = math::BitField<uint64_t, 64>;

	// ID
	IDRep idRep = {};
	idRep.WriteAt<0>( mSourceConnectionID );
	uint8_t* const id = details::Grow( bytes, sizeof( IDRep ) );
	memcpy( id, idRep.Data(), sizeof( IDRep ) );

	// Msg
	mMsg.Append( bytes );
}



template<typename Msg>
ReadResult MsgProxyT<Msg>::TryRead( rftl::byte_view& bytes )
{
	using IDRep = math::BitField<uint64_t, 64>;

	if( bytes.size() < sizeof( IDRep ) )
	{
		return ReadResult::kTooSmall;
	}

	// ID
	IDRep const* const idRep = reinterpret_cast<IDRep const*>( bytes.data() );
	mSourceConnectionID = idRep->ReadAt<0>();
	bytes.remove_prefix( sizeof( IDRep ) );

	// Msg
	return mMsg.TryRead( bytes );
}



template struct GAMESYNC_API MsgProxyT<MsgChat>;

///////////////////////////////////////////////////////////////////////////////
}

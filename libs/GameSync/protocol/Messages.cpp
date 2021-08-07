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
	using IDRep = math::BitField<64>;
	using EntryRep = math::BitField<64, 8>;

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
	using IDRep = math::BitField<64>;
	using EntryRep = math::BitField<64, 8>;

	if( bytes.size() < sizeof( IDRep ) + 1 )
	{
		return ReadResult::kTooSmall;
	}

	// ID
	IDRep const* const idRep = reinterpret_cast<IDRep const*>( bytes.data() );
	mYourConnectionID = idRep->ReadAt<0, ConnectionIdentifier>();
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
		entry.first = entryRep->ReadAt<0, ConnectionIdentifier>();
		entry.second = entryRep->ReadAt<1, input::PlayerID>();
		mConnectionEntries.emplace_back( rftl::move( entry ) );
		bytes.remove_prefix( sizeof( EntryRep ) );
	}

	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MsgClaimPlayer::Append( Buffer& bytes ) const
{
	using Rep = math::BitField<8, 1>;
	Rep rep = {};
	rep.WriteAt<0>( mPlayerID );
	rep.WriteAt<1>( mClaim ? 1u : 0u );

	uint8_t* const dest = details::Grow( bytes, sizeof( Rep ) );
	memcpy( dest, rep.Data(), sizeof( Rep ) );
}



ReadResult MsgClaimPlayer::TryRead( rftl::byte_view& bytes )
{
	using Rep = math::BitField<8, 1>;

	if( bytes.size() < sizeof( Rep ) )
	{
		return ReadResult::kTooSmall;
	}

	Rep const* const rep = reinterpret_cast<Rep const*>( bytes.data() );
	mPlayerID = rep->ReadAt<0, input::PlayerID>();
	mClaim = rep->ReadAt<1, uint8_t>() != 0;

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

void MsgRollbackInputEvents::Append( Buffer& bytes ) const
{
	using CommitRep = math::BitField<64>;
	using StreamIDRep = math::BitField<8>;
	using EventRep = math::BitField<64, 8>;

	// Commit frame
	CommitRep commitRep = {};
	commitRep.WriteAt<0>( mInputPack.mFrameReadyToCommit.time_since_epoch().count() );
	uint8_t* const commit = details::Grow( bytes, sizeof( CommitRep ) );
	memcpy( commit, commitRep.Data(), sizeof( CommitRep ) );

	// Stream count
	uint8_t* const streamCount = details::Grow( bytes, 1 );
	*streamCount = math::integer_cast<uint8_t>( mInputPack.mStreams.size() );

	// Streams
	for( RollbackInputPack::Streams::value_type const& stream : mInputPack.mStreams )
	{
		// Stream ID
		StreamIDRep streamIDRep = {};
		streamIDRep.WriteAt<0>( stream.first );
		uint8_t* const streamDest = details::Grow( bytes, sizeof( StreamIDRep ) );
		memcpy( streamDest, streamIDRep.Data(), sizeof( StreamIDRep ) );

		RollbackInputPack::Events const& events = stream.second;

		// Event count
		uint8_t* const eventCount = details::Grow( bytes, 1 );
		*eventCount = math::integer_cast<uint8_t>( events.size() );

		// Events
		for( rollback::InputEvent const& event : events )
		{
			EventRep eventRep = {};
			eventRep.WriteAt<0>( event.mTime.time_since_epoch().count() );
			eventRep.WriteAt<1>( event.mValue );
			uint8_t* const eventDest = details::Grow( bytes, sizeof( EventRep ) );
			memcpy( eventDest, eventRep.Data(), sizeof( EventRep ) );
		}
	}
}



ReadResult MsgRollbackInputEvents::TryRead( rftl::byte_view& bytes )
{
	using CommitRep = math::BitField<64>;
	using StreamIDRep = math::BitField<8>;
	using EventRep = math::BitField<64, 8>;

	// Commit frame
	if( bytes.size() < sizeof( CommitRep ) )
	{
		return ReadResult::kTooSmall;
	}
	CommitRep const* const commitRep = reinterpret_cast<CommitRep const*>( bytes.data() );
	mInputPack.mFrameReadyToCommit = time::CommonClock::TimePointFromNanos( commitRep->ReadAt<0, int64_t>() );
	bytes.remove_prefix( sizeof( CommitRep ) );

	// Stream count
	if( bytes.size() < 1 )
	{
		return ReadResult::kTooSmall;
	}
	uint8_t const streamCount = bytes.at<uint8_t>( 0 );
	bytes.remove_prefix( 1 );

	// Streams
	for( size_t i_stream = 0; i_stream < streamCount; i_stream++ )
	{
		// Stream ID
		if( bytes.size() < sizeof( StreamIDRep ) )
		{
			return ReadResult::kTooSmall;
		}
		StreamIDRep const* const streamIDRep = reinterpret_cast<StreamIDRep const*>( bytes.data() );
		rollback::InputStreamIdentifier const streamID = streamIDRep->ReadAt<0, rollback::InputStreamIdentifier>();
		bytes.remove_prefix( sizeof( StreamIDRep ) );

		RollbackInputPack::Events& events = mInputPack.mStreams[streamID];

		// Event count
		if( bytes.size() < 1 )
		{
			return ReadResult::kTooSmall;
		}
		uint8_t const eventCount = bytes.at<uint8_t>( 0 );
		bytes.remove_prefix( 1 );

		// Events
		if( bytes.size() < eventCount * sizeof( EventRep ) )
		{
			return ReadResult::kTooSmall;
		}
		events.reserve( eventCount );
		for( size_t i_event = 0; i_event < eventCount; i_event++ )
		{
			rollback::InputEvent event = {};
			EventRep const* const entryRep = reinterpret_cast<EventRep const*>( bytes.data() );
			event.mTime = time::CommonClock::TimePointFromNanos( entryRep->ReadAt<0, int64_t>() );
			event.mValue = entryRep->ReadAt<1, rollback::InputValue>();
			events.emplace_back( rftl::move( event ) );
			bytes.remove_prefix( sizeof( EventRep ) );
		}
	}

	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

template<typename Msg>
void MsgProxyT<Msg>::Append( Buffer& bytes ) const
{
	using IDRep = math::BitField<64>;

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
	using IDRep = math::BitField<64>;

	if( bytes.size() < sizeof( IDRep ) )
	{
		return ReadResult::kTooSmall;
	}

	// ID
	IDRep const* const idRep = reinterpret_cast<IDRep const*>( bytes.data() );
	mSourceConnectionID = idRep->ReadAt<0, ConnectionIdentifier>();
	bytes.remove_prefix( sizeof( IDRep ) );

	// Msg
	return mMsg.TryRead( bytes );
}



template struct GAMESYNC_API MsgProxyT<MsgChat>;
template struct GAMESYNC_API MsgProxyT<MsgRollbackInputEvents>;

///////////////////////////////////////////////////////////////////////////////
}

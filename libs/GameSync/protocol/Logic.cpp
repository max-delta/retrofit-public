#include "stdafx.h"
#include "Logic.h"

#include "GameSync/protocol/Encryption.h"
#include "GameSync/protocol/Messages.h"
#include "GameSync/SessionMembers.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"

#include "rftl/algorithm"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////
namespace details {

size_t CalcStandardTransmissionHeaderSize()
{
	Buffer temp;
	CommonHeader{}.Append( temp );
	MessageBatch{}.Append( temp );
	return temp.size();
}



static uint8_t* Grow( Buffer& bytes, size_t additional )
{
	// NOTE: Resize may re-allocate, use pointer post-grow
	bytes.resize( bytes.size() + additional );
	return &( bytes.at( bytes.size() - additional ) );
}

}
///////////////////////////////////////////////////////////////////////////////

ReadResult TryDecodeBatch(
	rftl::byte_view bytes,
	EncryptionState const& encryption,
	OnMessageFunc const& onMessage )
{
	RF_ASSERT( bytes.empty() == false );
	RF_ASSERT( onMessage != nullptr );

	// If encrypted, decrypt into a new buffer and switch to that
	Buffer encryptionBuffer;
	if( encryption.mMode != EncryptionMode::kUnencrypted )
	{
		encryptionBuffer.assign(
			reinterpret_cast<uint8_t const*>( bytes.begin() ),
			reinterpret_cast<uint8_t const*>( bytes.end() ) );
		bool const decrypted = DecryptBytes( encryptionBuffer.begin(), encryptionBuffer.end(), encryption );
		if( decrypted == false )
		{
			return ReadResult::kBadEncryption;
		}
		bytes = rftl::byte_view( encryptionBuffer.begin(), encryptionBuffer.end() );
	}

	// While there's still messages to decode...
	// NOTE: Parsing the ID will always take off some bytes, and has a min size
	//  requirement, so even if the data is corrupt this loop should never hang
	//  or leave trailing data
	while( bytes.empty() == false )
	{
		// ID
		MessageIdentifier identifier;
		ReadResult const identifierResult = identifier.TryRead( bytes );
		if( identifierResult != ReadResult::kSuccess )
		{
			return identifierResult;
		}

		// Message
		ReadResult const messageResult = onMessage( identifier.mID, bytes );
		if( messageResult != ReadResult::kSuccess )
		{
			return messageResult;
		}
	}

	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

GAMESYNC_API rftl::vector<Buffer> CreateTransmissions(
	Buffer&& messages,
	EncryptionState const& encryption,
	size_t maxTransmissionSize )
{
	RF_ASSERT( messages.empty() == false );

	rftl::vector<Buffer> retVal;

	// Pre-calc sizes
	static size_t const transmissionPrefixSize = details::CalcStandardTransmissionHeaderSize();
	RF_ASSERT( transmissionPrefixSize < maxTransmissionSize );
	size_t const maxMessageBytesPerTransmission = maxTransmissionSize - transmissionPrefixSize;
	RF_ASSERT( maxMessageBytesPerTransmission > 0 );

	// If encryption is needed, encrypt into a new buffer and switch to that
	rftl::byte_view messageBytes( messages.begin(), messages.end() );
	Buffer encryptionBuffer;
	if( encryption.mMode != EncryptionMode::kUnencrypted )
	{
		encryptionBuffer = messages;
		bool const encrypted = EncryptBytes( encryptionBuffer.begin(), encryptionBuffer.end(), encryption );
		if( encrypted == false )
		{
			RF_RETAIL_FATAL_MSG( "Failed to encrypt", "Encryption failed. Corruption of encryption state?" );
		}
		messageBytes = rftl::byte_view( encryptionBuffer.begin(), encryptionBuffer.end() );
	}

	// Consume all message bytes...
	size_t const totalBytes = messageBytes.size();
	while( messageBytes.empty() == false )
	{
		// How much can be fit in this transmission?
		size_t const remainingMessageBytes = messageBytes.size();
		size_t const messageBytesInThisTransmission = math::Min( remainingMessageBytes, maxMessageBytesPerTransmission );

		// Create the transmission buffer
		Buffer transmission;
		transmission.reserve( transmissionPrefixSize + messageBytesInThisTransmission );

		// Write the headers
		CommonHeader{}.Append( transmission );
		MessageBatch batch = {};
		batch.mTotalBytes = totalBytes;
		batch.mBatchBytes = messageBytesInThisTransmission;
		batch.mEncryption = encryption.mMode;
		batch.Append( transmission );
		RF_ASSERT( transmission.size() == transmissionPrefixSize );

		// Write the messages
		transmission.resize( transmission.size() + messageBytesInThisTransmission );
		rftl::memcpy(
			&transmission.at( transmissionPrefixSize ),
			messageBytes.data(),
			messageBytesInThisTransmission );
		messageBytes.remove_prefix( messageBytesInThisTransmission );

		// Store the transmission
		retVal.emplace_back( rftl::move( transmission ) );
	}

	return retVal;
}



ReadResult TryDecodeTransmission(
	rftl::byte_view& bytes,
	EncryptionState const& encryption,
	OnBatchFunc const& onBatch )
{
	RF_ASSERT( onBatch != nullptr );
	rftl::byte_view tempBytes = bytes;

	static constexpr ReadResult kSuccess = ReadResult::kSuccess;

	// Header
	CommonHeader header;
	ReadResult const headerResult = header.TryRead( tempBytes );
	if( headerResult != kSuccess )
	{
		return headerResult;
	}

	// Batch header
	MessageBatch batch;
	ReadResult const batchResult = batch.TryRead( tempBytes );
	if( batchResult != kSuccess )
	{
		return batchResult;
	}
	if( tempBytes.size() < batch.mBatchBytes )
	{
		return ReadResult::kTooSmall;
	}
	if( batch.mEncryption != encryption.mMode )
	{
		return ReadResult::kWrongEncryption;
	}

	// Batch payload
	rftl::byte_view const batchBytes = tempBytes.substr( 0, batch.mBatchBytes );
	ReadResult const payloadResult = onBatch( batch.mTotalBytes, batchBytes );
	if( payloadResult != kSuccess )
	{
		return payloadResult;
	}
	tempBytes.remove_prefix( batchBytes.size() );

	bytes = tempBytes;
	return kSuccess;
}



GAMESYNC_API ReadResult TryDecodeTransmissionsIntoFullBatch(
	rftl::byte_view& bytes,
	EncryptionState const& encryption,
	Buffer& fullBatch )
{
	rftl::byte_view const originalBytes = bytes;
	fullBatch.clear();

	// First attempt a dry-run to see if the bytes form a full batch
	size_t expectedTotalBytes = 0;
	{
		size_t bytesFound = 0;
		auto const onBatch = [&expectedTotalBytes, &bytesFound]( size_t totalBytes, rftl::byte_view bytes ) -> ReadResult //
		{
			if( expectedTotalBytes == 0 )
			{
				// Not set yet
				if( totalBytes == 0 )
				{
					// Empty batch, odd...
					return ReadResult::kSuccess;
				}
				else
				{
					expectedTotalBytes = totalBytes;
				}
			}

			if( expectedTotalBytes != totalBytes )
			{
				// Unexpected batch change
				return ReadResult::kLogicError;
			}

			bytesFound += bytes.size();
			return ReadResult::kSuccess;
		};

		// Read as dry-run
		rftl::byte_view dryRunBytes = originalBytes;
		ReadResult result = TryDecodeTransmission( dryRunBytes, encryption, onBatch );
		while( result == ReadResult::kSuccess && bytesFound < expectedTotalBytes )
		{
			result = TryDecodeTransmission( dryRunBytes, encryption, onBatch );
		}
		if( result != ReadResult::kSuccess )
		{
			return result;
		}
		if( bytesFound < expectedTotalBytes )
		{
			return ReadResult::kTooSmall;
		}
		RF_ASSERT( expectedTotalBytes == bytesFound );
		RF_ASSERT( expectedTotalBytes <= originalBytes.size() );
	}

	// Pull the bytes out
	{
		fullBatch.reserve( expectedTotalBytes );
		auto const onBatch = [&fullBatch]( size_t totalBytes, rftl::byte_view bytes ) -> ReadResult //
		{
			uint8_t* const dest = details::Grow( fullBatch, bytes.size() );
			memcpy( dest, bytes.data(), bytes.size() );
			return ReadResult::kSuccess;
		};

		RF_ASSERT( bytes == originalBytes );
		ReadResult result = TryDecodeTransmission( bytes, encryption, onBatch );
		while( result == ReadResult::kSuccess && fullBatch.size() < expectedTotalBytes )
		{
			result = TryDecodeTransmission( bytes, encryption, onBatch );
		}
		RF_ASSERT( result == ReadResult::kSuccess );
		RF_ASSERT( fullBatch.size() == expectedTotalBytes );
		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

GAMESYNC_API Buffer CreateHelloTransmission(
	size_t maxTransmissionSize,
	EncryptionState const& attemptedEncryption )
{
	// Clients start unencrypted
	RF_ASSERT( attemptedEncryption.mMode == protocol::EncryptionMode::kUnencrypted );

	Buffer messages;
	MessageIdentifier{ MsgHello::kID }.Append( messages );
	MsgHello hello = {};
	hello.mModeRequest = attemptedEncryption.mPending;
	hello.Append( messages );

	rftl::vector<Buffer> transmissions = CreateTransmissions(
		rftl::move( messages ),
		EncryptionState{},
		maxTransmissionSize );
	RF_ASSERT( transmissions.size() == 1 );

	return transmissions.at( 0 );
}



GAMESYNC_API ReadResult TryDecodeHelloTransmission(
	rftl::byte_view& bytes,
	EncryptionState& attemptedEncryption )
{
	// Clients start unencrypted
	RF_ASSERT( attemptedEncryption.mMode == protocol::EncryptionMode::kUnencrypted );

	auto const onMessage = [&attemptedEncryption]( MessageID const& id, rftl::byte_view& bytes ) -> ReadResult //
	{
		if( id == MsgHello::kID )
		{
			MsgHello msg = {};
			ReadResult const result = msg.TryRead( bytes );
			if( result == ReadResult::kSuccess )
			{
				attemptedEncryption.mPending = msg.mModeRequest;
			}
			return result;
		}
		return ReadResult::kLogicError;
	};

	auto const onBatch = [&onMessage]( size_t totalBytes, rftl::byte_view bytes ) -> ReadResult //
	{
		return TryDecodeBatch( bytes, EncryptionState{}, onMessage );
	};

	return TryDecodeTransmission( bytes, EncryptionState{}, onBatch );
}



GAMESYNC_API Buffer CreateWelcomeTransmission(
	size_t maxTransmissionSize,
	EncryptionState const& attemptedEncryption )
{
	// Hosts respond unencrypted
	RF_ASSERT( attemptedEncryption.mMode == protocol::EncryptionMode::kUnencrypted );

	Buffer messages;
	MessageIdentifier{ MsgWelcome::kID }.Append( messages );
	MsgWelcome welcome = {};
	welcome.mModeChange = attemptedEncryption.mPending;
	welcome.Append( messages );

	rftl::vector<Buffer> transmissions = CreateTransmissions(
		rftl::move( messages ),
		EncryptionState{},
		maxTransmissionSize );
	RF_ASSERT( transmissions.size() == 1 );

	return transmissions.at( 0 );
}



GAMESYNC_API ReadResult TryDecodeWelcomeTransmission(
	rftl::byte_view& bytes,
	EncryptionState& attemptedEncryption )
{
	// Hosts respond unencrypted
	RF_ASSERT( attemptedEncryption.mMode == protocol::EncryptionMode::kUnencrypted );

	auto const onMessage = [&attemptedEncryption]( MessageID const& id, rftl::byte_view& bytes ) -> ReadResult //
	{
		if( id == MsgWelcome::kID )
		{
			MsgWelcome msg = {};
			ReadResult const result = msg.TryRead( bytes );
			if( result == ReadResult::kSuccess )
			{
				attemptedEncryption.mPending = msg.mModeChange;
			}
			return result;
		}
		return ReadResult::kLogicError;
	};

	auto const onBatch = [&onMessage]( size_t totalBytes, rftl::byte_view bytes ) -> ReadResult //
	{
		return TryDecodeBatch( bytes, EncryptionState{}, onMessage );
	};

	return TryDecodeTransmission( bytes, EncryptionState{}, onBatch );
}

///////////////////////////////////////////////////////////////////////////////

GAMESYNC_API MsgSessionList CreateSessionListMessage(
	SessionMembers const& members,
	ConnectionIdentifier targetConnectionID )
{
	MsgSessionList retVal = {};

	retVal.mYourConnectionID = targetConnectionID;

	SessionMembers::ConnectionPlayerIDs const connections = members.GetConnectionPlayerIDs();
	for( SessionMembers::ConnectionPlayerIDs::value_type const& connection : connections )
	{
		ConnectionIdentifier const& connID = connection.first;

		if( connection.second.empty() )
		{
			// Player-less connections represented with an invalid player ID
			retVal.mConnectionEntries.emplace_back( connID, input::kInvalidPlayerID );
		}
		else
		{
			for( input::PlayerID const& playerID : connection.second )
			{
				retVal.mConnectionEntries.emplace_back( connID, playerID );
			}
		}
	}

	SessionMembers::PlayerIDs const unclaimed = members.GetUnclaimedPlayerIDs();
	for( input::PlayerID const& playerID : unclaimed )
	{
		// Connection-less players represented with an invalid connection ID
		retVal.mConnectionEntries.emplace_back( kInvalidConnectionIdentifier, playerID );
	}

	return retVal;
}



GAMESYNC_API SessionMembers ReadSessionListMessage(
	MsgSessionList const& message )
{
	SessionMembers retVal = {};

	retVal.mLocalConnection = message.mYourConnectionID;

	for( MsgSessionList::ConnectionEntry const& entry : message.mConnectionEntries )
	{
		ConnectionIdentifier const& conn = entry.first;
		input::PlayerID const& player = entry.second;

		bool const badConn = conn == kInvalidConnectionIdentifier;
		bool const badPlayer = player == input::kInvalidPlayerID;

		if( badConn && badPlayer )
		{
			RF_DBGFAIL();
			continue;
		}
		else if( badConn )
		{
			// Connection-less player
			retVal.mPlayerConnections[player];
		}
		else if( badPlayer )
		{
			// Player-less connection
			retVal.mAllConnections.emplace( conn );
		}
		else
		{
			retVal.mAllConnections.emplace( conn );
			RF_ASSERT( retVal.mPlayerConnections.count( player ) == 0 );
			retVal.mPlayerConnections[player] = conn;
		}
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

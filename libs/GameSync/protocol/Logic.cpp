#include "stdafx.h"
#include "Logic.h"

#include "GameSync/protocol/Encryption.h"
#include "GameSync/protocol/Messages.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"

#include "rftl/algorithm"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename TypeListType>
struct MessageWalker;

// 0 case
template<>
struct MessageWalker<TypeList<>>
{
	static bool IsInList( MessageID const& id )
	{
		return false;
	}

	static ReadResult TryRead( MessageID const& id, rftl::byte_view& bytes )
	{
		return ReadResult::kUnknownMessage;
	}
};

// N case
template<typename CurrentType, typename... RemainingTypes>
struct MessageWalker<TypeList<CurrentType, RemainingTypes...>>
{
	static bool IsInList( MessageID const& id )
	{
		if( id == CurrentType::kID )
		{
			return true;
		}
		using Next = MessageWalker<TypeList<RemainingTypes...>>;
		return Next::IsInList( id );
	}

	static ReadResult TryRead( MessageID const& id, rftl::byte_view& bytes )
	{
		if( id == CurrentType::kID )
		{
			CurrentType cur = {};
			return cur.TryRead( bytes );
		}

		using Next = MessageWalker<TypeList<RemainingTypes...>>;
		return Next::TryRead( id, bytes );
	}
};



size_t CalcStandardTransmissionHeaderSize()
{
	Buffer temp;
	CommonHeader{}.Append( temp );
	MessageBatch{}.Append( temp );
	return temp.size();
}

}
///////////////////////////////////////////////////////////////////////////////

ReadResult TryBlindDecodeBatch(
	rftl::byte_view bytes,
	EncryptionState const& encryption )
{
	using WalkAllMessages = details::MessageWalker<AllMessages>;
	return TryDecodeBatch( bytes, encryption, WalkAllMessages::TryRead );
}



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
		using WalkAllMessages = details::MessageWalker<AllMessages>;
		if( WalkAllMessages::IsInList( identifier.mID ) == false )
		{
			return ReadResult::kUnknownMessage;
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
}

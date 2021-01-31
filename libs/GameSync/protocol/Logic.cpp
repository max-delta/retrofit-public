#include "stdafx.h"
#include "Logic.h"

#include "GameSync/protocol/Encryption.h"
#include "GameSync/protocol/Messages.h"


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
	if( batch.mBatchBytes < tempBytes.size() )
	{
		return ReadResult::kTooSmall;
	}
	if( batch.mEncryption != encryption.mMode )
	{
		return ReadResult::kWrongEncryption;
	}

	// Batch payload
	rftl::byte_view const batchBytes = tempBytes.substr( batch.mBatchBytes );
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
}

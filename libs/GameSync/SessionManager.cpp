#include "stdafx.h"
#include "SessionManager.h"

#include "GameSync/protocol/Logic.h"
#include "GameSync/protocol/Standards.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "core_platform/IncomingBufferStitcher.h"
#include "core_math/math_casts.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/algorithms.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

bool SessionManager::Connection::HasPartialHandshake() const
{
	return mInitialConnectionTime < mPartialHandshakeTime;
}



bool SessionManager::Connection::HasHandshake() const
{
	if( mInitialConnectionTime < mCompletedHandshakeTime )
	{
		RF_ASSERT( mPartialHandshakeTime <= mCompletedHandshakeTime );
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////

SessionManager::SessionManager()
	: mEndpointManager( DefaultCreator<comm::EndpointManager>::Create() )
{
	//
}



SessionMembers SessionManager::GetSessionMembers() const
{
	ReaderLock const membersLock( mSessionMembersMutex );
	return mSessionMembers;
}



bool SessionManager::QueueOutgoingChatMessage( std::string&& text )
{
	RF_ASSERT( text.empty() == false );

	ConnectionIdentifier localIdentifier = kInvalidConnectionIdentifier;
	{
		ReaderLock const membersLock( mSessionMembersMutex );
		localIdentifier = mSessionMembers.mLocalConnection;
	}
	if( localIdentifier == kInvalidConnectionIdentifier )
	{
		return false;
	}

	ChatMessage message = {};
	message.mText = rftl::move( text );

	{
		WriterLock const unsentLock( mUnsentChatMessagesMutex );
		mUnsentChatMessages.emplace_back( message ); // Via copy
	}

	// Local copy adds information that would normally be set on receipt
	message.mSourceConnectionID = localIdentifier;
	message.mReceiveTime = Clock::now();

	{
		WriterLock const messagesLock( mChatMessagesMutex );
		mChatMessages.emplace_back( rftl::move( message ) ); // Via move
	}

	return true;
}



SessionManager::ChatMessages SessionManager::GetRecentChatMessages( size_t maxHistory ) const
{
	ReaderLock const messagesLock( mChatMessagesMutex );
	if( mChatMessages.size() <= maxHistory )
	{
		return mChatMessages;
	}
	return ChatMessages(
		mChatMessages.end() - math::integer_cast<ChatMessages::difference_type>( maxHistory ),
		mChatMessages.end() );
}

///////////////////////////////////////////////////////////////////////////////

void SessionManager::ProcessPendingConnectionOperations(
	OnMessageFunc const& onMessage,
	DoMessageWorkFunc const& doMessageWork )
{
	RF_ASSERT( onMessage != nullptr );
	RF_ASSERT( doMessageWork != nullptr );

	// Check all the connections for basic connectivity
	struct ValidConnection
	{
		ConnectionIdentifier mIdentifier = {};
		SharedPtr<comm::IncomingStream> incomingPtr;
		SharedPtr<comm::OutgoingStream> outgoingPtr;
		protocol::EncryptionState encryption;
	};
	rftl::static_vector<ValidConnection, kMaxConnectionCount> validConnections;
	rftl::static_vector<ConnectionIdentifier, kMaxConnectionCount> connectionsToDestroy;
	{
		ReaderLock const connectionLock( mConnectionsMutex );

		// Lookup the valid channels
		for( Connections::value_type const& connection : mConnections )
		{
			ConnectionIdentifier const& id = connection.first;
			Connection const& conn = connection.second;
			if( conn.HasHandshake() == false )
			{
				continue;
			}

			SharedPtr<comm::IncomingStream> incomingPtr;
			SharedPtr<comm::OutgoingStream> outgoingPtr;
			GetChannels( id, incomingPtr, outgoingPtr );
			if( incomingPtr == nullptr || outgoingPtr == nullptr )
			{
				// Flag for destroy
				connectionsToDestroy.emplace_back( id );
				continue;
			}

			if( incomingPtr->IsTerminated() || outgoingPtr->IsTerminated() )
			{
				// Flag for destroy
				connectionsToDestroy.emplace_back( id );
				continue;
			}

			ValidConnection valid = {};
			valid.mIdentifier = id;
			valid.incomingPtr = incomingPtr;
			valid.outgoingPtr = outgoingPtr;
			valid.encryption = conn.mEncryption;
			validConnections.emplace_back( rftl::move( valid ) );
		}
	}

	// Process incoming
	using FullBatches = rftl::vector<protocol::Buffer>;
	using FullBatchesBySender = rftl::unordered_map<ConnectionIdentifier, FullBatches>;
	FullBatchesBySender fullBatchesBySender;
	for( ValidConnection const& valid : validConnections )
	{
		ConnectionIdentifier const& id = valid.mIdentifier;
		comm::IncomingStream& incoming = *valid.incomingPtr;
		protocol::EncryptionState const& encryption = valid.encryption;

		// Form as many full batches as possible
		FullBatches fullBatches;
		while( true )
		{
			size_t const incomingSize = incoming.PeekNextBufferSize();
			if( incomingSize == 0 )
			{
				// No data yet / no more data after finishing a batch
				break;
			}

			// Try to decode a full batch
			protocol::Buffer const& buffer = incoming.CloneNextBuffer();
			rftl::byte_view bytes( buffer.begin(), buffer.end() );
			protocol::Buffer attemptedBatch;
			protocol::ReadResult const result = protocol::TryDecodeTransmissionsIntoFullBatch( bytes, encryption, attemptedBatch );
			if( result == protocol::ReadResult::kSuccess )
			{
				// Success, consume the bytes, store the batch
				size_t const consumedBytes = buffer.size() - bytes.size();
				protocol::Buffer const discarded = incoming.FetchNextBuffer( consumedBytes );
				fullBatches.emplace_back( rftl::move( attemptedBatch ) );
			}
			else if( result == protocol::ReadResult::kTooSmall )
			{
				// Not enough, try to stitch
				size_t const bytesStitched = incoming.TryStitchNextBuffer();
				if( bytesStitched <= 0 )
				{
					// Failed to stitch, will need to wait for more
					break;
				}
			}
			else
			{
				// Bad transmission
				RFLOG_ERROR( nullptr, RFCAT_GAMESYNC, "Transmission was invalid, terminating connection" );
				incoming.Terminate();
				connectionsToDestroy.emplace_back( id );
			}
		}
		if( fullBatches.empty() )
		{
			// No full batches could be formed
			continue;
		}

		RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Recieved %llu full batches from %llu", fullBatches.size(), id );

		// Store
		RF_ASSERT( fullBatchesBySender.count( id ) == 0 );
		fullBatchesBySender[id] = rftl::move( fullBatches );
	}

	// Create permitted recipient buffers
	MessagesByRecipient messagesByRecipient;
	for( ValidConnection const& valid : validConnections )
	{
		messagesByRecipient[valid.mIdentifier];
	}

	// Process messages
	for( ValidConnection const& valid : validConnections )
	{
		ConnectionIdentifier const& id = valid.mIdentifier;
		comm::IncomingStream& incoming = *valid.incomingPtr;
		protocol::EncryptionState const& encryption = valid.encryption;

		FullBatchesBySender::const_iterator const senderBatchesIter = fullBatchesBySender.find( id );
		if( senderBatchesIter == fullBatchesBySender.end() )
		{
			// No batches from this connection
			continue;
		}
		FullBatches const& fullBatches = senderBatchesIter->second;

		// For each batch...
		for( protocol::Buffer const& fullBatch : fullBatches )
		{
			rftl::byte_view const messages( fullBatch.begin(), fullBatch.end() );

			// Try a blind decode first
			{
				protocol::ReadResult const blindResult = TryBlindDecodeBatch( messages, encryption );
				if( blindResult != protocol::ReadResult::kSuccess )
				{
					// Bad blind decode
					RFLOG_ERROR( nullptr, RFCAT_GAMESYNC, "Batch was trivially invalid, terminating connection" );
					RF_DBGFAIL();
					incoming.Terminate();
					connectionsToDestroy.emplace_back( id );
					break;
				}
			}

			// Decode messages
			{
				auto const onMessageWrapper = //
					[&onMessage, connectionID = id] //
					( protocol::MessageID const& messageID, rftl::byte_view& bytes ) //
					-> protocol::ReadResult //
				{
					return onMessage( MessageParams{ connectionID, messageID, bytes } );
				};

				protocol::ReadResult const blindResult = protocol::TryDecodeBatch( messages, encryption, onMessageWrapper );
				if( blindResult != protocol::ReadResult::kSuccess )
				{
					// Bad decode
					RFLOG_ERROR( nullptr, RFCAT_GAMESYNC, "Batch was invalid, terminating connection" );
					RF_DBGFAIL();
					incoming.Terminate();
					connectionsToDestroy.emplace_back( id );
					break;
				}
			}
		}
	}

	// Process local logic
	{
		ConnectionIDs validConnectionIDs = {};
		for( ValidConnection const& valid : validConnections )
		{
			validConnectionIDs.emplace( valid.mIdentifier );
		}
		MessageWorkParams const params{ validConnectionIDs, messagesByRecipient };
		doMessageWork( params );
	}

	// Process outgoing
	for( ValidConnection const& valid : validConnections )
	{
		ConnectionIdentifier const& id = valid.mIdentifier;
		comm::OutgoingStream& outgoing = *valid.outgoingPtr;
		protocol::EncryptionState const& encryption = valid.encryption;

		protocol::Buffer& messages = messagesByRecipient.at( id );
		if( messages.empty() )
		{
			continue;
		}

		// Create and send out the transmissions
		rftl::vector<protocol::Buffer> transmissions = protocol::CreateTransmissions(
			rftl::move( messages ), encryption, protocol::kMaxRecommendedTransmissionSize );
		for( protocol::Buffer& transmission : transmissions )
		{
			bool const success = outgoing.StoreNextBuffer( rftl::move( transmission ) );
			if( success == false )
			{
				// Flag for destroy
				connectionsToDestroy.emplace_back( id );
				break;
			}
		}
	}

	// Destroy connections
	if( connectionsToDestroy.empty() == false )
	{
		WriterLock const connectionLock( mConnectionsMutex );

		comm::EndpointManager& endpointManager = *mEndpointManager;
		rftl::erase_duplicates( connectionsToDestroy );
		for( ConnectionIdentifier const& id : connectionsToDestroy )
		{
			RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Destroying a connection in operations" );

			Connections::iterator const iter = mConnections.find( id );
			if( iter == mConnections.end() )
			{
				// We dropped the lock on the connections, so it's possible
				//  this was terminated during processing
				RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Missing connection in operations" );
			}
			else
			{
				mConnections.erase( iter );
			}

			endpointManager.RemoveEndpoint( id );
		}
		endpointManager.RemoveOrphanedStreams( true );

		RFLOG_INFO( nullptr, RFCAT_GAMESYNC, "Connections destroyed, expect a connection update" );
	}
}



void SessionManager::GetChannels(
	ConnectionIdentifier id,
	SharedPtr<comm::IncomingStream>& incoming,
	SharedPtr<comm::OutgoingStream>& outgoing )
{
	incoming = nullptr;
	outgoing = nullptr;

	comm::EndpointManager& endpointManager = *mEndpointManager;

	SharedPtr<comm::LogicalEndpoint> const endpointPtr = endpointManager.GetEndpoint( id ).Lock();
	if( endpointPtr == nullptr )
	{
		RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null endpoint" );
		return;
	}
	comm::LogicalEndpoint& endpoint = *endpointPtr;

	static constexpr comm::ChannelFlags::Value kDesiredFlags = comm::ChannelFlags::Ordered;
	WeakSharedPtr<comm::IncomingStream> incomingWPtr = nullptr;
	WeakSharedPtr<comm::OutgoingStream> outgoingWPtr = nullptr;
	endpoint.ChooseIncomingChannel( incomingWPtr, kDesiredFlags );
	endpoint.ChooseOutgoingChannel( outgoingWPtr, kDesiredFlags );
	SharedPtr<comm::IncomingStream> const incomingPtr = incomingWPtr.Lock();
	SharedPtr<comm::OutgoingStream> const outgoingPtr = outgoingWPtr.Lock();
	if( incomingPtr == nullptr || outgoingPtr == nullptr )
	{
		RFLOG_DEBUG( nullptr, RFCAT_GAMESYNC, "Null channel" );
		return;
	}

	incoming = incomingPtr;
	outgoing = outgoingPtr;
}

//

bool SessionManager::HasBlindReader( protocol::MessageID const& id ) const
{
	ReaderLock const lock( mBlindReadersMutex );

	return mBlindReaders.count( id ) > 0;
}

//

protocol::ReadResult SessionManager::TryBlindMessageRead( protocol::MessageID const& id, rftl::byte_view& bytes )
{
	ReaderLock const lock( mBlindReadersMutex );

	BlindReaders::const_iterator const iter = mBlindReaders.find( id );
	if( iter == mBlindReaders.end() )
	{
		return protocol::ReadResult::kUnknownMessage;
	}

	return iter->second( bytes );
}

//

protocol::ReadResult SessionManager::TryBlindDecodeBatch( rftl::byte_view bytes, protocol::EncryptionState const& encryption )
{
	return protocol::TryDecodeBatch( bytes, encryption,
		[this]( protocol::MessageID const& id, rftl::byte_view& bytes ) -> protocol::ReadResult //
		{
			return this->TryBlindMessageRead( id, bytes );
		} );
}

///////////////////////////////////////////////////////////////////////////////
}

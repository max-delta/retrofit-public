#include "stdafx.h"
#include "SessionClientManager.h"

#include "GameSync/protocol/Logic.h"
#include "GameSync/protocol/Standards.h"
#include "GameSync/protocol/Messages.h"

#include "Communication/EndpointManager.h"
#include "Logging/Logging.h"

#include "PlatformNetwork_win32/TCPSocket.h"
#include "PlatformNetwork_win32/TCPBufferStream.h"

#include "core_platform/ThreadSettings.h"
#include "core_platform/IncomingBufferStitcher.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/ptr_transform.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/algorithms.h"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

SessionClientManager::SessionClientManager( ClientSpec spec )
	: mSpec( spec )
{
	AddBlindReader<protocol::MsgWelcome>();
	AddBlindReader<protocol::MsgSessionList>();
	AddBlindReader<protocol::MsgProxyChat>();
	AddBlindReader<protocol::MsgProxyRollbackInputEvents>();
}



SessionClientManager::~SessionClientManager()
{
	if( IsReceivingASession() )
	{
		StopReceivingASession();
	}
}



bool SessionClientManager::IsReceivingASession() const
{
	return mHandshakeThread.IsStarted();
}



void SessionClientManager::StartReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

	if constexpr( config::flag::kAsserts )
	{
		// Ensure there are no host connections
		{
			ReaderLock const connectionLock( mConnectionsMutex );

			RF_ASSERT( mConnections.empty() );
		}

		// Ensure there are no endpoints
		RF_ASSERT( mEndpointManager->GetAllEndpoints().empty() );
	}

	// Add our host connections
	{
		WriterLock const connectionLock( mConnectionsMutex );

		mConnections[kSingleHostIdentifier];
	}

	// Add our host endpoints
	mEndpointManager->AddEndpoint( kSingleHostIdentifier );

	// Initialize handshake thread
	{
		static constexpr auto prep = []() -> void //
		{
			using namespace platform::thread;
			SetThreadName( "Session Client Handshaker" );
			SetThreadPriority( ThreadPriority::Normal );
		};
		auto work = [this]() -> void //
		{
			this->DoHandshakes();
		};
		auto workCheck = [this]() -> bool //
		{
			bool const wasUneventful = mLastHandshakeUneventful.exchange( false, rftl::memory_order::memory_order_acquire );
			if( wasUneventful )
			{
				return false;
			}
			return true;
		};
		auto termCheck = [this]() -> bool //
		{
			return mShouldReceiveASession.load( rftl::memory_order::memory_order_acquire ) == false;
		};
		mHandshakeThread.Init( prep, work, workCheck, termCheck );
		mHandshakeThread.SetSafetyWakeupInterval( kHandshakeThrottle );
	}

	// Indicate the thread should run
	bool const wasReceiving = mShouldReceiveASession.exchange( true, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving == false );

	RF_ASSERT( mHandshakeThread.IsStarted() == false );
	mHandshakeThread.Start();
}



void SessionClientManager::StopReceivingASession()
{
	// NOTE: Taking lock entire time to lock start/stop logic
	WriterLock const startStopLock( mStartStopMutex );

	// Indicate our intent, which could potentially result in the handshake
	//  thread stopping on its own if we're extremely lucky
	bool const wasReceiving = mShouldReceiveASession.exchange( false, rftl::memory_order::memory_order_acq_rel );
	RF_ASSERT( wasReceiving );

	// Stop all host connections
	// NOTE: We have to do this, not the update thread, as it is likely
	//  blocked waiting for communications
	RF_TODO_ANNOTATION( "Stop all host connections" );

	// Stop handshake thread
	RF_ASSERT( mHandshakeThread.IsStarted() );
	mHandshakeThread.Stop();

	RF_TODO_ANNOTATION( "Destroy all host connections" );
	RF_TODO_ANNOTATION( "Destroy all endpoints" );
}



void SessionClientManager::ProcessPendingOperations()
{
	RF_ASSERT( IsReceivingASession() );

	auto const onMessage = [this]( MessageParams const& params ) -> protocol::ReadResult //
	{
		return this->HandleMessage( params );
	};
	auto const doMessageWork = [this]( MessageWorkParams const& params ) -> void //
	{
		this->DoMessageWork( params );
	};

	ProcessPendingConnectionOperations( onMessage, doMessageWork );
}



SessionClientManager::Diagnostics SessionClientManager::ReportDiagnostics() const
{
	Diagnostics retVal = {};

	{
		ReaderLock const connectionLock( mConnectionsMutex );

		for( Connections::value_type const& hostConnection : mConnections )
		{
			Connection const& conn = hostConnection.second;
			if( conn.mInitialConnectionTime < conn.mCompletedHandshakeTime )
			{
				retVal.mValidConnections++;
			}
			else
			{
				retVal.mInvalidConnections++;
			}
		}
	}

	return retVal;
}



void SessionClientManager::RequestPlayerChange( input::PlayerID id, bool claim )
{
	RF_ASSERT( id != input::kInvalidPlayerID );

	ReaderLock const lock( mPlayerChangesMutex );
	PlayerChange& playerChange = mPlayerChanges[id];
	playerChange = {};
	playerChange.mClaim = claim;
}

///////////////////////////////////////////////////////////////////////////////

void SessionClientManager::DoHandshakes()
{
	// NOTE: In a multi-host scenario, this will need to be refactored into a
	//  looping concept, similar to the host manager and client connections

	// Get the streams to communicate with a host
	SharedPtr<comm::IncomingStream> incomingStream = nullptr;
	SharedPtr<comm::OutgoingStream> outgoingStream = nullptr;
	GetOrCreateNextHandshakeChannels( incomingStream, outgoingStream );
	if( incomingStream == nullptr || outgoingStream == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to create/select a host connection as client" );
		return;
	}
	comm::IncomingStream& incoming = *incomingStream;
	comm::OutgoingStream& outgoing = *outgoingStream;

	// Check if we can probably ignore it before taking a write lock
	{
		ReaderLock const connectionLock( mConnectionsMutex );

		Connection& hostConnection = mConnections.at( kSingleHostIdentifier );
		RF_ASSERT( Clock::kLowest < hostConnection.mInitialConnectionTime );
		if( hostConnection.HasHandshake() )
		{
			// Already has handshake
			mLastHandshakeUneventful.store( true, rftl::memory_order::memory_order_release );
			return;
		}
	}

	// Check for, and perform any needed handshakes
	{
		WriterLock const connectionLock( mConnectionsMutex );

		Connection& hostConnection = mConnections.at( kSingleHostIdentifier );
		Clock::time_point const now = Clock::now();

		RF_ASSERT( Clock::kLowest < hostConnection.mInitialConnectionTime );
		if( hostConnection.HasPartialHandshake() == false )
		{
			// Never attempted handshake, need to do that

			protocol::EncryptionState& attemptedEncryption = hostConnection.mEncryption;
			attemptedEncryption.mPending = protocol::kExpectedEncryption;

			protocol::PrepareEncryptionRequest( attemptedEncryption );
			protocol::Buffer hello = protocol::CreateHelloTransmission( protocol::kMaxRecommendedTransmissionSize, attemptedEncryption );
			if( outgoing.StoreNextBuffer( rftl::move( hello ) ) )
			{
				hostConnection.mPartialHandshakeTime = now;
			}
		}

		if( hostConnection.HasHandshake() == false )
		{
			// Never completed handshake, need to do that

			size_t const incomingSize = incoming.PeekNextBufferSize();
			if( incomingSize == 0 )
			{
				// No data yet
				mLastHandshakeUneventful.store( true, rftl::memory_order::memory_order_release );
				return;
			}

			// If the handshake succeeds, we expect to enable encryption
			protocol::EncryptionState& attemptedEncryption = hostConnection.mEncryption;

			// See if data is valid
			protocol::Buffer const& buffer = incoming.CloneNextBuffer();
			rftl::byte_view bytes( buffer.begin(), buffer.end() );
			protocol::ReadResult const result = protocol::TryDecodeWelcomeTransmission( bytes, attemptedEncryption );
			if( result == protocol::ReadResult::kSuccess )
			{
				if( attemptedEncryption.mPending != protocol::kExpectedEncryption )
				{
					// Unsupported encryption, kill the connection
					RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission specified unsupported encryption, terminating connection" );
					incoming.Terminate();
					return;
				}

				// Success, consume the bytes
				RFLOG_MILESTONE( nullptr, RFCAT_GAMESYNC, "Welcome transmission was valid, upgrading connection" );
				size_t const consumedBytes = buffer.size() - bytes.size();
				protocol::Buffer const discarded = incoming.FetchNextBuffer( consumedBytes );

				// Finish handshake
				protocol::ApplyPendingEncryption( attemptedEncryption );
				hostConnection.mCompletedHandshakeTime = now;
			}
			else if( result == protocol::ReadResult::kTooSmall )
			{
				// Not enough data yet, attempt a stitch and try again next pass
				RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission wasn't large enough to decode. Suspicious..." );
				incoming.TryStitchNextBuffer();
				return;
			}
			else
			{
				// Bad transmission, kill the connection
				RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Welcome transmission was invalid, terminating connection" );
				incoming.Terminate();
				return;
			}
		}
	}
}



void SessionClientManager::GetOrCreateNextHandshakeChannels( SharedPtr<comm::IncomingStream>& incomingStream, SharedPtr<comm::OutgoingStream>& outgoingStream )
{

	// NOTE: In a multi-host scenario, we need more complex logic to
	//  evaluate what hosts we have, when we've heard from them, and which
	//  we're missing but could possibly connect to (and haven't failed to
	//  connect to them recently)
	auto const selectChannels = [this, &incomingStream, &outgoingStream]() -> bool //
	{
		GetChannels( kSingleHostIdentifier, incomingStream, outgoingStream );
		if( incomingStream == nullptr || outgoingStream == nullptr )
		{
			incomingStream = nullptr;
			outgoingStream = nullptr;
			return false;
		}
		return true;
	};

	// Select the best available streams
	bool const foundExistingStreams = selectChannels();
	if( foundExistingStreams == false )
	{
		// Failed to select streams, attempt to make new ones
		FormHostConnection( kSingleHostIdentifier, mSpec );
		bool const foundNewStreams = selectChannels();
		if( foundNewStreams == false )
		{
			RF_TODO_ANNOTATION( "Update timings for throttling logic" );
		}
	}
}



void SessionClientManager::FormHostConnection( comm::EndpointIdentifier hostIdentifier, ClientSpec spec )
{
	using TCPSocket = platform::network::TCPSocket;

	comm::EndpointManager& endpointManager = *mEndpointManager;

	// Remove all existing channels by resetting endpoint
	endpointManager.RemoveEndpoint( hostIdentifier );
	endpointManager.AddEndpoint( hostIdentifier );

	// Clear out and terminate any orphans this may have created
	endpointManager.RemoveOrphanedStreams( true );

	// Reset connection data
	{
		WriterLock const connectionLock( mConnectionsMutex );

		mConnections.at( kSingleHostIdentifier ) = Connection{};
	}

	// Block for new connection
	// NOTE: Not expected to block for an unreasonable time, but may need to
	//  re-visit with the addition of a timeout parameter on the API
	TCPSocket newConnection = TCPSocket::ConnectClientSocket( spec.mHostname, spec.mPort );
	if( newConnection.IsValid() == false )
	{
		// Failure may be intermittent, and may succeed if tried again later
		RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Failed to form a new host connection as client" );
		return;
	}

	// Update connection data
	{
		WriterLock const connectionLock( mConnectionsMutex );

		mConnections.at( kSingleHostIdentifier ).mInitialConnectionTime = Clock::now();
	}

	// Create channels
	CreateHostChannels( hostIdentifier, DefaultCreator<TCPSocket>::Create( rftl::move( newConnection ) ) );

	// Wake the handshaker to check out the new connection
	mHandshakeThread.Wake();
}



void SessionClientManager::CreateHostChannels( comm::EndpointIdentifier hostIdentifier, UniquePtr<platform::network::TCPSocket>&& newConnection )
{
	comm::EndpointManager& endpointManager = *mEndpointManager;

	// Add new streams to manager
	WeakSharedPtr<comm::IncomingStream> incomingStream;
	WeakSharedPtr<comm::OutgoingStream> outgoingStream;
	{
		using namespace platform;
		using namespace platform::network;
		SharedPtr<TCPSocket> sharedSocket;
		PtrTransformer<TCPSocket>::PerformTransformation( rftl::move( newConnection ), sharedSocket );
		UniquePtr<TCPIncomingBufferStream> incomingTCPStream = DefaultCreator<TCPIncomingBufferStream>::Create( sharedSocket );
		SharedPtr<TCPOutgoingBufferStream> outgoingTCPStream = DefaultCreator<TCPOutgoingBufferStream>::Create( sharedSocket );
		SharedPtr<IncomingBufferStitcher> incomingStitcher = DefaultCreator<IncomingBufferStitcher>::Create( rftl::move( incomingTCPStream ) );
		incomingStream = endpointManager.AddIncomingStream( rftl::move( incomingStitcher ) );
		outgoingStream = endpointManager.AddOutgoingStream( rftl::move( outgoingTCPStream ) );
	}

	// Add new channels to endpoint
	SharedPtr<comm::LogicalEndpoint> const hostEndpoint = endpointManager.GetEndpoint( hostIdentifier ).Lock();
	RF_ASSERT( hostEndpoint != nullptr );
	hostEndpoint->AddIncomingChannel( incomingStream, {} );
	hostEndpoint->AddOutgoingChannel( outgoingStream, {} );
}



protocol::ReadResult SessionClientManager::HandleMessage( MessageParams const& params )
{
	using namespace protocol;

	// Session list
	if( params.messageID == MsgSessionList::kID )
	{
		RFLOG_TRACE( nullptr, RFCAT_GAMESYNC, "Recieved session list" );
		MsgSessionList msg = {};
		ReadResult const read = msg.TryRead( params.bytes );
		if( read != ReadResult::kSuccess )
		{
			return read;
		}
		SessionMembers members = ReadSessionListMessage( msg );
		{
			WriterLock const membersLock( mSessionMembersMutex );
			mSessionMembers = rftl::move( members );
		}
		return ReadResult::kSuccess;
	}

	// Chat message
	if( params.messageID == MsgProxyChat::kID )
	{
		RFLOG_TRACE( nullptr, RFCAT_GAMESYNC, "Recieved chat" );
		MsgProxyChat msg = {};
		ReadResult const read = msg.TryRead( params.bytes );
		if( read != ReadResult::kSuccess )
		{
			return read;
		}

		ChatMessage chat = {};
		chat.mSourceConnectionID = msg.mSourceConnectionID;
		chat.mReceiveTime = Clock::now();
		chat.mText.assign( msg.mMsg.mText.begin(), msg.mMsg.mText.end() );

		// Store for local log
		{
			WriterLock const messagesLock( mChatMessagesMutex );
			mChatMessages.emplace_back( rftl::move( chat ) );
		}

		return ReadResult::kSuccess;
	}

	// Input message
	if( params.messageID == MsgProxyRollbackInputEvents::kID )
	{
		RFLOG_TRACE( nullptr, RFCAT_GAMESYNC, "Recieved input" );
		MsgProxyRollbackInputEvents msg = {};
		ReadResult const read = msg.TryRead( params.bytes );
		if( read != ReadResult::kSuccess )
		{
			return read;
		}

		RollbackSourcedPack pack = {};
		pack.mSourceConnectionID = msg.mSourceConnectionID;
		pack.mInputPack = rftl::move( msg.mMsg.mInputPack );

		// Store for local queue
		{
			WriterLock const packLock( mRollbackSourcedPacksMutex );
			mRollbackSourcedPacks.emplace_back( rftl::move( pack ) );
		}

		return ReadResult::kSuccess;
	}

	RFLOG_WARNING( nullptr, RFCAT_GAMESYNC, "Unhandled message ID" );
	RF_DBGFAIL();
	return ReadResult::kUnknownMessage;
}



void SessionClientManager::DoMessageWork( MessageWorkParams const& params )
{
	if( params.validConnectionIDs.empty() )
	{
		// No host yet
		return;
	}

	RF_ASSERT( params.validConnectionIDs.size() == 1 );
	RF_ASSERT( *params.validConnectionIDs.begin() == kSingleHostIdentifier );
	protocol::Buffer& messages = params.messagesByRecipient.at( kSingleHostIdentifier );

	// Process player claims
	{
		ReaderLock const currentLock( mSessionMembersMutex );

		// Get our connection
		ConnectionIdentifier const& connectionID = mSessionMembers.mLocalConnection;
		if( connectionID != kInvalidConnectionIdentifier )
		{
			WriterLock const requestLock( mPlayerChangesMutex );

			SessionMembers::PlayerIDs const all = mSessionMembers.GetPlayerIDs();
			SessionMembers::PlayerIDs const local = mSessionMembers.GetLocalPlayerIDs();
			SessionMembers::PlayerIDs const unclaimed = mSessionMembers.GetUnclaimedPlayerIDs();

			Clock::time_point const now = Clock::now();

			// For each request...
			PlayerChanges::iterator requestIter = mPlayerChanges.begin();
			while( requestIter != mPlayerChanges.end() )
			{
				input::PlayerID const& id = requestIter->first;
				RF_ASSERT( id != input::kInvalidPlayerID );
				PlayerChange& change = requestIter->second;

				if( all.count( id ) == 0 )
				{
					// Not a valid ID yet
					requestIter++;
					continue;
				}

				if( change.mClaim && local.count( id ) != 0 )
				{
					// Claim succeeded
					requestIter = mPlayerChanges.erase( requestIter );
					continue;
				}

				if( change.mClaim == false && unclaimed.count( id ) != 0 )
				{
					// Relinquish succeeded
					requestIter = mPlayerChanges.erase( requestIter );
					continue;
				}

				if( change.mClaim && unclaimed.count( id ) == 0 )
				{
					// Want to claim, but it's not available yet
					requestIter++;
					continue;
				}

				// Want to make a request
				if( change.mClaim )
				{
					RF_ASSERT( unclaimed.count( id ) != 0 );
				}
				else
				{
					RF_ASSERT( local.count( id ) != 0 );
				}

				if( now - change.mSent < kPlayerClaimThrottle )
				{
					// Not enough time since last time this was sent
					requestIter++;
					continue;
				}

				// Make request
				protocol::MessageIdentifier{ protocol::MsgClaimPlayer::kID }.Append( messages );
				protocol::MsgClaimPlayer claimPlayer = {};
				claimPlayer.mPlayerID = id;
				claimPlayer.mClaim = change.mClaim;
				claimPlayer.Append( messages );

				// Update sent time
				change.mSent = now;

				requestIter++;
				continue;
			}
		}
	}

	// Process chat
	{
		// Pull all the unsent chat messages
		ChatMessages chatsToSend;
		{
			WriterLock const chatLock( mUnsentChatMessagesMutex );
			chatsToSend.swap( mUnsentChatMessages );
		}

		// For each chat message...
		for( ChatMessage const& chatToSend : chatsToSend )
		{
			// Send
			protocol::MessageIdentifier{ protocol::MsgChat::kID }.Append( messages );
			protocol::MsgChat chat = {};
			chat.mText.assign( chatToSend.mText.begin(), chatToSend.mText.end() );
			chat.Append( messages );
		}
	}

	// Process input
	{
		// Pull all the unsent input packs
		RollbackInputPacks packsToSend;
		{
			WriterLock const packLock( mUnsentRollbackInputPacksMutex );
			packsToSend.swap( mUnsentRollbackInputPacks );
		}

		// For each input pack...
		for( RollbackInputPack& packToSend : packsToSend )
		{
			// Send
			protocol::MessageIdentifier{ protocol::MsgRollbackInputEvents::kID }.Append( messages );
			protocol::MsgRollbackInputEvents pack = {};
			pack.mInputPack = rftl::move( packToSend );
			pack.Append( messages );
		}
	}

	RF_TODO_ANNOTATION( "Local queue" );
}

///////////////////////////////////////////////////////////////////////////////
}

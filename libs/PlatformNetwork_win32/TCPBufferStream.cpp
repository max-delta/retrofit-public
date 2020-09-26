#include "stdafx.h"
#include "TCPBufferStream.h"

#include "PlatformNetwork_win32/TCPSocket.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

TCPIncomingBufferStream::TCPIncomingBufferStream( SharedPtr<TCPSocket> connectedSocket )
	: TCPIncomingBufferStream( connectedSocket, kDefaultPrefetchSize )
{
	//
}



TCPIncomingBufferStream::TCPIncomingBufferStream( SharedPtr<TCPSocket> connectedSocket, size_t maxPrefetchSize )
	: mSocket( connectedSocket )
	, mMaxPrefetchSize( maxPrefetchSize )
{
	RF_ASSERT( connectedSocket != nullptr );
	RF_ASSERT( connectedSocket->IsListener() == false );
}



size_t TCPIncomingBufferStream::PeekNextBufferSize() const
{
	// Benign, but likely indicates a design flaw in caller
	static constexpr auto raceWarning = []() //
	{
		RFLOG_WARNING( nullptr, RFCAT_PLATFORMNETWORK, "Race condition on TCP buffer stream, re-evaluate usage model" );
	};

	// Check for existing prefetch buffer
	{
		ReaderLock const prefetchLock( mPrefetchMutex, rftl::try_to_lock );
		if( prefetchLock.owns_lock() == false )
		{
			raceWarning();
			return 0;
		}

		if( mPrefetchBuffer.empty() == false )
		{
			return mPrefetchBuffer.size();
		}
	}

	// Fill prefetch buffer
	{
		WriterLock const prefetchLock( mPrefetchMutex, rftl::try_to_lock );
		if( prefetchLock.owns_lock() == false )
		{
			raceWarning();
			return 0;
		}

		// Check to make sure we didn't enter a race when we briefly dropped
		//  our lock to upgrae to a write
		if( mPrefetchBuffer.empty() == false )
		{
			raceWarning();
			return 0;
		}

		Prefetch( prefetchLock );
		return mPrefetchBuffer.size();
	}
}



TCPIncomingBufferStream::Buffer TCPIncomingBufferStream::FetchNextBuffer()
{
	WriterLock const prefetchLock( mPrefetchMutex );

	if( mPrefetchBuffer.empty() )
	{
		Hardfetch( prefetchLock, kDefaultPrefetchSize );
	}
	return rftl::move( mPrefetchBuffer );
}



bool TCPIncomingBufferStream::IsTerminated() const
{
	return mSocket->IsValid() == false;
}



SharedPtr<TCPSocket const> TCPIncomingBufferStream::GetSocket() const
{
	return mSocket;
}

///////////////////////////////////////////////////////////////////////////////

void TCPIncomingBufferStream::Prefetch( WriterLock const& lock ) const
{
	RF_ASSERT( lock.owns_lock() );
	RF_ASSERT( mPrefetchBuffer.empty() );

	TCPSocket& socket = *mSocket;
	bool const canPeek = socket.PeekReceiveNonBlocking();
	if( canPeek == false )
	{
		return;
	}

	const_cast<TCPIncomingBufferStream*>( this )->Hardfetch( lock, kDefaultPrefetchSize );
}



void TCPIncomingBufferStream::Hardfetch( WriterLock const& lock, size_t maxSize )
{
	RF_ASSERT( lock.owns_lock() );
	RF_ASSERT( mPrefetchBuffer.empty() );

	TCPSocket& socket = *mSocket;
	socket.ReceiveBuffer( mPrefetchBuffer, maxSize );
}

///////////////////////////////////////////////////////////////////////////////

TCPOutgoingBufferStream::TCPOutgoingBufferStream( SharedPtr<TCPSocket> connectedSocket )
	: mSocket( connectedSocket )
{
	RF_ASSERT( connectedSocket != nullptr );
	RF_ASSERT( connectedSocket->IsListener() == false );
}



bool TCPOutgoingBufferStream::StoreNextBuffer( Buffer&& buffer )
{
	bool const success = mSocket->SendBuffer( buffer );
	if( success )
	{
		buffer.clear();
	}
	return success;
}



bool TCPOutgoingBufferStream::IsTerminated() const
{
	return mSocket->IsValid() == false;
}



SharedPtr<TCPSocket const> TCPOutgoingBufferStream::GetSocket() const
{
	return mSocket;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "IncomingBufferStitcher.h"

#include "core_math/math_casts.h"

#include "core/rf_assert.h"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

IncomingBufferStitcher::IncomingBufferStitcher( UniquePtr<IncomingBufferStream>&& underlyingStream )
	: mUnderlyingStream( rftl::move( underlyingStream ) )
{
	RF_ASSERT( mUnderlyingStream != nullptr );
}



size_t IncomingBufferStitcher::PeekNextBufferSize() const
{
	// NOTE: Lock intentionally kept past buffer access, to ensure a competing
	//  stitching fetch on the working buffer is blocked
	ReaderLock const lock( mWorkingBufferMutex );

	// Prefer our working buffer first
	if( mWorkingBuffer.empty() == false )
	{
		return mWorkingBuffer.size();
	}

	// Fall back to underlying
	return mUnderlyingStream->PeekNextBufferSize();
}



IncomingBufferStitcher::Buffer IncomingBufferStitcher::FetchNextBuffer()
{
	// NOTE: Lock intentionally kept past buffer access, to ensure a competing
	//  stitching fetch on the working buffer is blocked
	WriterLock const lock( mWorkingBufferMutex );

	// Prefer our working buffer first
	if( mWorkingBuffer.empty() == false )
	{
		return rftl::move( mWorkingBuffer );
	}

	// Fall back to underlying
	// NOTE: Skips our stiching capabilities
	return mUnderlyingStream->FetchNextBuffer();
}



void IncomingBufferStitcher::Terminate()
{
	mUnderlyingStream->Terminate();
}



bool IncomingBufferStitcher::IsTerminated() const
{
	return mUnderlyingStream->IsTerminated();
}



IncomingBufferStitcher::Buffer IncomingBufferStitcher::FetchNextBuffer( size_t exactSize )
{
	RF_ASSERT( exactSize > 0 );

	WriterLock const lock( mWorkingBufferMutex );

	// Fill up working buffer
	while( mWorkingBuffer.size() < exactSize )
	{
		// Fetch
		Buffer const tempBuf = mUnderlyingStream->FetchNextBuffer();
		if( tempBuf.empty() )
		{
			// Error, bail
			return {};
		}

		// Append
		mWorkingBuffer.insert( mWorkingBuffer.end(), tempBuf.begin(), tempBuf.end() );
	}

	if( mWorkingBuffer.size() == exactSize )
	{
		// Perfect match
		return rftl::move( mWorkingBuffer );
	}

	// Extract and trim
	Buffer::const_iterator const begin = mWorkingBuffer.begin();
	Buffer::const_iterator const end = begin + math::integer_cast<Buffer::difference_type>( exactSize );
	Buffer const retVal = Buffer( begin, end );
	RF_ASSERT( retVal.size() == exactSize );
	mWorkingBuffer.erase( begin, end );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

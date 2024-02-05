#include "stdafx.h"
#include "FileHandle.h"

#include "core_math/math_casts.h"

#include "rftl/ios"
#include "rftl/istream"
#include "rftl/ostream"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( UniquePtr<rftl::streambuf>&& streamBuf )
	: mStreamBuf( rftl::move( streamBuf ) )
{
	RF_ASSERT( mStreamBuf != nullptr );
}



FileHandle::~FileHandle()
{
	RF_ASSERT( mStreamBuf != nullptr );
	// NOTE: Would like to check for errors when closing a file, but there is
	//  no exposed way to do that unless we know the concrete type
	// NOTE: Maybe have an optional WeakPtr option for the concrete type, that
	//  we could use here if available?
	RF_TODO_ANNOTATION( "Come up with a way to check for errors on close" );
	mStreamBuf = nullptr;
}



size_t FileHandle::GetFileSize() const
{
	RF_ASSERT( mStreamBuf != nullptr );
	rftl::streambuf& buf = *mStreamBuf;

	static constexpr rftl::ios_base::openmode kMode = rftl::ios_base::in | rftl::ios_base::out;

	// Need to restore state when done
	rftl::streampos const originalPos = buf.pubseekoff( 0, rftl::ios_base::cur, kMode );
	if( originalPos < 0 )
	{
		RF_DBGFAIL();
		return 0;
	}

	rftl::streampos const endPos = buf.pubseekoff( 0, rftl::ios_base::end, kMode );
	if( endPos < 0 )
	{
		// Unclear what to do here
		RF_DBGFAIL();
	}

	rftl::streamoff const retVal = endPos;

	rftl::streampos const restoredPos = buf.pubseekpos( originalPos, kMode );
	if( restoredPos < 0 )
	{
		// Unclear what to do here
		RF_DBGFAIL();
	}
	if( restoredPos != originalPos )
	{
		// Unclear what to do here
		RF_DBGFAIL();
	}

	return math::integer_cast<size_t>( retVal );
}



void FileHandle::Rewind()
{
	RF_ASSERT( mStreamBuf != nullptr );
	rftl::streambuf& buf = *mStreamBuf;

	static constexpr rftl::ios_base::openmode kMode = rftl::ios_base::in | rftl::ios_base::out;

	rftl::streampos const startPos = buf.pubseekoff( 0, rftl::ios_base::beg, kMode );
	if( startPos != 0 )
	{
		RF_DBGFAIL();
	}
}



int FileHandle::GetChar()
{
	RF_ASSERT( mStreamBuf != nullptr );
	rftl::istream stream( mStreamBuf.Get() );

	int const retVal = stream.get();
	static_assert( rftl::istream::traits_type::eof() == EOF );

	return retVal;
}



size_t FileHandle::ReadBytes( void* buffer, size_t numBytes )
{
	RF_ASSERT( buffer != nullptr );
	RF_ASSERT( numBytes > 0 );

	RF_ASSERT( mStreamBuf != nullptr );
	rftl::istream stream( mStreamBuf.Get() );

	stream.read(
		reinterpret_cast<char*>( buffer ),
		math::integer_cast<rftl::streamsize>( numBytes ) );
	size_t const retVal = math::integer_cast<size_t>( stream.gcount() );

	return retVal;
}



size_t FileHandle::WriteBytes( void const* buffer, size_t numBytes )
{
	RF_ASSERT( buffer != nullptr );
	RF_ASSERT( numBytes > 0 );

	RF_ASSERT( mStreamBuf != nullptr );
	rftl::ostream stream( mStreamBuf.Get() );

	rftl::streampos const originalPos = stream.tellp();
	RF_ASSERT( originalPos >= 0 );

	RF_ASSERT( ( stream.rdstate() & stream.badbit ) == 0 );
	stream.write(
		reinterpret_cast<char const*>( buffer ),
		math::integer_cast<rftl::streamsize>( numBytes ) );
	bool const error = ( stream.rdstate() & stream.badbit ) != 0;

	rftl::streampos const newPos = stream.tellp();
	RF_ASSERT( newPos >= 0 );
	RF_ASSERT( originalPos <= newPos );
	size_t const bytesWritten = math::integer_cast<size_t>( newPos - originalPos );

	if( error )
	{
		// Error? There's like no good documentation for what you can do here
		RF_ASSERT( bytesWritten < numBytes );
	}
	else
	{
		RF_ASSERT( bytesWritten == numBytes );
	}

	return bytesWritten;
}



WeakPtr<rftl::streambuf> FileHandle::GetUnsafeTransientStreamBuffer()
{
	RF_ASSERT( mStreamBuf != nullptr );
	return mStreamBuf;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "FileBuffer.h"

#include "core_math/math_clamps.h"
#include "core_vfs/SeekHandle.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

FileBuffer::FileBuffer( ExplicitDefaultConstruct )
{
	//
}



FileBuffer::FileBuffer( SeekHandle& file, bool addTerminatingNull )
{
	file.Rewind();

	size_t const fileSize = file.GetSeekableSize();
	if( addTerminatingNull )
	{
		mBuffer.resize( fileSize + 1 );
	}
	else
	{
		mBuffer.resize( fileSize );
	}

	// NOTE: Filesize could be zero, which could make the buffer not actually
	//  get allocated, so we only try to read for non-zero buffers
	size_t amountRead = 0;
	if( fileSize > 0 )
	{
		amountRead = file.ReadBytes( mBuffer.data(), fileSize );
	}

	if( addTerminatingNull )
	{
		mBuffer.resize( amountRead + 1 );
		mBuffer.back() = '\0';
	}
	else
	{
		mBuffer.resize( amountRead );
	}

	if( mBuffer.capacity() > mBuffer.size() )
	{
		// This shouldn't normally happen
		RF_DBGFAIL_MSG( "Buffer for file will be shrunk, expected to perform extra allocations" );
	}
	mBuffer.shrink_to_fit();
}



FileBuffer::FileBuffer( SeekHandle& file, size_t maxBytes )
{
	RF_ASSERT( maxBytes > 0 );

	file.Rewind();

	size_t const fileSize = file.GetSeekableSize();
	size_t const bytesToRead = math::Min( fileSize, maxBytes );
	mBuffer.resize( bytesToRead );

	size_t const amountRead = file.ReadBytes( mBuffer.data(), bytesToRead );
	mBuffer.resize( amountRead );

	if( mBuffer.capacity() > mBuffer.size() )
	{
		// This shouldn't normally happen
		RF_DBGFAIL_MSG( "Buffer for file will be shrunk, expected to perform extra allocations" );
	}
	mBuffer.shrink_to_fit();
}



FileBuffer::FileBuffer( FileBuffer&& rhs )
	: mBuffer( rftl::move( rhs.mBuffer ) )
{
	//
}



FileBuffer& FileBuffer::operator=( FileBuffer&& rhs )
{
	if( this != &rhs )
	{
		this->mBuffer = rftl::move( rhs.mBuffer );
	}
	return *this;
}



bool FileBuffer::IsEmpty() const
{
	return mBuffer.empty();
}



size_t FileBuffer::GetSize() const
{
	return mBuffer.size();
}



rftl::byte_view FileBuffer::GetBytes() const
{
	return rftl::byte_view( mBuffer.data(), mBuffer.size() );
}



rftl::string_view FileBuffer::GetChars() const
{
	return rftl::string_view( reinterpret_cast<char const*>( mBuffer.data() ), mBuffer.size() );
}

///////////////////////////////////////////////////////////////////////////////
}

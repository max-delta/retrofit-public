#include "stdafx.h"
#include "FileBuffer.h"

#include "PlatformFilesystem/FileHandle.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

FileBuffer::FileBuffer( ExplicitDefaultConstruct )
{
	//
}



FileBuffer::FileBuffer( FileHandle const& file, bool addTerminatingNull )
{
	rewind( file.GetFile() );

	size_t const fileSize = file.GetFileSize();
	if( addTerminatingNull )
	{
		mBuffer.resize( fileSize + 1 );
	}
	else
	{
		mBuffer.resize( fileSize );
	}

	size_t const amountRead = fread( mBuffer.data(), sizeof( decltype( mBuffer )::value_type ), fileSize, file.GetFile() );
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
}}

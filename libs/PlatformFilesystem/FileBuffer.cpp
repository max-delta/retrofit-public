#include "stdafx.h"
#include "FileBuffer.h"

#include "PlatformFilesystem/FileHandle.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

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



void const* FileBuffer::GetData() const
{
	return mBuffer.data();
}



size_t FileBuffer::GetSize() const
{
	return mBuffer.size();
}

///////////////////////////////////////////////////////////////////////////////
}}

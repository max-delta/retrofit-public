#include "stdafx.h"
#include "FileHandle.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( FILE* file )
	: mFile( file )
{
	RF_ASSERT( file != nullptr );
}



FileHandle::~FileHandle()
{
	RF_ASSERT( mFile != nullptr );
	int const closeResult = fclose( mFile );
	RF_ASSERT( closeResult == 0 );
	mFile = nullptr;
}



FILE* FileHandle::GetFile() const
{
	return mFile;
}

///////////////////////////////////////////////////////////////////////////////
}}

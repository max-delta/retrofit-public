#include "stdafx.h"
#include "FileHandle.h"

#include "core_math/math_casts.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( FILE*&& file )
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



size_t FileHandle::GetFileSize() const
{
	// Need to restore state when done
	fpos_t originalPos = {};
	int const getResult = fgetpos( mFile, &originalPos );
	if( getResult != 0 )
	{
		RF_DBGFAIL();
		return 0;
	}

	int const seekResult = fseek( mFile, 0, SEEK_END );
	if( seekResult != 0 )
	{
		// Unclear what to do here
		RF_DBGFAIL();
	}

	long retVal = ftell( mFile );

	int const setResult = fsetpos( mFile, &originalPos );
	if( setResult != 0 )
	{
		// Unclear what to do here
		RF_DBGFAIL();
	}

	return math::integer_cast<size_t>( retVal );
}

///////////////////////////////////////////////////////////////////////////////
}

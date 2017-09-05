#include "stdafx.h"
#include "FileHandle.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( FILE* file )
	: m_File( file )
{
	RF_ASSERT( file != nullptr );
}



FileHandle::~FileHandle()
{
	RF_ASSERT( m_File != nullptr );
	int closeResult = fclose( m_File );
	RF_ASSERT( closeResult == 0 );
	m_File = nullptr;
}



FILE* FileHandle::GetFile() const
{
	return m_File;
}

///////////////////////////////////////////////////////////////////////////////
}}
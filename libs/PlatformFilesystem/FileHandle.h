#pragma once
#include "project.h"

#include "core/macros.h"

#include <stdio.h>


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API FileHandle
{
	RF_NO_COPY( FileHandle );
	RF_NO_MOVE( FileHandle );

	//
	// Public methods
public:
	FileHandle() = delete;
	FileHandle( FILE* file );
	~FileHandle();

	FILE* GetFile() const;


	//
	// Private data
private:
	FILE* m_File;
};

///////////////////////////////////////////////////////////////////////////////
}}
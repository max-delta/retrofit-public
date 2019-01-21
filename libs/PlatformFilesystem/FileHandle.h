#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/cstdio"


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
	explicit FileHandle( FILE*&& file );
	~FileHandle();

	FILE* GetFile() const;
	size_t GetFileSize() const;


	//
	// Private data
private:
	FILE* mFile;
};

///////////////////////////////////////////////////////////////////////////////
}}

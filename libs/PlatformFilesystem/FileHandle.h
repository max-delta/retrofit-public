#pragma once
#include "project.h"

#include "core_vfs/SeekHandle.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API FileHandle : public SeekHandle
{
	RF_NO_COPY( FileHandle );
	RF_NO_MOVE( FileHandle );

	//
	// Public methods
public:
	explicit FileHandle( UniquePtr<rftl::streambuf>&& streamBuf );
	~FileHandle();
};

///////////////////////////////////////////////////////////////////////////////
}

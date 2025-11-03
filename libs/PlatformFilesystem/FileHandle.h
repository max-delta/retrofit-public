#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"

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
	FileHandle( UniquePtr<rftl::filebuf>&& fileBuf, VFSPath const& path );
	~FileHandle();


	//
	// Private methods
private:
	FileHandle( WeakPtr<rftl::filebuf> fileBuf, UniquePtr<rftl::filebuf>&& streamBuf, VFSPath const& path );


	//
	// Private data
private:
	WeakPtr<rftl::filebuf> mConcreteType;
	VFSPath const mPath;
};

///////////////////////////////////////////////////////////////////////////////
}

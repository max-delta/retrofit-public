#pragma once
#include "project.h"

#include "GameSprite/SpriteFwd.h"

#include "PlatformFilesystem/VFSPath.h"

#include "rftl/unordered_map"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

class GAMESPRITE_API BitmapCache
{
	RF_NO_COPY( BitmapCache );

	//
	// Types and constants
private:
	using CachedBitmaps = rftl::unordered_map<file::VFSPath, UniquePtr<sprite::Bitmap>>;


	//
	// Public methods
public:
	explicit BitmapCache( WeakPtr<file::VFS const> vfs );

	WeakPtr<sprite::Bitmap const> Fetch( file::VFSPath const& path );


	//
	// Private data
private:
	WeakPtr<file::VFS const> const mVfs;
	CachedBitmaps mCachedBitmaps;
};

///////////////////////////////////////////////////////////////////////////////
}}

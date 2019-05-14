#include "stdafx.h"
#include "BitmapCache.h"

#include "GameSprite/Bitmap.h"
#include "GameSprite/BitmapReader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

BitmapCache::BitmapCache( WeakPtr<file::VFS const> vfs )
	: mVfs( vfs )
{
	//
}


WeakPtr<sprite::Bitmap const> BitmapCache::Fetch( file::VFSPath const& path )
{
	CachedBitmaps::const_iterator const iter = mCachedBitmaps.find( path );
	if( iter != mCachedBitmaps.end() )
	{
		// Return cached
		RF_ASSERT( iter->second != nullptr );
		return iter->second;
	}

	// Load to cache
	UniquePtr<sprite::Bitmap> bitmap = DefaultCreator<sprite::Bitmap>::Create( ExplicitDefaultConstruct{} );
	WeakPtr<sprite::Bitmap const> retVal = bitmap;
	{
		file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
		RF_ASSERT( handle != nullptr );
		file::FileBuffer const buffer( *handle, false );
		*bitmap = sprite::BitmapReader::ReadRGBABitmap( buffer.GetData(), buffer.GetSize() );
	}
	mCachedBitmaps[path] = rftl::move( bitmap );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

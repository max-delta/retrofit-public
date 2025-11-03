#include "stdafx.h"
#include "FramePackManager.h"

#include "PPU/FramePackSerDes.h"
#include "PPU/FramePack.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core_math/math_casts.h"
#include "core_vfs/SeekHandle.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

FramePackManager::FramePackManager( WeakPtr<file::VFS> const& vfs, TextureLoadRefFunc&& texLoadFunc )
	: ResourceManagerType()
	, mVfs( vfs )
	, mTexLoadFunc( rftl::move( texLoadFunc ) )
{
	RF_ASSERT( mVfs != nullptr );
	RF_ASSERT( mTexLoadFunc != nullptr );
}



FramePackManager::~FramePackManager()
{
	InternalShutdown();
}



UniquePtr<FramePackManager::ResourceType> FramePackManager::AllocateResourceFromFile( Filename const& filename )
{
	rftl::vector<file::VFSPath> textures;
	UniquePtr<FramePackBase> framePack;

	// Read in from file
	{
		// Open
		file::VFS const& vfs = *mVfs;
		file::SeekHandlePtr fileHandle = vfs.GetFileForRead( filename );
		if( fileHandle == nullptr )
		{
			RFLOG_ERROR( filename, RFCAT_PPU, "Failed to open framepack file" );
			return nullptr;
		}

		// Read into buffer
		file::FileBuffer buffer( *fileHandle, false );

		// Close
		fileHandle = nullptr;

		// Deserialize
		bool const readSuccess = FramePackSerDes::DeserializeFromBuffer( textures, buffer.GetBytes(), framePack );
		if( readSuccess == false )
		{
			RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize FPack" );
			return nullptr;
		}
	}

	FramePackBase::TimeSlot* const timeSlots = framePack->GetMutableTimeSlots();
	size_t const numSlots = framePack->mNumTimeSlots;
	RF_ASSERT( textures.size() == numSlots );
	for( size_t i = 0; i < numSlots; i++ )
	{
		FramePackBase::TimeSlot& timeSlot = timeSlots[i];
		file::VFSPath const& texPath = textures[i];
		ManagedTextureID const texID = mTexLoadFunc( texPath );
		if( texID == kInvalidManagedTextureID )
		{
			RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture for framepack" );
			timeSlot.mTextureReference = kInvalidManagedTextureID;
			continue;
		}
		timeSlot.mTextureReference = texID;
	}

	return framePack;
}

///////////////////////////////////////////////////////////////////////////////
}

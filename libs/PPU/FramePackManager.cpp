#include "stdafx.h"
#include "FramePackManager.h"

#include "PPU/FramePackSerDes.h"
#include "PPU/FramePack.h"
#include "PlatformFilesystem/VFS.h"

#include "core_math/math_casts.h"
#include "core_vfs/FileBuffer.h"
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



UniquePtr<FramePackManager::ResourceType> FramePackManager::AllocateResourceFromFile( ResourceNameView resourceName, ManagedResourceIDType managedResourceID, Filename const& filename )
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

	// Track texture reuse so we don't load duplicate textures multiple times
	RF_TODO_ANNOTATION(
		"Don't allow framepacks to serialize duplicate textures, rely on some"
		" aliasing capabilities in the format instead" );
	using ExistingTextures = rftl::unordered_map<file::VFSPath, ManagedTextureID>;
	ExistingTextures existingTextures;

	FramePackBase::TimeSlot* const timeSlots = framePack->GetMutableTimeSlots();
	size_t const numSlots = framePack->mNumTimeSlots;
	RF_ASSERT( textures.size() == numSlots );
	for( size_t i = 0; i < numSlots; i++ )
	{
		FramePackBase::TimeSlot& timeSlot = timeSlots[i];
		file::VFSPath const& texPath = textures[i];

		// Check for texture reuse
		ManagedTextureID texID;
		ExistingTextures::const_iterator const existingTexture = existingTextures.find( texPath );
		if( existingTexture != existingTextures.end() )
		{
			texID = existingTexture->second;
			RFLOG_INFO( filename, RFCAT_PPU, "Reusing a texture {} for framepack {}", texPath, resourceName );
		}
		else
		{
			ResourceName const texName = rftl::format( "fpack_{}_{}", resourceName, i );
			texID = mTexLoadFunc( texName, texPath );
			if( texID == kInvalidManagedTextureID )
			{
				RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture for framepack" );
			}
			bool const newEntry = existingTextures.emplace( texPath, texID ).second;
			RF_ASSERT( newEntry );
		}

		timeSlot.mTextureReference = texID;
	}

	return framePack;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "FramePackManager.h"

#include "PPU/FramePackSerDes.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

FramePackManager::FramePackManager( WeakPtr<gfx::TextureManager> const& texMan, WeakPtr<file::VFS> const& vfs )
	: ResourceManagerType()
	, mTextureManager( texMan )
	, mVfs( vfs )
{
	//
}



FramePackManager::~FramePackManager()
{
	InternalShutdown();
}



UniquePtr<FramePackManager::ResourceType> FramePackManager::AllocateResourceFromFile( Filename const& filename )
{
	RF_ASSERT( mTextureManager != nullptr );

	// Open
	file::VFS const& vfs = *mVfs;
	file::FileHandlePtr fileHandle = vfs.GetFileForRead( filename );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to open framepack file" );
		return nullptr;
	}

	// Read into buffer
	std::vector<uint8_t> buffer;
	{
		FILE* const file = fileHandle->GetFile();
		RF_ASSERT( file != nullptr );

		// Get size
		int const seekSuccess = fseek( file, 0, SEEK_END );
		RF_ASSERT( seekSuccess == 0 );
		size_t numBytesInFile;
		{
			long const numBytesInFileLong = ftell( file );
			RF_ASSERT( numBytesInFileLong >= 0 );
			numBytesInFile = math::integer_cast<size_t>( numBytesInFileLong );
		}
		rewind( file );

		// Read
		buffer.resize( numBytesInFile );
		size_t const bytesRead = fread( buffer.data(), sizeof( uint8_t ), numBytesInFile, file );
		RF_ASSERT( bytesRead == numBytesInFile );
	}

	// Close
	fileHandle = nullptr;

	// Deserialize
	rftl::vector<file::VFSPath> textures;
	UniquePtr<gfx::FramePackBase> framePack;
	bool const readSuccess = gfx::FramePackSerDes::DeserializeFromBuffer( textures, buffer, framePack );
	if( readSuccess == false )
	{
		RFLOG_ERROR( filename, RFCAT_PPU, "Failed to deserialize FPack" );
		return nullptr;
	}
	buffer.clear();
	buffer.shrink_to_fit();

	TextureManager& texMan = *mTextureManager.Get();
	FramePackBase::TimeSlot* const timeSlots = framePack->GetMutableTimeSlots();
	size_t const numSlots = framePack->mNumTimeSlots;
	RF_ASSERT( textures.size() == numSlots );
	for( size_t i = 0; i < numSlots; i++ )
	{
		FramePackBase::TimeSlot& timeSlot = timeSlots[i];
		file::VFSPath const& texPath = textures[i];
		rftl::string const texName = file::VFS::CreateStringFromPath( texPath );
		ManagedTextureID const texID = texMan.LoadNewResourceGetID( texName, texPath );
		if( texID == kInvalidManagedTextureID )
		{
			RFLOG_ERROR( filename, RFCAT_PPU, "Failed to load texture for framepack" );
			timeSlot.m_TextureReference = kInvalidManagedTextureID;
			continue;
		}
		timeSlot.m_TextureReference = texID;
	}

	return framePack;
}

///////////////////////////////////////////////////////////////////////////////
}}

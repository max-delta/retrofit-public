#include "stdafx.h"
#include "SaveManager.h"

#include "cc3o3/save/SaveLoader.h"
#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

SaveManager::SaveNames SaveManager::FindSaveNames() const
{
	SaveNames retVal;

	file::VFS const& vfs = *app::gVfs;
	file::VFSPath const savesRoot = paths::UserSavesRoot();

	rftl::vector<file::VFSPath> files;
	rftl::vector<file::VFSPath> folders;
	vfs.EnumerateDirectory( savesRoot, file::VFSMount::Permissions::ReadOnly, files, folders );

	if( files.empty() == false )
	{
		RFLOG_WARNING( savesRoot, RFCAT_CC3O3, "Found unexpected files when enumerating save directory" );
	}

	// Saves are represented as folders that contain the save data
	for( file::VFSPath const& folder : folders )
	{
		RF_ASSERT( folder.Empty() == false );
		retVal.emplace( folder.GetLastElement() );
	}

	return retVal;
}



UniquePtr<SaveBlob> SaveManager::LoadSave( SaveName const& name ) const
{
	file::VFS const& vfs = *app::gVfs;
	file::VFSPath const saveRoot = paths::UserSavesRoot().GetChild( name );

	// Open file
	file::VFSPath const filePath = saveRoot.GetChild( "file.oo" );
	file::FileBuffer fileBuffer( ExplicitDefaultConstruct{} );
	{
		file::FileHandlePtr const fileHandle = vfs.GetFileForRead( filePath );
		RFLOG_TEST_AND_FATAL( fileHandle != nullptr, filePath, RFCAT_CC3O3, "Failed to open save for read" );
		fileBuffer = rftl::move( file::FileBuffer( *fileHandle, true ) );
		size_t const fileSize = fileBuffer.GetSize();
		RFLOG_TEST_AND_FATAL( fileSize > 10, filePath, RFCAT_CC3O3, "Unreasonably small file" );
		RFLOG_TEST_AND_FATAL( fileSize < 10'000, filePath, RFCAT_CC3O3, "Unreasonably large file" );
		RFLOG_TEST_AND_FATAL( fileBuffer.GetChars().size() == fileSize, filePath, RFCAT_CC3O3, "Unexpected file size after load, expected ASCII with no nulls" );
	}

	// Load
	SaveLoader const loader;
	return loader.LoadFromBuffer( fileBuffer.GetChars() );
}

///////////////////////////////////////////////////////////////////////////////
}

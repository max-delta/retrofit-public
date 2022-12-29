#include "stdafx.h"
#include "ResourceLoader.h"

#include "Logging/Logging.h"

#include "PlatformFilesystem/FileBuffer.h"
#include "PlatformFilesystem/VFS.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

ResourceLoader::ResourceLoader( WeakPtr<file::VFS const> vfs )
	: mVfs( vfs )
{
	//
}



ResourceLoader::~ResourceLoader() = default;



void ResourceLoader::AddResourceClass(
	ResourceTypeIdentifier typeID,
	char const* className )
{
	// TODO: Take in registry by WeakPtr, remove this wrapper function
	mTypeRegistry.AddResourceClass( typeID, className );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceLoader::InjectTypes(
	script::OOLoader& loader,
	ResourceTypeIdentifier typeID )
{
	ResourceTypeRecord::ClassInfos const classInfos = mTypeRegistry.GetClassInfos( typeID );

	for( ResourceTypeRecord::ClassInfos::value_type const& entry : classInfos )
	{
		rftl::string const& typeName = entry.first;
		reflect::ClassInfo const* const& classInfo = entry.second;
		RF_ASSERT( classInfo != nullptr );

		bool const injected = loader.InjectReflectedClassByClassInfo( *classInfo, typeName.c_str() );
		RFLOG_TEST_AND_FATAL( injected, nullptr, RFCAT_GAMERESOURCE, "Could not inject type '%s'", typeName.c_str() );
	}
}



bool ResourceLoader::AddSource(
	script::OOLoader& loader,
	file::VFSPath const& path )
{
	file::VFS const& vfs = *mVfs;

	// Open file
	file::FileBuffer fileBuffer( ExplicitDefaultConstruct{} );
	{
		file::FileHandlePtr const fileHandle = vfs.GetFileForRead( path );
		if( fileHandle == nullptr )
		{
			RFLOG_NOTIFY( path, RFCAT_GAMERESOURCE, "Failed to open resource for read" );
			return false;
		}
		fileBuffer = file::FileBuffer( *fileHandle, true );
		size_t const fileSize = fileBuffer.GetSize();
		RFLOG_TEST_AND_FATAL( fileSize > 10, path, RFCAT_GAMERESOURCE, "Unreasonably small file" );
		RFLOG_TEST_AND_FATAL( fileSize < 10'000, path, RFCAT_GAMERESOURCE, "Unreasonably large file" );
		RFLOG_TEST_AND_FATAL( fileBuffer.GetChars().size() == fileSize, path, RFCAT_GAMERESOURCE, "Unexpected file size after load, expected ASCII with no nulls" );
	}

	bool const success = loader.AddSourceFromBuffer( fileBuffer.GetChars() );
	RFLOG_TEST_AND_NOTIFY( success, path, RFCAT_GAMERESOURCE, "Failed to add source from file" );
	return success;
}



bool ResourceLoader::AddSource(
	script::OOLoader& loader,
	rftl::string_view buffer )
{
	bool const success = loader.AddSourceFromBuffer( buffer );
	RFLOG_TEST_AND_NOTIFY( success, nullptr, RFCAT_GAMERESOURCE, "Failed to add source from buffer" );
	return success;
}

///////////////////////////////////////////////////////////////////////////////
}

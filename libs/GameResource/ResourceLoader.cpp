#include "stdafx.h"
#include "ResourceLoader.h"

#include "GameResource/ResourceTypeRegistry.h"
#include "GameScripting/OOLoader.h"

#include "RFType/GlobalTypeDatabase.h"

#include "Logging/Logging.h"
#include "Serialization/AutoImporter.h"
#include "Serialization/ObjectDeserializer.h"

#include "PlatformFilesystem/FileBuffer.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/VFS.h"

#include "core_rftype/ConstructedType.h"

#include "core/ptr/default_creator.h"

#include "rftl/unordered_set"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

ResourceLoader::ResourceLoader(
	WeakPtr<file::VFS const> vfs,
	WeakPtr<ResourceTypeRegistry const> typeRegistry )
	: mVfs( vfs )
	, mTypeRegistry( typeRegistry )
{
	//
}



ResourceLoader::~ResourceLoader() = default;



bool ResourceLoader::PopulateClassFromFile(
	ResourceTypeIdentifier typeID,
	reflect::ClassInfo const& classInfo,
	void* classInstance,
	file::VFSPath const& path )
{
	if( ProbablyAnImporter( path ) )
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

			size_t const fileSize = fileHandle->GetFileSize();
			RFLOG_TEST_AND_FATAL( fileSize > serialization::AutoImporter::kLongestPeekMagicBytes, nullptr, RFCAT_SERIALIZATION, "Unreasonably small file" );
			RFLOG_TEST_AND_FATAL( fileSize < 10'000, nullptr, RFCAT_SERIALIZATION, "Unreasonably large file" );

			fileBuffer = file::FileBuffer( *fileHandle, false );
		}

		serialization::AutoImporter importer;

		bool const readSuccess = importer.ReadFromString( fileBuffer.GetChars() );
		if( readSuccess == false )
		{
			return false;
		}

		return PopulateClassViaImporter(
			importer,
			classInfo,
			classInstance );
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, path );
	if( sourceSuccess == false )
	{
		return false;
	}

	return PopulateClassViaOO(
		loader,
		kRootVariableName,
		classInfo,
		classInstance );
}



bool ResourceLoader::PopulateClassFromBuffer(
	ResourceTypeIdentifier typeID,
	reflect::ClassInfo const& classInfo,
	void* classInstance,
	rftl::string_view buffer )
{
	if( ProbablyAnImporter( buffer ) )
	{
		serialization::AutoImporter importer;

		bool const readSuccess = importer.ReadFromString( buffer );
		if( readSuccess == false )
		{
			return false;
		}

		return PopulateClassViaImporter(
			importer,
			classInfo,
			classInstance );
	}

	script::OOLoader loader;

	InjectTypes( loader, typeID );
	bool const sourceSuccess = AddSource( loader, buffer );
	if( sourceSuccess == false )
	{
		return false;
	}

	return PopulateClassViaOO(
		loader,
		kRootVariableName,
		classInfo,
		classInstance );
}

///////////////////////////////////////////////////////////////////////////////

bool ResourceLoader::ProbablyAnImporter(
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
		fileBuffer = file::FileBuffer( *fileHandle, serialization::AutoImporter::kLongestPeekMagicBytes );
	}

	return ProbablyAnImporter( fileBuffer.GetChars() );
}



bool ResourceLoader::ProbablyAnImporter(
	rftl::string_view buffer )
{
	return serialization::AutoImporter::LooksLikeSupportedType( buffer );
}



bool ResourceLoader::PopulateClassViaImporter(
	serialization::Importer& importer,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	// HACK: Testing
	RF_TODO_ANNOTATION( "Proper params" );
	serialization::ObjectDeserializer::Params HACK_params = {};

	// HACK: Hard-coded type testing
	ResourceTypeIdentifier typeID; // TODO: Param to this function
	( (void)typeID );
	RF_TODO_ANNOTATION( "Require non-invalid type ID" );
	RF_TODO_ANNOTATION( "Verify all loaded classes are allowed for the resource type" );
	HACK_params.mTypeLookupFunc = []( math::HashVal64 const& classKey ) -> reflect::ClassInfo const*
	{
		return rftype::GetGlobalTypeDatabase().LookupClassForKey( classKey );
	};
	HACK_params.mClassConstructFunc = []( reflect::ClassInfo const& classInfo ) -> rftype::ConstructedType
	{
		return rftype::GetGlobalTypeDatabase().ConstructClass( classInfo );
	};
	HACK_params.mContinueOnMissingTypeLookups = false;

	HACK_params.mAllowLocalIndirections = true;

	bool const deserializeSuccess = serialization::ObjectDeserializer::DeserializeSingleObject(
		importer,
		classInfo,
		classInstance,
		HACK_params );
	if( deserializeSuccess == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_GAMERESOURCE, "Failed to deserialize single object" );
		return false;
	}

	return true;
}



void ResourceLoader::InjectTypes(
	script::OOLoader& loader,
	ResourceTypeIdentifier typeID )
{
	ResourceTypeRegistry const& typeRegistry = *mTypeRegistry;

	// Determine what classes are allowed
	ResourceTypeRecord::ClassInfos const classInfos = typeRegistry.GetClassInfos( typeID );

	// Enable type construction
	{
		// Faster lookup table for shim to use
		rftl::unordered_set<reflect::ClassInfo const*> allowedClasses;
		for( ResourceTypeRecord::ClassInfos::value_type const& entry : classInfos )
		{
			allowedClasses.emplace( entry.second );
		}

		// Check validity in shim, and pass along
		auto classConstructFunc = [typeID, allowedClasses]( reflect::ClassInfo const& classInfo ) -> rftype::ConstructedType
		{
			if( allowedClasses.count( &classInfo ) <= 0 )
			{
				RFLOGF_ERROR( nullptr, RFCAT_GAMERESOURCE, "Class info not in allowed list for type {}", typeID );
				RF_DBGFAIL_MSG( "Class not in allow list" );
				return {};
			}
			return rftype::GetGlobalTypeDatabase().ConstructClass( classInfo );
		};
		bool const constructable = loader.AllowTypeConstruction( rftl::move( classConstructFunc ) );
		RFLOG_TEST_AND_FATAL( constructable, nullptr, RFCAT_GAMERESOURCE, "Could not enable type construction" );
	}

	// Inject the types
	for( ResourceTypeRecord::ClassInfos::value_type const& entry : classInfos )
	{
		rftl::string const& typeName = entry.first;
		reflect::ClassInfo const* const& classInfo = entry.second;
		RF_ASSERT( classInfo != nullptr );

		bool const injected = loader.InjectReflectedClassByClassInfo( *classInfo, typeName );
		RFLOGF_TEST_AND_FATAL( injected, nullptr, RFCAT_GAMERESOURCE, "Could not inject type '{}'", typeName );
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



bool ResourceLoader::PopulateClassViaOO(
	script::OOLoader& loader,
	char const* rootVariableName,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	return loader.PopulateClass(
		rootVariableName,
		classInfo,
		classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "ResourceSaver.h"

#include "GameResource/ResourceTypeRegistry.h"

#include "Serialization/ObjectSerializer.h"
#include "Serialization/DiagnosticExporter.h"
#include "Serialization/XmlExporter.h"

#include "Logging/Logging.h"

#include "PlatformFilesystem/FileBuffer.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/VFS.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

ResourceSaver::ResourceSaver(
	WeakPtr<file::VFS const> vfs,
	WeakPtr<ResourceTypeRegistry const> typeRegistry )
	: mVfs( vfs )
	, mTypeRegistry( typeRegistry )
{
	//
}



ResourceSaver::~ResourceSaver() = default;



bool ResourceSaver::SaveClassToFile(
	reflect::ClassInfo const& classInfo,
	void const* classInstance,
	ResourceTypeIdentifier typeID,
	file::VFSPath const& path )
{
	// HACK: Only doing simple single-object serialization right now
	RF_TODO_ANNOTATION( "Support complex pointer graphs" );

	( (void)typeID );
	RF_TODO_ANNOTATION( "Require non-invalid type ID" );
	RF_TODO_ANNOTATION( "Verify all saved classes are in registry so they can load later" );

	// Acquire file handle
	file::FileHandlePtr const fileHandle = mVfs->GetFileForWrite( path );
	if( fileHandle == nullptr )
	{
		RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to open resource for write" );
		return false;
	}

	// TODO: BinaryExporter
	// TODO: Figure out a better solution for filename extensions
	//static constexpr char kFileExtension[] = ".xml";
	using PlaceholderExporter = serialization::XmlExporter;
	rftl::string buffer;

	// Serialize
	buffer.clear();
	{
		// TODO: Remove
		{
			serialization::DiagnosticExporter diagExporter{ false };
			bool const diagSuccess = serialization::ObjectSerializer::SerializeSingleObject(
				diagExporter, classInfo, classInstance );
			RF_ASSERT( diagSuccess );
			bool const diagFinalSuccess = diagExporter.Root_FinalizeExport();
			RF_ASSERT( diagFinalSuccess );
		}

		PlaceholderExporter placeholderExporter = {};

		bool const serializeSuccess = serialization::ObjectSerializer::SerializeSingleObject(
			placeholderExporter, classInfo, classInstance );
		if( serializeSuccess == false )
		{
			RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to serialize resource for write" );
			return false;
		}

		bool const finalizeSuccess = placeholderExporter.Root_FinalizeExport();
		if( finalizeSuccess == false )
		{
			RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to finalize exporter for write" );
			return false;
		}

		bool const bufferSuccess = placeholderExporter.WriteToString( buffer );
		if( bufferSuccess == false )
		{
			RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to write resource to buffer" );
			return false;
		}
	}
	RF_ASSERT( buffer.empty() == false );

	// Write
	FILE* const file = fileHandle->GetFile();
	RF_ASSERT( file != nullptr );
	size_t const bytesWritten = fwrite( buffer.data(), sizeof( decltype( buffer )::value_type ), buffer.size(), file );
	if( bytesWritten != buffer.size() )
	{
		RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to write resource" );
		return false;
	}

	RFLOG_INFO( path, RFCAT_GAMERESOURCE, "Resource written" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

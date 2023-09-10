#include "stdafx.h"
#include "ResourceSaver.h"

#include "GameResource/ResourceTypeRegistry.h"

#include "Serialization/ObjectSerializer.h"
#include "Serialization/XmlExporter.h"

#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS )
	#include "Serialization/AutoImporter.h"
	#include "Serialization/DiagnosticExporter.h"
	#include "Serialization/DiagnosticImportTestShim.h"
#endif

#include "Logging/Logging.h"

#include "PlatformFilesystem/FileBuffer.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/VFS.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////
namespace details {

#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS )
// These diagnostics can get really spammy, but can still run some useful
//  checks even while silent
static constexpr bool kSilentDiagnosticOnExport = true;
static constexpr bool kSilentDiagnosticTestImport = true;
#endif

}
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
	// HACK: Testing
	// TODO: Proper params
	serialization::ObjectSerializer::Params HACK_params = {};

	// HACK: Only doing simple single-object serialization right now
	RF_TODO_ANNOTATION( "Support complex pointer graphs" );
	HACK_params.mInstanceID = 1;

	// HACK: Hard-coded type testing
	( (void)typeID );
	RF_TODO_ANNOTATION( "Require non-invalid type ID" );
	RF_TODO_ANNOTATION( "Verify all saved classes are in registry so they can load later" );
	HACK_params.mTypeID = 2;
	HACK_params.mTypeDebugName = "HACK";

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

#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS )
		// Diagnostic export
		{
			serialization::DiagnosticExporter diagExporter{ details::kSilentDiagnosticOnExport };
			bool const diagSuccess = serialization::ObjectSerializer::SerializeSingleObject(
				diagExporter, classInfo, classInstance, HACK_params );
			RF_ASSERT( diagSuccess );
			bool const diagFinalSuccess = diagExporter.Root_FinalizeExport();
			RF_ASSERT( diagFinalSuccess );
		}
#endif

		PlaceholderExporter placeholderExporter = {};

		bool const serializeSuccess = serialization::ObjectSerializer::SerializeSingleObject(
			placeholderExporter, classInfo, classInstance, HACK_params );
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

#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_SAVE_DIAGNOSTICS )
	// Diagnostic import
	{
		serialization::AutoImporter test = {};
		test.ReadFromString( buffer );
		bool const result = serialization::DiagnosticProcessImport( test, details::kSilentDiagnosticTestImport );
		RF_ASSERT( result );
	}
#endif

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

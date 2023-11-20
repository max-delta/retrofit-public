#include "stdafx.h"
#include "ResourceSaver.h"

#include "GameResource/ResourceTypeRegistry.h"

#include "RFType/GlobalTypeDatabase.h"

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

#include "core_rftype/StoredClassKey.h"


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
	RF_TODO_ANNOTATION( "Proper params" );
	serialization::ObjectSerializer::Params HACK_params = {};

	// HACK: Only doing limited multi-object local serialization right now
	HACK_params.mInstanceID = 1;
	HACK_params.mCollapseAllPossibleIndirections = true;
	serialization::exporter::InstanceID nextIDGen = 2;
	HACK_params.mInstanceIDGenerator = [&nextIDGen]() -> serialization::exporter::InstanceID
	{
		serialization::exporter::InstanceID const nextID = nextIDGen;
		nextIDGen++;
		return nextID;
	};

	// HACK: Hard-coded type testing
	( (void)typeID );
	RF_TODO_ANNOTATION( "Require non-invalid type ID" );
	RF_TODO_ANNOTATION( "Verify all saved classes are in registry so they can load later" );
	HACK_params.mTypeLookupFunc = []( reflect::ClassInfo const& classInfo ) -> rftype::StoredClassKey
	{
		return rftype::GetGlobalTypeDatabase().LookupKeyForClass( classInfo );
	};
	HACK_params.mContinueOnMissingTypeLookups = false;

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
			// NOTE: This sorta assumes that serializing is a non-mutating
			//  operation, so the diagnostic pass will match the true pass, but
			//  that might not be true, as accessors could secretly be
			//  self-mutating behind-the-scenes if they really wanted to be,
			//  but that is discouraged and should be rare in practice

			// Exporter
			serialization::DiagnosticExporter diagExporter{ details::kSilentDiagnosticOnExport };

			// Serialize (diagnostic only)
			bool const diagSuccess =
				serialization::ObjectSerializer::SerializeMultipleObjects(
					diagExporter,
					classInfo,
					classInstance,
					HACK_params );
			RF_ASSERT( diagSuccess );

			// Finalize
			bool const diagFinalSuccess = diagExporter.Root_FinalizeExport();
			RF_ASSERT( diagFinalSuccess );
		}
#endif

		// Exporter
		PlaceholderExporter placeholderExporter = {};

		// Serialize
		bool const serializeSuccess = serialization::ObjectSerializer::SerializeMultipleObjects(
			placeholderExporter,
			classInfo,
			classInstance,
			HACK_params );
		if( serializeSuccess == false )
		{
			RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to serialize resource for write" );
			return false;
		}

		// Finalize
		bool const finalizeSuccess = placeholderExporter.Root_FinalizeExport();
		if( finalizeSuccess == false )
		{
			RFLOG_ERROR( path, RFCAT_GAMERESOURCE, "Failed to finalize exporter for write" );
			return false;
		}

		// Write to buffer
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

	// Write to file
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

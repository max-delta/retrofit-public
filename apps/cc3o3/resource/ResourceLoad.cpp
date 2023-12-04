#include "stdafx.h"
#include "ResourceLoad.h"

#include "cc3o3/Common.h"
#include "cc3o3/campaign/CampaignDesc.h"
#include "cc3o3/campaign/RosterMemberDesc.h"
#include "cc3o3/char/CharData.h"
#include "cc3o3/char/ElementSlots.h"
#include "cc3o3/encounter/EncounterDesc.h"
#include "cc3o3/overworld/OverworldDesc.h"
#include "cc3o3/site/SiteDesc.h"
#include "cc3o3/save/SaveBlob.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameResource/ResourceTypeRegistry.h"
#include "GameResource/ResourceLoader.h"
#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_LOAD_DIAGNOSTICS )
	#include "GameResource/ResourceSaver.h"
#endif

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::resource {
///////////////////////////////////////////////////////////////////////////////
namespace details {

namespace type {
enum : ResourceTypeIdentifier
{
	// This list enumerates all of the top-level resources that can be loaded
	Invalid = kInvalidResourceTypeIdentifier,
	CampaignDesc,
	RosterMemberDesc,
	CharData,
	EncounterDesc,
	OverworldDesc,
	SaveBlob,
	Loadout,
	SiteDesc
};
}



template<typename T>
using ResourceCreator = DefaultCreator<T>;



template<typename T>
UniquePtr<T> LoadFromFile(
	ResourceTypeIdentifier typeID,
	file::VFSPath const& path )
{
	UniquePtr<T> retVal = gResourceLoader->LoadClassFromFile<T, ResourceCreator>( typeID, path );
	if( retVal == nullptr )
	{
		RFLOG_ERROR( path, RFCAT_CC3O3, "Failed to load class from file" );
	}

#if RF_IS_ALLOWED( RF_CONFIG_RESOURCE_LOAD_DIAGNOSTICS )
	{
		// Write out to buffer
		rftl::string buffer;
		bool const shimSuccess = gResourceSaver->SaveClassToBuffer( typeID, *retVal, buffer );
		if( shimSuccess == false )
		{
			RFLOG_FATAL( path, RFCAT_CC3O3, "Failed to re-save loaded class from file" );
		}

		// Load back in
		// NOTE: Not stomping return immediately, so that the original can be
		//  debugged alongside the buffer's contents
		UniquePtr<T> shimVal = gResourceLoader->LoadClassFromBuffer<T, ResourceCreator>( typeID, buffer );
		if( shimVal == nullptr )
		{
			RFLOG_FATAL( path, RFCAT_CC3O3, "Failed to load re-saved shim class" );
		}
		retVal = rftl::move( shimVal );
	}
#endif

	return retVal;
}



template<typename T>
rftl::vector<UniquePtr<T>> LoadFromDirectory(
	ResourceTypeIdentifier typeID,
	file::VFSPath const& path,
	bool recursive )
{
	file::VFS const& vfs = *app::gVfs;

	rftl::vector<UniquePtr<T>> retVal;

	// Find all the files
	rftl::vector<file::VFSPath> files;
	if( recursive )
	{
		vfs.EnumerateDirectoryRecursive( path, file::VFSMount::Permissions::ReadOnly, files );
	}
	else
	{
		rftl::vector<file::VFSPath> folders;
		vfs.EnumerateDirectory( path, file::VFSMount::Permissions::ReadOnly, files, folders );
	}

	// Load them all
	bool partialFailure = false;
	retVal.reserve( files.size() );
	for( file::VFSPath const& file : files )
	{
		UniquePtr<T> resource = LoadFromFile<T>( typeID, file );
		if( resource == nullptr )
		{
			// NOTE: Will still store the nullptr as an indicator of the issue,
			//  since a load failure could be prett catastrophic to downstream
			//  code that expected it
			partialFailure = true;
		}
		retVal.emplace_back( rftl::move( resource ) );
	}

	if( partialFailure )
	{
		if( recursive )
		{
			RFLOG_ERROR( path, RFCAT_CC3O3, "Failed to load one or more files in directory (recursive)" );
		}
		else
		{
			RFLOG_ERROR( path, RFCAT_CC3O3, "Failed to load one or more files in directory (non-recursive)" );
		}
	}

	return retVal;
}

}
///////////////////////////////////////////////////////////////////////////////

void InitializeLoader()
{
	ResourceTypeRegistry& typeRegistry = *gResourceTypeRegistry;

	{
		// This list specifies which classes can be deserialized when loading
		//  a given resource type
		using namespace details::type;

		typeRegistry.AddResourceType( CampaignDesc, "Campaign" );
		typeRegistry.AddResourceClass( CampaignDesc, "CampaignDesc" );

		typeRegistry.AddResourceType( RosterMemberDesc, "Roster" );
		typeRegistry.AddResourceClass( RosterMemberDesc, "RosterMemberDesc" );

		typeRegistry.AddResourceType( CharData, "Character" );
		typeRegistry.AddResourceClass( CharData, "CharData" );
		typeRegistry.AddResourceClass( CharData, "Description" );
		typeRegistry.AddResourceClass( CharData, "ElementSlots" );
		typeRegistry.AddResourceClass( CharData, "Equipment" );
		typeRegistry.AddResourceClass( CharData, "Genetics" );
		typeRegistry.AddResourceClass( CharData, "Stats" );
		typeRegistry.AddResourceClass( CharData, "Visuals" );

		typeRegistry.AddResourceType( EncounterDesc, "Encounter" );
		typeRegistry.AddResourceClass( EncounterDesc, "EncounterDesc" );
		typeRegistry.AddResourceClass( EncounterDesc, "EntityDesc" );

		typeRegistry.AddResourceType( OverworldDesc, "Overworld" );
		typeRegistry.AddResourceClass( OverworldDesc, "OverworldDesc" );
		typeRegistry.AddResourceClass( OverworldDesc, "AreaDesc" );

		typeRegistry.AddResourceType( SaveBlob, "Save" );
		typeRegistry.AddResourceClass( SaveBlob, "SaveBlob" );
		typeRegistry.AddResourceClass( SaveBlob, "SaveFileBlob" );

		typeRegistry.AddResourceType( Loadout, "Loadout" );
		typeRegistry.AddResourceClass( Loadout, "ElementSlots" );

		typeRegistry.AddResourceType( SiteDesc, "Site" );
		typeRegistry.AddResourceClass( SiteDesc, "SiteDesc" );
	}
}

///////////////////////////////////////////////////////////////////////////////

#define RF_LOADER_CONSTLESS( TYPE_ID, TYPE ) \
	template<> \
	UniquePtr<TYPE> LoadFromFile<TYPE>( file::VFSPath const& path ) \
	{ \
		return details::LoadFromFile<TYPE>( TYPE_ID, path ); \
	} \
	template<> \
	rftl::vector<UniquePtr<TYPE>> LoadFromDirectory<TYPE>( file::VFSPath const& path, bool recursive ) \
	{ \
		return details::LoadFromDirectory<TYPE>( TYPE_ID, path, recursive ); \
	}

#define RF_LOADER( TYPE_ID, TYPE ) \
	RF_LOADER_CONSTLESS( TYPE_ID, TYPE ); \
	RF_LOADER_CONSTLESS( TYPE_ID, TYPE const );

// This list correlates top-level resource types with their classes
RF_LOADER( details::type::CampaignDesc, campaign::CampaignDesc );
RF_LOADER( details::type::RosterMemberDesc, campaign::RosterMemberDesc );
RF_LOADER( details::type::CharData, character::CharData );
RF_LOADER( details::type::EncounterDesc, encounter::EncounterDesc );
RF_LOADER( details::type::OverworldDesc, overworld::OverworldDesc );
RF_LOADER( details::type::SaveBlob, save::SaveBlob );
RF_LOADER( details::type::Loadout, character::ElementSlots );
RF_LOADER( details::type::SiteDesc, site::SiteDesc );

#undef RF_LOADER
#undef RF_LOADER_CONSTLESS

///////////////////////////////////////////////////////////////////////////////
}

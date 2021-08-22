#include "stdafx.h"
#include "SaveManager.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/campaign/CampaignManager.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/resource/ResourceLoad.h"
#include "cc3o3/save/SaveBlob.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/Common.h"

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



UniquePtr<SaveBlob> SaveManager::LoadBlob( SaveName const& name ) const
{
	file::VFSPath const saveRoot = paths::UserSavesRoot().GetChild( name );
	file::VFSPath const filePath = saveRoot.GetChild( "file.oo" );
	return resource::LoadFromFile<SaveBlob>( filePath );
}



bool SaveManager::StoreBlob( SaveName const& name, SaveBlob const& blob )
{
	RF_TODO_BREAK();
	return true;
}



bool SaveManager::PerformInitialLoad( SaveName const& name )
{
	campaign::CampaignManager& campaign = *gCampaignManager;

	file::VFSPath const saveRoot = paths::UserSavesRoot().GetChild( name );

	// Read in campaign progress
	gCampaignManager->HardcodedPrepareCampaign( nullptr );
	gCampaignManager->HardcodedLoadCampaignProgress( nullptr );

	// Setup campaign
	// TODO: Use save data
	// TODO: Competitive multiplayer
	// TODO: Cooperative multiplayer
	campaign.HardcodedSinglePlayerCharacterLoad();
	campaign.HardcodedSinglePlayerObjectSetup();
	campaign.HardcodedSinglePlayerApplyProgression();

	// Read in loadouts
	// TODO: Sanitize? Or atleast warn on invalid setup
	gCompanyManager->ReadLoadoutsFromSave( saveRoot, appstate::InputHelpers::GetSinglePlayer() );

	RF_TODO_ANNOTATION( "Use the blob for system data?" );
	return LoadBlob( name ) != nullptr;
}



bool SaveManager::PerformStore( SaveName const& name )
{
	file::VFSPath const saveRoot = paths::UserSavesRoot().GetChild( name );

	// Save campaign progress
	gCampaignManager->HardcodedSaveCampaignProgress( nullptr );

	// Save loadouts
	gCompanyManager->WriteLoadoutsToSave( saveRoot, appstate::InputHelpers::GetSinglePlayer() );

	RF_TODO_ANNOTATION( "Build the blob from system data" );
	return StoreBlob( name, SaveBlob{} );
}

///////////////////////////////////////////////////////////////////////////////
}

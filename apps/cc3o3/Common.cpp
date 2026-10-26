#include "stdafx.h"
#include "Common.h"

#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/casting/CastingEngine.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/encounter/EncounterManager.h"
#include "cc3o3/save/SaveManager.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/campaign/CampaignManager.h"
#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameResource/ResourceTypeRegistry.h"
#include "GameResource/ResourceLoader.h"
#include "GameResource/ResourceSaver.h"
#include "GameSync/RollbackInputManager.h"
#include "GameSprite/CharacterCreator.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFS.h"
#include "PPU/PaletteLoader.h"
#include "PPU/PPUController.h"
#include "Rollback/RollbackManager.h"

#include "core_component/TypedObjectManager.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF::cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
inline namespace globals {
constinit WeakPtr<rollback::RollbackManager> gRollbackManager;
constinit WeakPtr<sync::RollbackInputManager> gRollbackInputManager;
constinit WeakPtr<resource::ResourceTypeRegistry> gResourceTypeRegistry;
constinit WeakPtr<resource::ResourceLoader> gResourceLoader;
constinit WeakPtr<resource::ResourceSaver> gResourceSaver;
constinit WeakPtr<sprite::CharacterCreator> gCharacterCreator;
constinit WeakPtr<character::CharacterValidator> gCharacterValidator;
constinit WeakPtr<character::CharacterDatabase> gCharacterDatabase;
constinit WeakPtr<combat::CombatEngine> gCombatEngine;
constinit WeakPtr<element::ElementDatabase> gElementDatabase;
constinit WeakPtr<cast::CastingEngine> gCastingEngine;
constinit WeakPtr<company::CompanyManager> gCompanyManager;
constinit WeakPtr<encounter::EncounterManager> gEncounterManager;
constinit WeakPtr<state::ObjectManager> gObjectManager;
constinit WeakPtr<save::SaveManager> gSaveManager;
constinit WeakPtr<campaign::CampaignManager> gCampaignManager;
static constinit UniquePtr<rollback::RollbackManager> sRollbackManager;
static constinit UniquePtr<sync::RollbackInputManager> sRollbackInputManager;
static constinit UniquePtr<resource::ResourceTypeRegistry> sResourceTypeRegistry;
static constinit UniquePtr<resource::ResourceLoader> sResourceLoader;
static constinit UniquePtr<resource::ResourceSaver> sResourceSaver;
static constinit UniquePtr<sprite::CharacterCreator> sCharacterCreator;
static constinit UniquePtr<character::CharacterValidator> sCharacterValidator;
static constinit UniquePtr<character::CharacterDatabase> sCharacterDatabase;
static constinit UniquePtr<combat::CombatEngine> sCombatEngine;
static constinit UniquePtr<element::ElementDatabase> sElementDatabase;
static constinit UniquePtr<cast::CastingEngine> sCastingEngine;
static constinit UniquePtr<company::CompanyManager> sCompanyManager;
static constinit UniquePtr<encounter::EncounterManager> sEncounterManager;
static constinit UniquePtr<state::ObjectManager> sObjectManager;
static constinit UniquePtr<save::SaveManager> sSaveManager;
static constinit UniquePtr<campaign::CampaignManager> sCampaignManager;
}

///////////////////////////////////////////////////////////////////////////////

void SystemStartup( init::StartupConfig const& config )
{
	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "System startup" );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Mounting cc3o3 VFS layers..." );
	file::VFSPath const mountFile = paths::VfsConfig();
	bool const vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( file::kMountPriorityLowest, mountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load cc3o3 mount file" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Configuring cc3o3 graphics..." );
	gfx::Palette4a5_16 globalPalette = {};
	bool const paletteLoaded = gfx::PaletteLoader::LoadPalette( globalPalette, *app::gVfs, paths::GlobalPalette() );
	if( paletteLoaded == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load global palette" );
	}
	app::gGraphics->ReplaceGlobalPalette( globalPalette );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing rollback manager..." );
	sRollbackManager = DefaultCreator<rollback::RollbackManager>::Create();
	gRollbackManager = sRollbackManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing rollback input..." );
	sRollbackInputManager = DefaultCreator<sync::RollbackInputManager>::Create( gRollbackManager );
	gRollbackInputManager = sRollbackInputManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing resource type registry..." );
	sResourceTypeRegistry = DefaultCreator<resource::ResourceTypeRegistry>::Create();
	gResourceTypeRegistry = sResourceTypeRegistry;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing resource loader..." );
	sResourceLoader = DefaultCreator<resource::ResourceLoader>::Create( app::gVfs, gResourceTypeRegistry );
	gResourceLoader = sResourceLoader;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing resource saver..." );
	sResourceSaver = DefaultCreator<resource::ResourceSaver>::Create( app::gVfs, gResourceTypeRegistry );
	gResourceSaver = sResourceSaver;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing character creator..." );
	sCharacterCreator = DefaultCreator<sprite::CharacterCreator>::Create( app::gVfs, app::gGraphics );
	gCharacterCreator = sCharacterCreator;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing character validator..." );
	sCharacterValidator = DefaultCreator<character::CharacterValidator>::Create( app::gVfs, gCharacterCreator );
	gCharacterValidator = sCharacterValidator;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing character database..." );
	sCharacterDatabase = DefaultCreator<character::CharacterDatabase>::Create();
	gCharacterDatabase = sCharacterDatabase;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing combat engine..." );
	sCombatEngine = DefaultCreator<combat::CombatEngine>::Create( app::gVfs );
	gCombatEngine = sCombatEngine;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing element database..." );
	sElementDatabase = DefaultCreator<element::ElementDatabase>::Create( app::gVfs );
	gElementDatabase = sElementDatabase;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing casting engine..." );
	sCastingEngine = DefaultCreator<cast::CastingEngine>::Create( app::gVfs, gCombatEngine, gElementDatabase );
	gCastingEngine = sCastingEngine;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing company manager..." );
	sCompanyManager = DefaultCreator<company::CompanyManager>::Create( app::gVfs, gElementDatabase );
	gCompanyManager = sCompanyManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing encounter manager..." );
	sEncounterManager = DefaultCreator<encounter::EncounterManager>::Create();
	gEncounterManager = sEncounterManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing object manager..." );
	sObjectManager = DefaultCreator<state::ObjectManager>::Create(
		component::ManagerIdentifier( 1u ), component::ScopeIdentifier( 1u ), state::ComponentResolver() );
	gObjectManager = sObjectManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing save manager..." );
	sSaveManager = DefaultCreator<save::SaveManager>::Create();
	gSaveManager = sSaveManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing campaign manager..." );
	sCampaignManager = DefaultCreator<campaign::CampaignManager>::Create();
	gCampaignManager = sCampaignManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "System startup complete" );
}



void SystemShutdown()
{
	sCampaignManager = nullptr;
	sSaveManager = nullptr;
	sObjectManager = nullptr;
	sEncounterManager = nullptr;
	sCompanyManager = nullptr;
	sCastingEngine = nullptr;
	sElementDatabase = nullptr;
	sCombatEngine = nullptr;
	sCharacterDatabase = nullptr;
	sCharacterValidator = nullptr;
	sCharacterCreator = nullptr;
	sResourceSaver = nullptr;
	sResourceLoader = nullptr;
	sResourceTypeRegistry = nullptr;
	sRollbackInputManager = nullptr;
	sRollbackManager = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "Common.h"

#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/combat/CombatEngine.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSync/RollbackInputManager.h"
#include "GameSprite/CharacterCreator.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFS.h"
#include "Rollback/RollbackManager.h"

#include "core_component/TypedObjectManager.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF::cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
WeakPtr<rollback::RollbackManager> gRollbackManager;
WeakPtr<sync::RollbackInputManager> gRollbackInputManager;
WeakPtr<sprite::CharacterCreator> gCharacterCreator;
WeakPtr<character::CharacterValidator> gCharacterValidator;
WeakPtr<character::CharacterDatabase> gCharacterDatabase;
WeakPtr<combat::CombatEngine> gCombatEngine;
WeakPtr<element::ElementDatabase> gElementDatabase;
WeakPtr<company::CompanyManager> gCompanyManager;
WeakPtr<state::ObjectManager> gObjectManager;
static UniquePtr<rollback::RollbackManager> sRollbackManager;
static UniquePtr<sync::RollbackInputManager> sRollbackInputManager;
static UniquePtr<sprite::CharacterCreator> sCharacterCreator;
static UniquePtr<character::CharacterValidator> sCharacterValidator;
static UniquePtr<character::CharacterDatabase> sCharacterDatabase;
static UniquePtr<combat::CombatEngine> sCombatEngine;
static UniquePtr<element::ElementDatabase> sElementDatabase;
static UniquePtr<company::CompanyManager> sCompanyManager;
static UniquePtr<state::ObjectManager> sObjectManager;

///////////////////////////////////////////////////////////////////////////////

void SystemStartup()
{
	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "System startup" );

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Mounting cc3o3 VFS layers..." );
	file::VFSPath const mountFile = paths::VfsConfig();
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( mountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load cc3o3 mount file" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing rollback manager..." );
	sRollbackManager = DefaultCreator<rollback::RollbackManager>::Create();
	gRollbackManager = sRollbackManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing rollback input..." );
	sRollbackInputManager = DefaultCreator<sync::RollbackInputManager>::Create( gRollbackManager );
	gRollbackInputManager = sRollbackInputManager;

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

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing company manager..." );
	sCompanyManager = DefaultCreator<company::CompanyManager>::Create( app::gVfs, gElementDatabase );
	gCompanyManager = sCompanyManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "Initializing object manager..." );
	sObjectManager = DefaultCreator<state::ObjectManager>::Create(
		component::ManagerIdentifier( 1u ), component::ScopeIdentifier( 1u ), state::ComponentResolver() );
	gObjectManager = sObjectManager;

	RFLOG_MILESTONE( nullptr, RFCAT_STARTUP, "System startup complete" );
}



void SystemShutdown()
{
	sObjectManager = nullptr;
	sCompanyManager = nullptr;
	sElementDatabase = nullptr;
	sCharacterDatabase = nullptr;
	sCharacterValidator = nullptr;
	sCharacterCreator = nullptr;
	sRollbackInputManager = nullptr;
	sRollbackManager = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

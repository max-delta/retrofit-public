#include "stdafx.h"
#include "Common.h"

#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/company/CompanyManager.h"
#include "cc3o3/elements/ElementDatabase.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCreator.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFS.h"

#include "core_component/TypedObjectManager.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
WeakPtr<sprite::CharacterCreator> gCharacterCreator;
WeakPtr<character::CharacterValidator> gCharacterValidator;
WeakPtr<character::CharacterDatabase> gCharacterDatabase;
WeakPtr<company::CompanyManager> gCompanyManager;
WeakPtr<element::ElementDatabase> gElementDatabase;
WeakPtr<state::ObjectManager> gObjectManager;
static UniquePtr<sprite::CharacterCreator> sCharacterCreator;
static UniquePtr<character::CharacterValidator> sCharacterValidator;
static UniquePtr<character::CharacterDatabase> sCharacterDatabase;
static UniquePtr<company::CompanyManager> sCompanyManager;
static UniquePtr<element::ElementDatabase> sElementDatabase;
static UniquePtr<state::ObjectManager> sObjectManager;

///////////////////////////////////////////////////////////////////////////////

void SystemStartup()
{
	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "System start" );

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Mounting cc3o3 VFS layers..." );
	file::VFSPath const mountFile = paths::VfsConfig();
	bool vfsTestDataLoad = app::gVfs->AttemptSubsequentMount( mountFile );
	if( vfsTestDataLoad == false )
	{
		RFLOG_FATAL( mountFile, RFCAT_STARTUP, "Can't load cc3o3 mount file" );
	}

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing character creator..." );
	sCharacterCreator = DefaultCreator<sprite::CharacterCreator>::Create( app::gVfs, app::gGraphics );
	gCharacterCreator = sCharacterCreator;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing character validator..." );
	sCharacterValidator = DefaultCreator<character::CharacterValidator>::Create( app::gVfs, gCharacterCreator );
	gCharacterValidator = sCharacterValidator;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing character database..." );
	sCharacterDatabase = DefaultCreator<character::CharacterDatabase>::Create();
	gCharacterDatabase = sCharacterDatabase;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing company manager..." );
	sCompanyManager = DefaultCreator<company::CompanyManager>::Create( app::gVfs );
	gCompanyManager = sCompanyManager;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing element database..." );
	sElementDatabase = DefaultCreator<element::ElementDatabase>::Create( app::gVfs );
	gElementDatabase = sElementDatabase;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing object manager..." );
	sObjectManager = DefaultCreator<state::ObjectManager>::Create(
		component::ManagerIdentifier( 1u ), component::ScopeIdentifier( 1u ), state::ComponentResolver() );
	gObjectManager = sObjectManager;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "System startup complete" );
}



void SystemShutdown()
{
	sObjectManager = nullptr;
	sElementDatabase = nullptr;
	sCompanyManager = nullptr;
	sCharacterDatabase = nullptr;
	sCharacterValidator = nullptr;
	sCharacterCreator = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

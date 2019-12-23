#include "stdafx.h"
#include "Common.h"

#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/char/CharacterValidator.h"
#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSprite/CharacterCreator.h"

#include "Logging/Logging.h"
#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
WeakPtr<sprite::CharacterCreator> gCharacterCreator;
WeakPtr<character::CharacterValidator> gCharacterValidator;
WeakPtr<character::CharacterDatabase> gCharacterDatabase;
static UniquePtr<sprite::CharacterCreator> sCharacterCreator;
static UniquePtr<character::CharacterValidator> sCharacterValidator;
static UniquePtr<character::CharacterDatabase> sCharacterDatabase;

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

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "System startup complete" );
}



void SystemShutdown()
{
	sCharacterDatabase = nullptr;
	sCharacterValidator = nullptr;
	sCharacterCreator = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

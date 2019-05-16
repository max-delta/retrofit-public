#include "stdafx.h"
#include "Common.h"

#include "cc3o3/char/CharacterDatabase.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"
#include "core/ptr/unique_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
WeakPtr<character::CharacterDatabase> gCharacterDatabase;
static UniquePtr<character::CharacterDatabase> sCharacterDatabase;

///////////////////////////////////////////////////////////////////////////////

void SystemStartup()
{
	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "System start" );

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "Initializing character database..." );
	sCharacterDatabase = DefaultCreator<character::CharacterDatabase>::Create();
	gCharacterDatabase = sCharacterDatabase;

	RFLOG_MILESTONE( nullptr, RFCAT_CC3O3, "System startup complete" );
}



void SystemShutdown()
{
	sCharacterDatabase = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

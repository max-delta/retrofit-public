#include "stdafx.h"
#include "SaveLoader.h"

#include "cc3o3/save/SaveBlob.h"

#include "GameScripting/OOLoader.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::save {
///////////////////////////////////////////////////////////////////////////////

UniquePtr<SaveBlob> SaveLoader::LoadFromBuffer( rftl::string_view buffer ) const
{
	script::OOLoader loader;

	// Inject types
	{
		bool success;
		success = loader.InjectReflectedClassByCompileType<SaveFileBlob>( "SaveFileBlob" );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to inject" );
		success = loader.InjectReflectedClassByCompileType<SaveBlob>( "SaveBlob" );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to inject" );
	}

	// Inject source
	{
		bool success;
		success = loader.AddSourceFromBuffer( buffer );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to load" );
	}

	// Extract blob
	UniquePtr<SaveBlob> retVal = DefaultCreator<SaveBlob>::Create();
	{
		bool const success = loader.PopulateClass( "save", *retVal );
		RFLOG_TEST_AND_FATAL( success, nullptr, RFCAT_CC3O3, "Failed to populate" );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

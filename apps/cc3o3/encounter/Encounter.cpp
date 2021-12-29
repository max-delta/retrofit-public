#include "stdafx.h"
#include "Encounter.h"

#include "cc3o3/encounter/EncounterDesc.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "PlatformFilesystem/VFSPath.h"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

Encounter Encounter::LoadFromDesc( file::VFSPath const& descPath )
{
	Encounter retVal = {};

	UniquePtr<EncounterDesc const> descPtr = resource::LoadFromFile<EncounterDesc const>( descPath );
	RFLOG_TEST_AND_FATAL( descPtr != nullptr, descPath, RFCAT_CC3O3, "Failed to load encounter desc" );
	EncounterDesc const& desc = *descPtr;

	// Entities
	retVal.mEntities.reserve( desc.mEntities.size() );
	for( EntityDesc const& entityDesc : desc.mEntities )
	{
		Entity entity = {};
		entity.mTODO = entityDesc.mTODO;
		retVal.mEntities.emplace_back( rftl::move( entity ) );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "cc3o3/encounter/Entity.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/vector"


namespace RF::cc::encounter {
///////////////////////////////////////////////////////////////////////////////

class Encounter
{
	//
	// Types and constants
public:
	using Entities = rftl::vector<Entity>;


	//
	// Public methods
public:
	Encounter() = default;

	static Encounter LoadFromDesc( file::VFSPath const& descPath );


	//
	// Public data
public:
	Entities mEntities;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "Localization/LocFwd.h"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocEngine
{
	//
	// Public methods
public:
	TextDirection GetTextDirection() const;

	LocResult Query( LocQuery const& query ) const;


	//
	// Private data
private:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}}

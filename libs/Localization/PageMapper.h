#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API PageMapper
{
	//
	// Public methods
public:
	rftl::string MapTo8Bit( rftl::u32string const& codePoints ) const;


	//
	// Private data
private:
	// TODO
};

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "cc3o3/options/OptionsFwd.h"

#include "core/macros.h"

#include "rftl/string_view"


namespace RF::cc::options {
///////////////////////////////////////////////////////////////////////////////

class OptionLogic
{
	RF_NO_INSTANCE( OptionLogic );

public:
	static bool FindListIdentifier( size_t& index, OptionDesc const& desc, rftl::string_view identifier );
	static bool CycleList( OptionValue& value, OptionDesc const& desc, bool forward );
};

///////////////////////////////////////////////////////////////////////////////
}

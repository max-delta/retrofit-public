#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"

#include "rftl/vector"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class InputHelpers final
{
	RF_NO_INSTANCE( InputHelpers );

public:
	static rftl::vector<ui::FocusEventType> GetMainMenuInputToProcess();
};

///////////////////////////////////////////////////////////////////////////////
}}}

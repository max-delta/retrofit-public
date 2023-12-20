#pragma once
#include "project.h"

#include "GameAction/Context.h"

#include "rftl/string"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Generic useless error, best avoided in favor a contexts with richer data
class GAMEACTION_API GenericErrorContext final : public Context
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	virtual bool IsATerminalError() const override;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "GenericErrorContext.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::act::GenericErrorContext )
{
	RFTYPE_META().BaseClass<RF::act::Context>();
}

namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

bool GenericErrorContext::IsATerminalError() const
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

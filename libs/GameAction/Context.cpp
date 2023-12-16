#include "stdafx.h"
#include "Context.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::act::Context )
{
	// No meta information, only used for virtual casting
}

namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

Context::Context() = default;
Context::Context( Context const& rhs ) = default;
Context::~Context() = default;



bool Context::IsATerminalError() const
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

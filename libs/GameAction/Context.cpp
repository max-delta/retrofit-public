#include "stdafx.h"
#include "Context.h"


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

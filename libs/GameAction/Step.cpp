#include "stdafx.h"
#include "Step.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::act::Step )
{
	using RF::act::Step;
	RFTYPE_REGISTER_BY_NAME( "Step" );
}

namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

Step::Step() = default;
Step::~Step() = default;

///////////////////////////////////////////////////////////////////////////////
}

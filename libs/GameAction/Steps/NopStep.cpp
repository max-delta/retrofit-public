#include "stdafx.h"
#include "NopStep.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::act::NopStep )
{
	RFTYPE_META().BaseClass<RF::act::Step>();
	RFTYPE_REGISTER_BY_NAME( "NopStep" );
	RFTYPE_REGISTER_DEFAULT_CREATOR();
}

namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

UniquePtr<Context> NopStep::Execute( Environment const& env, Context& ctx ) const
{
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

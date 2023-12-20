#include "stdafx.h"
#include "CombatContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::cast::CombatContext )
{
	RFTYPE_META().BaseClass<RF::act::Context>();
}

namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

CombatContext::CombatContext(
	combat::CombatInstance const& combatInstance )
	: mCombatInstance( combatInstance )
{
	//
}



UniquePtr<act::Context> CombatContext::Clone() const
{
	return DefaultCreator<CombatContext>::Create( *this );
}

///////////////////////////////////////////////////////////////////////////////
}

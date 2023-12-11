#include "stdafx.h"
#include "Environment.h"

#include "GameAction/ActionDatabase.h"
#include "GameAction/ConditionDatabase.h"

#include "core/ptr/default_creator.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

Environment::Environment()
	: mActionDatabase( DefaultCreator<ActionDatabase>::Create() )
	, mConditionDatabase( DefaultCreator<ConditionDatabase>::Create() )
{
	//
}



ActionDatabase const& Environment::GetActionDatabase() const
{
	return *mActionDatabase;
}



ActionDatabase& Environment::GetMutableActionDatabase()
{
	return *mActionDatabase;
}



ConditionDatabase const& Environment::GetConditionDatabase() const
{
	return *mConditionDatabase;
}



ConditionDatabase& Environment::GetMutableConditionDatabase()
{
	return *mConditionDatabase;
}

///////////////////////////////////////////////////////////////////////////////
}

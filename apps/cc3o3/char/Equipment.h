#pragma once
#include "project.h"

#include "rftl/string"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct Equipment
{
	rftl::string mArmor1 = {};
	rftl::string mArmor2 = {};
	rftl::string mAccessory1 = {};
	rftl::string mAccessory2 = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

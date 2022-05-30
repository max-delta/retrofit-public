#pragma once
#include "project.h"

#include "rftl/string"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct Visuals
{
	bool mCompositable = false;
	rftl::string mBase = {};
	rftl::string mTop = {};
	rftl::string mBottom = {};
	rftl::string mHair = {};
	rftl::string mSpecies = {};
};

///////////////////////////////////////////////////////////////////////////////
}

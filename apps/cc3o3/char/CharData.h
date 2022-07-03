#pragma once
#include "project.h"

#include "cc3o3/char/Description.h"
#include "cc3o3/char/Genetics.h"
#include "cc3o3/char/Visuals.h"
#include "cc3o3/char/Stats.h"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

struct CharData
{
	// TODO: Better solution for enum<->string (probably in C++20)
	rftl::string mEntityClass = {};

	Description mDescription = {};
	rftl::string mInnate = {};
	Genetics mGenetics = {};
	Visuals mVisuals = {};
	Stats mStats = {};
};

///////////////////////////////////////////////////////////////////////////////
}

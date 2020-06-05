#pragma once
#include "project.h"

#include "cc3o3/char/Description.h"
#include "cc3o3/char/Genetics.h"
#include "cc3o3/char/Visuals.h"
#include "cc3o3/char/Stats.h"
#include "cc3o3/char/Equipment.h"
#include "cc3o3/char/ElementSlots.h"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct CharData
{
	Description mDescription = {};
	rftl::string mInnate = {};
	Genetics mGenetics = {};
	Visuals mVisuals = {};
	Stats mStats = {};
	Equipment mEquipment = {};
	ElementSlots mInnateElements = {};
	ElementSlots mEquippedElements = {};
};

///////////////////////////////////////////////////////////////////////////////
}}}

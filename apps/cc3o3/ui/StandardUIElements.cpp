#include "stdafx.h"
#include "StandardUIElements.h"

#include "PPU/FramePack.h"
#include "PPU/FramePackManager.h"
#include "PPU/PPUController.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

FramePackDef QueryFramePackDef(
	gfx::ppu::PPUController const& ppu,
	FramePackProto const& proto )
{
	return QueryFramePackDef(
		*ppu.GetFramePackManager(),
		proto );
}

FramePackDef QueryFramePackDef(
	gfx::ppu::FramePackManager const& framePackMan,
	FramePackProto const& proto )
{
	gfx::ppu::ManagedFramePackID const fPackID = framePackMan.GetManagedResourceIDFromResourceName( proto.mResourceName );
	RF_ASSERT( fPackID != gfx::ppu::kInvalidManagedFramePackID );
	WeakPtr<gfx::ppu::FramePackBase const> const fPackPtr = framePackMan.GetResourceFromManagedResourceID( fPackID );
	RFLOG_TEST_AND_FATAL( fPackPtr != nullptr, proto.mResourceName, RFCAT_CC3O3, "Expected a frame pack to already be loaded, and it is not" );
	gfx::ppu::FramePackBase const& fPack = *fPackPtr;

	FramePackDef const retVal = {
		.mResourceName = proto.mResourceName,
		.mManagedID = fPackID,
		.mMaxTimeIndex = fPack.CalculateTimeIndexBoundary(),
		.mPreferredSlowdownRate = fPack.mPreferredSlowdownRate,
		.mExpectedWidth = proto.mExpectedWidth,
		.mExpectedHeight = proto.mExpectedHeight,
	};

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

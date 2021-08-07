#include "stdafx.h"
#include "Texture.h"

#include "Timing/FrameClock.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

Texture::Texture()
	: mLastUsedInFrame( time::CommonClock::time_point() )
{
	//
}



Texture::~Texture()
{
	RF_ASSERT( mDeviceRepresentation == kInvalidDeviceTextureID );
}



DeviceTextureID Texture::GetDeviceRepresentation() const
{
	UpdateFrameUsage();
	return mDeviceRepresentation;
}



uint32_t Texture::DebugGetWidth() const
{
	return mWidthPostLoad;
}



uint32_t Texture::DebugGetHeight() const
{
	return mHeightPostLoad;
}



void Texture::UpdateFrameUsage() const
{
	mLastUsedInFrame = time::FrameClock::now();
}

///////////////////////////////////////////////////////////////////////////////
}

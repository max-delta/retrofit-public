#include "stdafx.h"
#include "Texture.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

Texture::Texture()
	: mLastUsedInFrame( time::FrameClock::time_point() )
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
}}

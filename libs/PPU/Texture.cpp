#include "stdafx.h"
#include "Texture.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

Texture::Texture()
	: m_LastUsedInFrame( time::FrameClock::time_point() )
	, m_DeviceRepresentation( k_InvalidDeviceTextureID )
{
	//
}



Texture::~Texture()
{
	RF_ASSERT( m_DeviceRepresentation == k_InvalidDeviceTextureID );
}



DeviceTextureID Texture::GetDeviceRepresentation() const
{
	UpdateFrameUsage();
	return m_DeviceRepresentation;
}



uint32_t Texture::DebugGetWidth() const
{
	return m_WidthPostLoad;
}



uint32_t Texture::DebugGetHeight() const
{
	return m_HeightPostLoad;
}



void Texture::UpdateFrameUsage() const
{
	m_LastUsedInFrame = time::FrameClock::now();
}

///////////////////////////////////////////////////////////////////////////////
}}

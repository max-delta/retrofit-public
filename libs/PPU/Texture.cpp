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



void Texture::UpdateFrameUsage() const
{
	m_LastUsedInFrame = time::FrameClock::now();
}

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "Texture.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

FrameID Texture::s_MostRecentFrame = k_FirstFrameID;

///////////////////////////////////////////////////////////////////////////////

Texture::Texture()
	: m_LastUsedInFrame( k_InvalidFrameID )
	, m_DeviceRepresentation( k_InvalidTextureID )
{
	//
}



Texture::~Texture()
{
	RF_ASSERT( m_DeviceRepresentation == k_InvalidTextureID );
}



TextureID Texture::GetDeviceRepresentation() const
{
	UpdateFrameUsage();
	return m_DeviceRepresentation;
}



void Texture::UpdateFrameUsage() const
{
	m_LastUsedInFrame = s_MostRecentFrame;
}

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Texture
{
	RF_NO_COPY(Texture);
public:
	Texture();
	Texture( Texture && ) = default;
	~Texture();
	Texture& operator =( Texture && ) = default;
	DeviceTextureID GetDeviceRepresentation() const;

private:
	void UpdateFrameUsage() const;
	friend class TextureManager;
	static FrameID s_MostRecentFrame;
	mutable FrameID m_LastUsedInFrame;
	DeviceTextureID m_DeviceRepresentation;
};

///////////////////////////////////////////////////////////////////////////////
}}

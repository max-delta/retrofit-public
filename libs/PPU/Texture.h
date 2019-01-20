#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "Timing/clocks.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API Texture
{
	RF_NO_COPY( Texture );

private:
	friend class PPUController;
	friend class TextureManager;

public:
	Texture();
	Texture( Texture&& ) = default;
	~Texture();
	Texture& operator=( Texture&& ) = default;
	DeviceTextureID GetDeviceRepresentation() const;

	uint32_t DebugGetWidth() const;
	uint32_t DebugGetHeight() const;


private:
	void UpdateFrameUsage() const;
	friend class TextureManager;
	mutable time::FrameClock::time_point m_LastUsedInFrame;
	DeviceTextureID m_DeviceRepresentation;
	uint32_t m_WidthPostLoad;
	uint32_t m_HeightPostLoad;
};

///////////////////////////////////////////////////////////////////////////////
}}

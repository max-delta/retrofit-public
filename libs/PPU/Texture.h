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
	friend class FontManager;

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
	mutable time::FrameClock::time_point mLastUsedInFrame;
	DeviceTextureID mDeviceRepresentation = kInvalidDeviceTextureID;
	uint32_t mWidthPostLoad;
	uint32_t mHeightPostLoad;
};

///////////////////////////////////////////////////////////////////////////////
}}

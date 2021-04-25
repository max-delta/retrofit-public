#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core_math/AABB4.h"
#include "core_math/Color3f.h"
#include "core_platform/shim/win_shim.h"

#include "rftl/array"
#include "rftl/extension/byte_view.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class DeviceInterface
{
	RF_NO_COPY( DeviceInterface );

public:
	DeviceInterface() = default;
	virtual ~DeviceInterface() = default;

	virtual bool AttachToWindow( shim::HWND hWnd ) = 0;
	virtual bool DetachFromWindow() = 0;
	virtual bool Initialize2DGraphics() = 0;

	virtual bool SetSurfaceSize( uint16_t width, uint16_t height ) = 0;
	virtual bool SetBackgroundColor( math::Color3f color ) = 0;
	virtual bool SetFontScale( float scale ) = 0;

	virtual DeviceTextureID LoadTexture( rftl::byte_view const& buffer, uint32_t& width, uint32_t& height ) = 0;
	virtual bool UnloadTexture( DeviceTextureID textureID ) = 0;

	virtual DeviceFontID CreateBitmapFont( rftl::byte_view const& buffer, uint32_t& characterWidth, uint32_t& characterHeight, rftl::array<uint32_t, 256>* variableWidth ) = 0;
	virtual bool DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color ) = 0;
	virtual bool DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color, math::AABB4f texUV ) = 0;

	virtual bool DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z ) = 0;
	virtual bool DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z, math::AABB4f texUV ) = 0;

	virtual bool DebugRenderText( math::Vector2f pos, float z, const char* fmt, ... ) = 0;
	virtual bool DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float z, float width, math::Color3f color ) = 0;

	virtual bool BeginFrame() = 0;
	virtual bool RenderFrame() = 0;
	virtual bool EndFrame() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core_math/vector2.h"
#include "core_platform/win_shim.h"

#include <stdio.h>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class DeviceInterface
{
public:
	virtual ~DeviceInterface();

	virtual bool AttachToWindow( shim::HWND hWnd ) = 0;
	virtual bool DetachFromWindow() = 0;
	virtual bool Initialize2DGraphics() = 0;

	virtual bool SetSurfaceSize( uint16_t width, uint16_t height ) = 0;
	virtual bool SetBackgroundColor( float r, float g, float b, float a ) = 0;
	virtual bool SetFontScale( float scale ) = 0;

	virtual DeviceTextureID LoadTexture( FILE* file, uint32_t& width, uint32_t& height ) = 0;
	virtual bool UnloadTexture( DeviceTextureID textureID ) = 0;

	virtual bool DrawBillboard( DeviceTextureID textureID, math::Vector2f topLeft, math::Vector2f bottomRight, float z ) = 0;

	virtual bool DebugRenderText( math::Vector2f pos, const char *fmt, ... ) = 0;
	virtual bool DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float width ) = 0;

	virtual bool BeginFrame() = 0;
	virtual bool RenderFrame() = 0;
	virtual bool EndFrame() = 0;
};

///////////////////////////////////////////////////////////////////////////////
inline DeviceInterface::~DeviceInterface()
{
	//
}
///////////////////////////////////////////////////////////////////////////////
}}

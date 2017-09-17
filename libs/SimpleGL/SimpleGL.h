#pragma once
#include "project.h"

#include "PPU/DeviceInterface.h"

//#ifdef SIMPLEGL_EXPORTS
	class SIMPLEGL_API RF::gfx::DeviceInterface;
//#endif

namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class SIMPLEGL_API SimpleGL final : public DeviceInterface
{
public:
	enum class ProjectionMode
	{
		TRUE_BUFFER_00UPLEFT, // A unit is a buffer pixel
		NDC01_00UPLEFT, // NDC 0<>1
		NDC01_00DWNLEFT, // NDC 0<>1
		NDC11_11UPRIGHT, // NDC -1<>1
	};

public:
	bool AttachToWindow( shim::HWND hWnd ) override;
	bool DetachFromWindow() override;
	bool Initialize2DGraphics() override;

	bool SetProjectionMode( ProjectionMode mode );
	bool SetSurfaceSize( uint16_t width, uint16_t height ) override;
	bool SetBackgroundColor( float r, float g, float b, float a ) override;
	bool SetFontScale( float scale ) override;

	DeviceTextureID LoadTexture( FILE* file, uint32_t& width, uint32_t& height ) override;
	bool UnloadTexture( DeviceTextureID textureID ) override;

	bool glPrint( char const* fmt, ... );
	bool glPrint( char const* fmt, va_list args );

	bool DebugRenderText( math::Vector2f pos, const char *fmt, ... ) override;
	bool DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float width ) override;
	bool DrawBillboard( DeviceTextureID textureID, math::Vector2f topLeft, math::Vector2f bottomRight, float z ) override;

	bool BeginFrame();
	bool RenderFrame();
	bool EndFrame();

private:
	unsigned int font_base;
	void BuildFont( int8_t height );

private:
	shim::HWND hWnd;
	shim::HDC hDC;
	shim::HGLRC hRC;
	ProjectionMode projectionMode;
	int width;
	int height;
	float xFudge; // HACK: Pixel fudge for anti-aliasing repair
	float yFudge; // HACK: Pixel fudge for anti-aliasing repair
};

///////////////////////////////////////////////////////////////////////////////
}}

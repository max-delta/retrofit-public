#pragma once
#include "project.h"

#include "core_platform/win_shim.h"

namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

// TODO: Shared in core math
struct Vector2f
{
	Vector2f( float x = 0, float y = 0 ) : x( x ), y( y )
	{
	}
	float x;
	float y;
};

///////////////////////////////////////////////////////////////////////////////

class SIMPLEGL_API SimpleGL
{
public:
	bool AttachToWindow( shim::HWND hWnd );
	bool DetachFromWindow();

	bool SetSurfaceSize( int width = 640, int height = 480 );
	bool SetBackgroundColor( float r = 1.f, float g = 0.f, float b = 0.f, float a = 0.5f );

	bool glPrint( const char *fmt, ... );

	bool DrawLine( Vector2f p0, Vector2f p1 );

	bool BeginFrame();
	bool RenderFrame();
	bool EndFrame();

private:
	unsigned int font_base;
	void BuildFont();

	shim::HWND hWnd;
	shim::HDC hDC;
	shim::HGLRC hRC;
	int width;
	int height;
};

///////////////////////////////////////////////////////////////////////////////
}}

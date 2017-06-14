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
	enum class ProjectionMode
	{
		TRUE_BUFFER_00UPLEFT, // A unit is a buffer pixel
		NDC01_00UPLEFT, // NDC 0<>1
		NDC01_00DWNLEFT, // NDC 0<>1
		NDC11_11UPRIGHT, // NDC -1<>1
	};

public:
	bool AttachToWindow( shim::HWND hWnd );
	bool DetachFromWindow();

	bool SetProjectionMode( ProjectionMode mode );
	bool SetSurfaceSize( int width = 640, int height = 480 );
	bool SetBackgroundColor( float r = 1.f, float g = 0.f, float b = 0.f, float a = 0.5f );

	unsigned int LoadTexture( char const* filename );
	bool UnloadTexture( unsigned int textureID );

	bool glPrint( const char *fmt, ... );

	bool DrawLine( Vector2f p0, Vector2f p1 );
	bool DrawBillboard( unsigned int textureID, Vector2f topLeft, Vector2f bottomRight, float z = 0.f );

	bool BeginFrame();
	bool RenderFrame();
	bool EndFrame();

private:
	unsigned int font_base;
	void BuildFont();

private:
	shim::HWND hWnd;
	shim::HDC hDC;
	shim::HGLRC hRC;
	ProjectionMode projectionMode;
	int width;
	int height;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "SimpleGL.h"

#include "core/macros.h"
#include "core_math/math_casts.h"

#include "core_platform/gdi_shim.h"
#include "core_platform/gl_inc.h"
#pragma comment(lib, "opengl32.lib")

#include "stb_image/stb_image.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

bool SimpleGL::AttachToWindow( shim::HWND hWnd )
{
	this->hWnd = hWnd;

	hDC = shim::GetDC( hWnd ); // Grab the window's device context. If anything else
	                           // in the currently running proccess besides Graphics
	                           // tries to modify the device context, Windows responds
	                           // with "Step off, bitch! Can't touch dis!"

	shim::PIXELFORMATDESCRIPTOR pfd = {};
	memset( &pfd, 0, sizeof( pfd ) ); // Clear all the crap in the structure.
	pfd.nSize = sizeof( pfd ); // The size of the structure.
	pfd.nVersion = 1; // The version of this structure format.
	pfd.dwFlags = shim::kPFD_DRAW_TO_WINDOW | // This buffer can map directly to a window.
		shim::kPFD_SUPPORT_OPENGL | // This buffer can be used by OpenGL. (Kinda important)
		shim::kPFD_DOUBLEBUFFER; // This buffer is double-buffered.
	pfd.iPixelType = shim::kPFD_TYPE_RGBA; // Pixels will use RGBA-style data.
	pfd.cColorBits = 24; // 24-bit colors
	pfd.cDepthBits = 16; // Number of depth layers per pixel.
	pfd.iLayerType = shim::kPFD_MAIN_PLANE; // Supposedly ignored.

	int iFormat = shim::ChoosePixelFormat( hDC, &pfd ); // Try to find an appropriate
	                                                    // pixel format to meet our demands.
	shim::SetPixelFormat( hDC, iFormat, &pfd ); // Set the chosen pixel format.

	hRC = shim::wglCreateContext( hDC ); // Create a render context for OpenGL.
	shim::wglMakeCurrent( hDC, hRC ); // All calls to OpenGL will use this context.

	SetFontScale( 1.f );

	return true;
}

bool SimpleGL::DetachFromWindow()
{
	glDeleteLists( font_base, 96 ); // Delete the font list.
	shim::wglMakeCurrent( nullptr, nullptr ); // All calls to OpenGL will be ignored.
	shim::wglDeleteContext( hRC ); // Discard the old render context.
	shim::ReleaseDC( hWnd, hDC ); // Release control of the device context.
	return true;
}

bool SimpleGL::Initialize2DGraphics()
{
	SetProjectionMode( RF::gfx::SimpleGL::ProjectionMode::NDC01_00UPLEFT );
	return true;
}

bool SimpleGL::SetProjectionMode( ProjectionMode mode )
{
	projectionMode = mode;
	return true;
}

bool SimpleGL::SetSurfaceSize( uint16_t width, uint16_t height )
{
	this->width = width;
	this->height = height;

	glViewport( 0, 0, width, height ); // Reset The Current Viewport

	glMatrixMode( GL_PROJECTION ); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix

	switch( projectionMode )
	{
		case RF::gfx::SimpleGL::ProjectionMode::TRUE_BUFFER_00UPLEFT:
			glOrtho(0.0f,width,height,0.0f,-1.0f,1.0f); // Create Ortho View (0,0 At Top Left)
			xFudge = 0.5f;
			yFudge = 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC01_00UPLEFT:
			glOrtho( 0.0f, 1, 1, 0.0f, -1.0f, 1.0f ); // Create Ortho View (0,0 At Top Left)
			xFudge = 1.f / width * 0.5f;
			yFudge = 1.f / height * 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC01_00DWNLEFT:
			glOrtho( 0.0f, 1, 0.0f, 1, -1.0f, 1.0f ); // Create Ortho View (0,0 At Bottom Left)
			xFudge = 1.f / width * 0.5f;
			yFudge = 1.f / height * 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC11_11UPRIGHT:
			glOrtho( -1, 1, -1, 1, -1.0f, 1.0f ); // Create Ortho View (1,1 At Top Right)
			xFudge = 2.f / width * 0.5f;
			yFudge = 2.f / height * 0.5f;
			break;
		default:
			glOrtho( 0.0f, 1, 1, 0.0f, -1.0f, 1.0f );
			xFudge = 1.f / width * 0.5f;
			yFudge = 1.f / height * 0.5f;
			break;
	}

	glMatrixMode( GL_MODELVIEW ); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix



	glPolygonMode( GL_FRONT, GL_FILL ); //The front faces of polygons will be
									  //drawn filled.
	glPolygonMode( GL_BACK, GL_FILL ); //The back faces of polygons will be
									 //drawn filled.
	glShadeModel( GL_SMOOTH ); //Individual vertexes in a polygon can have
							 //individual color values. These values are
							 //then smoothed across the polygon in relation
							 //to a point's proximity to a vertex.

	glEnable( GL_TEXTURE_2D ); // Enable Texture Mapping

	glEnable( GL_POINT_SMOOTH ); //Enable antialiasing for points.
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST ); //For deciding how to calculate
											 //antialiasing for points, we
											 //prefer the nicest possible
											 //algorithms.

	glEnable( GL_LINE_SMOOTH ); //Enable antialiasing for lines.
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST ); //For deciding how to calculate
											//antialiasing for lines, we
											//prefer the nicest possible
											//algorithms.

	glEnable( GL_BLEND ); //Enable alpha-blending. Note that this is also required
						//for antialiasing.
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); //When we blend two
													   //colors together, alpha calculations will be done
													   //based on the overlapping color's alpha value.
	return true;
}

bool SimpleGL::SetBackgroundColor( float r, float g, float b, float a )
{
	glClearColor( r, g, b, a ); //When we call Clear, it will clear to this color
	return true;
}

bool SimpleGL::SetFontScale( float scale )
{
	BuildFont( math::integer_cast<int8_t>( 8 * scale ) );
	return true;
}

DeviceTextureID SimpleGL::LoadTexture( FILE* file, uint32_t& width, uint32_t& height )
{
	int x, y, n;
	unsigned char* data = stbi_load_from_file( file, &x, &y, &n, 4 );
	RF_ASSERT( data != nullptr );
	if( data == nullptr )
	{
		return 0;
	}

	width = math::integer_cast<uint32_t>( x );
	height = math::integer_cast<uint32_t>( y );

	unsigned int retVal = 0;
	glGenTextures( 1, &retVal );
	glBindTexture( GL_TEXTURE_2D, retVal );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	stbi_image_free( data );
	return retVal;
}

bool SimpleGL::UnloadTexture( DeviceTextureID textureID )
{
	GLuint const texID = static_cast<GLuint>( textureID );
	glDeleteTextures( 1, &texID );
	return true;
}

bool SimpleGL::DebugRenderText( math::Vector2f pos, const char * fmt, ... )
{
	glColor3f( 0, 0, 0 );
	glRasterPos2f( pos.x, pos.y );
	va_list args;
	va_start( args, fmt );
	bool retVal = glPrint( fmt, args );
	va_end( args );
	return retVal;
}

bool SimpleGL::DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float width )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	glColor3f( 0, 0, 0 );
	if( width < 1 )
	{
		glLineWidth( 1.1f ); // HACK: Make diagonals fatter, but not horizontal or vertical
		glBegin( GL_LINES );
		glVertex2f( p0.x + xFudge, p0.y + yFudge );
		glVertex2f( p1.x + xFudge, p1.y + yFudge );
		glEnd();
	}
	else
	{
		glLineWidth( width );
		glBegin( GL_LINES );
		glVertex2f( p0.x - xFudge * width, p0.y - yFudge * width );
		glVertex2f( p1.x - xFudge * width, p1.y - yFudge * width );
		glEnd();
	}
	return true;
}

bool SimpleGL::DrawBillboard( DeviceTextureID textureID, math::Vector2f topLeft, math::Vector2f bottomRight, float z )
{
	GLuint const texID = static_cast<GLuint>( textureID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glColor3f( 1, 1, 1 );
	float const left = topLeft.x;
	float const top = topLeft.y;
	float const right = bottomRight.x;
	float const bottom = bottomRight.y;
	glBegin(GL_QUADS);
	{
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( left, top, z );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( right, top, z );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f( right, bottom, z );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( left, bottom, z );
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}

bool SimpleGL::BeginFrame()
{
	shim::wglMakeCurrent( hDC, hRC );
	glClear( GL_COLOR_BUFFER_BIT ); //Clear the buffer
	glPushMatrix();
	return true;
}

bool SimpleGL::RenderFrame()
{
	glColor3f( 1, 1, 1 );
	#ifdef SIMPLEGL_DBG_GRID
	{
		for( float horizontal = 0; horizontal <= 1; horizontal += 0.1f )
		{
			glBegin( GL_LINES );
			glVertex2d( horizontal, 0 );
			glVertex2d( horizontal, 1 );
			glEnd();
		}
		for( float vertical = 0; vertical <= 1; vertical += 0.1f )
		{
			glBegin( GL_LINES );
			glVertex2d( 0, vertical );
			glVertex2d( 1, vertical );
			glEnd();
		}
	}
	#endif
	glFlush(); //Makes sure that all buffered commands are sent to the
				//graphics card. Note that this does NOT insure that the
				//commands were executed, only that they have been sent.
	return true;
}

bool SimpleGL::EndFrame()
{
	glPopMatrix();
	shim::SwapBuffers( hDC );
	return true;
}

void SimpleGL::BuildFont( int8_t height )					// Build Our Bitmap Font
{
	shim::HFONT font;						// Windows Font ID
	shim::HFONT oldfont;					// Used For Good House Keeping

	glDeleteLists( font_base, 96 );
	font_base = glGenLists( 96 );					// Storage For 96 Characters ( NEW )
	font = shim::CreateFontW(
		-height/*-12*/, // Height Of Font
		0, // Width Of Font
		0, // Angle Of Escapement
		0, // Orientation Angle
		shim::kFW_BOLD, // Font Weight
		false, // Italic
		false, // Underline
		false, // Strikeout
		shim::kANSI_CHARSET, // Character Set Identifier
		shim::kOUT_TT_PRECIS, // Output Precision
		shim::kCLIP_DEFAULT_PRECIS, // Clipping Precision
		shim::kANTIALIASED_QUALITY, // Output Quality
		shim::kFF_DONTCARE | shim::kDEFAULT_PITCH, // Family And Pitch
		L"Arial" ); // Font Name
	oldfont = (shim::HFONT)shim::SelectObject( hDC, font );		// Selects The Font We Want
	shim::wglUseFontBitmapsW( hDC, 32, 96, font_base );			// Builds 96 Characters Starting At Character 32
														//wglUseFontOutlines(hDC, 32, 96, base, 0, 1,WGL_FONT_LINES, 0);			// Builds 96 Characters Starting At Character 32
	shim::SelectObject( hDC, oldfont );				// Selects The Font We Want
	shim::DeleteObject( font );					// Delete The Font
}

bool SimpleGL::glPrint( char const* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool retVal = glPrint( fmt, args );
	va_end( args );
	return retVal;
}

bool SimpleGL::glPrint( char const* fmt, va_list args )
{
	char text[256];
	if( fmt == nullptr )
		return false;
	vsprintf_s( text, fmt, args );
	glPushAttrib( GL_LIST_BIT ); // Pushes The Display List Bits
	glListBase( font_base - 32 ); // Sets The Base Character to 32
	glCallLists( (GLsizei)strlen( text ), GL_UNSIGNED_BYTE, text );	// Draws The Display List Text
	glPopAttrib(); // Pops The Display List Bits
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}

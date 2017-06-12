#include "stdafx.h"
#include "SimpleGL.h"

#include "core_platform/gdi_shim.h"
#include "core_platform/gl_inc.h"
#pragma comment(lib, "opengl32.lib")

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

	BuildFont();

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

bool SimpleGL::SetSurfaceSize( int width, int height )
{
	this->width = width;
	this->height = height;

	glViewport( 0, 0, width, height ); // Reset The Current Viewport

	glMatrixMode( GL_PROJECTION ); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix

	//glOrtho(0.0f,width,height,0.0f,-1.0f,1.0f); // Create Ortho View (0,0 At Top Left)
	glOrtho( 0.0f, 1, 1, 0.0f, -1.0f, 1.0f ); // Create Ortho View (0,0 At Top Left)

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

bool SimpleGL::DrawLine( Vector2f p0, Vector2f p1 )
{
	glColor3f( 0, 0, 0 );
	glBegin( GL_LINES );
	glVertex2f( p0.x, p0.y );
	glVertex2f( p1.x, p1.y );
	glEnd();
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

void SimpleGL::BuildFont()					// Build Our Bitmap Font
{
	shim::HFONT font;						// Windows Font ID
	shim::HFONT oldfont;					// Used For Good House Keeping

	font_base = glGenLists( 96 );					// Storage For 96 Characters ( NEW )
	font = shim::CreateFontW(
		-10/*-12*/, // Height Of Font
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
		L"Courier New" ); // Font Name
	oldfont = (shim::HFONT)shim::SelectObject( hDC, font );		// Selects The Font We Want
	shim::wglUseFontBitmapsW( hDC, 32, 96, font_base );			// Builds 96 Characters Starting At Character 32
														//wglUseFontOutlines(hDC, 32, 96, base, 0, 1,WGL_FONT_LINES, 0);			// Builds 96 Characters Starting At Character 32
	shim::SelectObject( hDC, oldfont );				// Selects The Font We Want
	shim::DeleteObject( font );					// Delete The Font
}

bool SimpleGL::glPrint( const char *fmt, ... )				// Custom GL "Print" Routine
{
	char		text[256];				// Holds Our String
	va_list		ap;					// Pointer To List Of Arguments
	if( fmt == nullptr )					// If There's No Text
		return false;						// Do Nothing
	va_start( ap, fmt );					// Parses The String For Variables
	vsprintf_s( text, fmt, ap );				// And Converts Symbols To Actual Numbers
	va_end( ap );						// Results Are Stored In Text
	glPushAttrib( GL_LIST_BIT );				// Pushes The Display List Bits		( NEW )
	glListBase( font_base - 32 );					// Sets The Base Character to 32	( NEW )
	glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );	// Draws The Display List Text	( NEW )
	glPopAttrib();						// Pops The Display List Bits	( NEW )
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}

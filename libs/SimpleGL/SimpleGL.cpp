#include "stdafx.h"
#include "SimpleGL.h"

#include "core/macros.h"
#include "core_math/math_casts.h"

#include "core_platform/shim/gdi_shim.h"
#include "core_platform/inc/gl_inc.h"
#pragma comment( lib, "opengl32.lib" )

#include "stb_image/stb_image.h"

#include "rftl/cstdio"
#include "rftl/cstdarg"
#include "rftl/string"
#include "rftl/string_view"
#include "rftl/vector"
#include "rftl/array"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////
namespace details {

void ConsumeErrors()
{
	GLenum const error = glGetError();
	if( error != GL_NO_ERROR )
	{
		rftl::abort();
	}
}
#define CONSUME_ERRORS() ::RF::gfx::details::ConsumeErrors()
#define CANT_ERROR()
#define API_ERROR_BUG() // Can't call glGetError() here

}
///////////////////////////////////////////////////////////////////////////////

bool SimpleGL::AttachToWindow( shim::HWND hWnd )
{
	this->mHWnd = hWnd;

	// Grab the windows's device context
	// NOTE: May cause errors if something else in the process also calls this
	mHDC = shim::GetDC( hWnd );

	shim::PIXELFORMATDESCRIPTOR pfd = {};
	memset( &pfd, 0, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags =
		shim::kPFD_DRAW_TO_WINDOW | // This buffer can map directly to a window
		shim::kPFD_SUPPORT_OPENGL | // This buffer can be used by OpenGL
		shim::kPFD_DOUBLEBUFFER; // This buffer is double-buffered
	pfd.iPixelType = shim::kPFD_TYPE_RGBA;
	pfd.cColorBits = 24; // 24-bit colors
	pfd.cDepthBits = 16; // Number of depth layers per pixel
	pfd.iLayerType = shim::kPFD_MAIN_PLANE; // Supposedly ignored

	// Try to find an appropriate pixel format to meet our demands
	int const iFormat = shim::ChoosePixelFormat( mHDC, &pfd );
	if( iFormat == 0 )
	{
		return false;
	}

	// Set the chosen pixel format
	bool const pixelFormatSuccess = shim::SetPixelFormat( mHDC, iFormat, &pfd );
	if( pixelFormatSuccess == false )
	{
		return false;
	}

	// Create a render context for OpenGL
	mHRC = shim::wglCreateContext( mHDC );
	if( mHRC == nullptr )
	{
		return false;
	}

	// All calls to OpenGL will use this context
	bool const makeCurrentSuccess = shim::wglMakeCurrent( mHDC, mHRC );
	if( makeCurrentSuccess == false )
	{
		return false;
	}

	return true;
}



bool SimpleGL::DetachFromWindow()
{
	bool totalSuccess = true;

	// Delete the font list
	glDeleteLists( font_base, 96 );
	CONSUME_ERRORS();

	// All calls to OpenGL will be ignored
	bool const makeCurrentSuccess = shim::wglMakeCurrent( nullptr, nullptr );
	if( makeCurrentSuccess == false )
	{
		totalSuccess = false;
	}

	// Discard the old render context
	bool const deleteContextSuccess = shim::wglDeleteContext( mHRC );
	if( deleteContextSuccess == false )
	{
		totalSuccess = false;
	}

	// Release control of the device context
	int const releaseResult = shim::ReleaseDC( mHWnd, mHDC );
	if( releaseResult != 1 )
	{
		totalSuccess = false;
	}

	return totalSuccess;
}



rftl::string SimpleGL::GetAttachedDeviceDescription() const
{
	GLubyte const* const versionStringBytes = glGetString( GL_VERSION );
	CONSUME_ERRORS();
	if( versionStringBytes == nullptr )
	{
		return "SimpleGL - Invalid version string";
	}

	rftl::string_view const versionString( reinterpret_cast<char const*>( versionStringBytes ) );
	return rftl::string( "SimpleGL - " ).append( versionString );
}



bool SimpleGL::Initialize2DGraphics()
{
	{
		// Oldest version check support, gets a UTF-8 string that begins with
		//  the major number followed by a dot, and then quickly degenerates
		//  into a bunhc of vendor-specific stuff past that
		GLubyte const* const versionStringBytes = glGetString( GL_VERSION );
		CONSUME_ERRORS();
		if( versionStringBytes == nullptr )
		{
			fprintf( stderr, "Invalid OpenGL version string\n" );
			return false;
		}
		rftl::string_view const versionString( reinterpret_cast<char const*>( versionStringBytes ) );
		if( versionString.size() < 2 )
		{
			fprintf( stderr, "Unsupported OpenGL version string\n" );
			return false;
		}
		if( versionString.at( 0 ) == '1' && versionString.at( 1 ) == '.' )
		{
			// Specifically want to avoid anything before OpenGL 2.0, since we
			//  want to make use of non-power-of-two (NPOT) textures
			fprintf( stderr, "Unsupported OpenGL version: %s\n", versionStringBytes );
			return false;
		}
	}

	SetFontScale( 1.f );

	SetProjectionMode( RF::gfx::SimpleGL::ProjectionMode::NDC01_00UPLEFT );
	return true;
}



bool SimpleGL::SetProjectionMode( ProjectionMode mode )
{
	mProjectionMode = mode;
	return true;
}



bool SimpleGL::SetSurfaceSize( uint16_t width, uint16_t height )
{
	this->mWidth = width;
	this->mHeight = height;
	float const widthf = math::float_cast<float>( width );
	float const heightf = math::float_cast<float>( height );

	// Reset The current viewport
	glViewport( 0, 0, width, height );
	CONSUME_ERRORS();

	// Select and reset the texture matrix
	glMatrixMode( GL_TEXTURE );
	CONSUME_ERRORS();
	glLoadIdentity();
	CONSUME_ERRORS();

	// Select and reset the projection matrix
	glMatrixMode( GL_PROJECTION );
	CONSUME_ERRORS();
	glLoadIdentity();
	CONSUME_ERRORS();

	// WARNING: glOrtho has TERRIBLE parameters
	// SEE: https://www.opengl.org/discussion_boards/showthread.php/176234-glortho-zNear-and-zFar
	//  "Remember that the near and far values used by glOrtho are only
	//   distances and (assuming that the eye is located at the origin) sets up
	//   a projection where -nearVal is on the front clipping plane and -farVal
	//   is the location of the far clipping plane."
	// "So given an statement like:
	//  glOrtho( 0, sizeX, sizeY, 0, 0, 1 );
	//  you render close to the far clipping plane with a z-value of -0.9999999
	//  and render close to the near clipping plane with a z-value of
	//  -0.000001, respectively."
	switch( mProjectionMode )
	{
		case RF::gfx::SimpleGL::ProjectionMode::TRUE_BUFFER_00UPLEFT:
			// Create Ortho View (0,0 At Top Left)
			glOrtho( 0, width, height, 0, 1, -1 );
			CONSUME_ERRORS();
			mXFudge = 0.5f;
			mYFudge = 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC01_00UPLEFT:
			// Create Ortho View (0,0 At Top Left)
			glOrtho( 0, 1, 1, 0, 1, -1 );
			CONSUME_ERRORS();
			mXFudge = 1.f / widthf * 0.5f;
			mYFudge = 1.f / heightf * 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC01_00DWNLEFT:
			// Create Ortho View (0,0 At Bottom Left)
			glOrtho( 0, 1, 0, 1, 1, -1 );
			CONSUME_ERRORS();
			mXFudge = 1.f / widthf * 0.5f;
			mYFudge = 1.f / heightf * 0.5f;
			break;
		case RF::gfx::SimpleGL::ProjectionMode::NDC11_11UPRIGHT:
			// Create Ortho View (1,1 At Top Right)
			glOrtho( -1, 1, -1, 1, 1, -1 );
			CONSUME_ERRORS();
			mXFudge = 2.f / widthf * 0.5f;
			mYFudge = 2.f / heightf * 0.5f;
			break;
		default:
			glOrtho( 0, 1, 1, 0, 1, -1 );
			CONSUME_ERRORS();
			mXFudge = 1.f / widthf * 0.5f;
			mYFudge = 1.f / heightf * 0.5f;
			break;
	}

	// Select and reset the modelview matrix
	glMatrixMode( GL_MODELVIEW );
	CONSUME_ERRORS();
	glLoadIdentity();
	CONSUME_ERRORS();

	// The front faces of polygons will be drawn filled
	glPolygonMode( GL_FRONT, GL_FILL );
	CONSUME_ERRORS();

	//The back faces of polygons will be drawn filled
	glPolygonMode( GL_BACK, GL_FILL );
	CONSUME_ERRORS();

	// Individual vertexes in a polygon can have individual color values. These
	//  values are then smoothed across the polygon in relation to a point's
	//  proximity to a vertex.
	glShadeModel( GL_SMOOTH );
	CONSUME_ERRORS();

	// Enable texture mapping
	glEnable( GL_TEXTURE_2D );
	CONSUME_ERRORS();

	// Enable depth-buffer
	// NOTE: May not be needed if depth-sorting is performed before raster
	glDepthMask( GL_TRUE );
	CONSUME_ERRORS();

	// Enable depth-testing
	// NOTE: May not be needed if depth-sorting is performed before raster
	glEnable( GL_DEPTH_TEST );
	CONSUME_ERRORS();

	// Passes if the incoming depth value is GL_LESS than the stored depth
	//  value
	// NOTE: May not be needed if depth-sorting is performed before raster
	glDepthFunc( GL_LESS );
	CONSUME_ERRORS();

	// Enable antialiasing for points
	glEnable( GL_POINT_SMOOTH );
	CONSUME_ERRORS();

	// For deciding how to calculate antialiasing for points, we prefer the
	//  nicest possible algorithms
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	CONSUME_ERRORS();

	// Enable antialiasing for lines
	glEnable( GL_LINE_SMOOTH );
	CONSUME_ERRORS();

	// For deciding how to calculate antialiasing for lines, we prefer the
	//  nicest possible algorithms
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	CONSUME_ERRORS();

	// Enable alpha-blending. Note that this is also required for antialiasing.
	glEnable( GL_BLEND );
	CONSUME_ERRORS();

	// When we blend two colors together, alpha calculations will be done based
	//  on the overlapping color's alpha value
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	CONSUME_ERRORS();

	return true;
}



bool SimpleGL::SetBackgroundColor( math::Color3f color )
{
	// When we call Clear, it will clear to this color
	glClearColor( color.r, color.g, color.b, 1.f );
	CONSUME_ERRORS();
	return true;
}



bool SimpleGL::SetFontScale( float scale )
{
	BuildFont( math::integer_cast<int8_t>( 8 * scale ) );
	return true;
}



DeviceTextureID SimpleGL::LoadTexture( rftl::byte_view const& buffer, uint32_t& width, uint32_t& height )
{
	int x, y, n;
	unsigned char* data = stbi_load_from_memory( reinterpret_cast<stbi_uc const*>( buffer.data() ), math::integer_cast<int>( buffer.size() ), &x, &y, &n, 4 );
	RF_ASSERT( data != nullptr );
	if( data == nullptr )
	{
		return 0;
	}

	width = math::integer_cast<uint32_t>( x );
	height = math::integer_cast<uint32_t>( y );

	unsigned int retVal = 0;
	glGenTextures( 1, &retVal );
	CONSUME_ERRORS();
	glBindTexture( GL_TEXTURE_2D, retVal );
	CONSUME_ERRORS();
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
	CONSUME_ERRORS();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	CONSUME_ERRORS();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	CONSUME_ERRORS();
	stbi_image_free( data );
	return retVal;
}



bool SimpleGL::UnloadTexture( DeviceTextureID textureID )
{
	GLuint const texID = static_cast<GLuint>( textureID );
	glDeleteTextures( 1, &texID );
	CONSUME_ERRORS();
	return true;
}



DeviceFontID SimpleGL::CreateBitmapFont( rftl::byte_view const& buffer, uint32_t& characterWidth, uint32_t& characterHeight, rftl::array<uint32_t, 256>* variableWidth, rftl::array<uint32_t, 256>* variableHeight )
{
	int tx, ty, tn;
	size_t const kRGBAElements = 3;
	unsigned char* data = stbi_load_from_memory( reinterpret_cast<stbi_uc const*>( buffer.data() ), math::integer_cast<int>( buffer.size() ), &tx, &ty, &tn, kRGBAElements );
	RF_ASSERT( data != nullptr );
	if( data == nullptr )
	{
		return false;
	}
	RF_ASSERT( tx > 0 );
	RF_ASSERT( ty > 0 );
	RF_ASSERT( tn == kRGBAElements );
	size_t x = static_cast<size_t>( tx );
	size_t y = static_cast<size_t>( ty );
	size_t n = static_cast<size_t>( tn );

	// Check font type
	{
		enum class FontType
		{
			Invalid = -1,
			Square,
			Narrow
		};
		FontType fontType = FontType::Invalid;
		if( x == y )
		{
			// 2x2
			fontType = FontType::Square;
		}
		else if( x * 2 == y )
		{
			// 2x1
			fontType = FontType::Narrow;
		}
		else
		{
			// Unsupported ratio
			stbi_image_free( data );
			return false;
		}
		( (void)fontType );
	}

	// Expect 16x16 characters
	constexpr size_t kCharactersPerRow = 16;
	constexpr size_t kCharactersPerColumn = 16;
	bool const divisibleAlongWidth = ( x % kCharactersPerRow ) == 0;
	bool const divisibleAlongHeight = ( y % kCharactersPerColumn ) == 0;
	if( divisibleAlongWidth == false || divisibleAlongHeight == false )
	{
		// Not divisible
		stbi_image_free( data );
		return false;
	}

	uint32_t const charWidth = math::integer_cast<uint32_t>( x / kCharactersPerRow );
	characterWidth = charWidth;
	uint32_t const charHeight = math::integer_cast<uint32_t>( y / kCharactersPerColumn );
	characterHeight = charHeight;
	if( variableWidth != nullptr )
	{
		for( uint32_t& width : *variableWidth )
		{
			width = 0;
		}
	}
	if( variableHeight != nullptr )
	{
		for( uint32_t& height : *variableHeight )
		{
			height = 0;
		}
	}

	using CharacterStorage = rftl::vector<uint8_t>;
	using CharacterListStorage = rftl::vector<CharacterStorage>;
	CharacterListStorage listStorage;
	listStorage.clear();
	listStorage.resize( 256, CharacterStorage{} );
	uint8_t const* readHead = data;
	uint8_t const* const maxReadHead = &data[x * y * n];
	for( size_t row = 0; row < kCharactersPerColumn; row++ )
	{
		for( uint32_t scanline = 0; scanline < charHeight; scanline++ )
		{
			for( size_t column = 0; column < kCharactersPerRow; column++ )
			{
				size_t const characterIndex = row * kCharactersPerRow + column;
				CharacterStorage& characterStorage = listStorage.at( characterIndex );

				uint32_t unusedVariableWidth = 0;
				uint32_t* variableCharWidthPtr = &unusedVariableWidth;
				uint32_t* variableCharHeightPtr = &unusedVariableWidth;
				if( variableWidth != nullptr )
				{
					variableCharWidthPtr = &variableWidth->at( characterIndex );
				}
				if( variableHeight != nullptr )
				{
					variableCharHeightPtr = &variableHeight->at( characterIndex );
				}
				uint32_t& variableCharWidth = *variableCharWidthPtr;
				uint32_t& variableCharHeight = *variableCharHeightPtr;

				for( uint32_t pixel = 0; pixel < charWidth; pixel++ )
				{
					static_assert( kRGBAElements == 3, "Unexpected pixel size" );
					RF_ASSERT( readHead < maxReadHead );
					(void)maxReadHead;
					//uint8_t const redElement = readHead[0]; // Unused
					uint8_t const greenElement = readHead[1];
					//uint8_t const blueElement = readHead[2]; // Unused
					if( greenElement > 128 )
					{
						variableCharWidth = math::Max( variableCharWidth, pixel + 1 );
						variableCharHeight = math::Max( variableCharHeight, scanline + 1 );
					}
					characterStorage.emplace_back( greenElement );
					readHead += n;
				}
			}
		}
	}
	stbi_image_free( data );

	mLastCreatedFontId++;
	DeviceFontID const retVal = mLastCreatedFontId;
	RF_ASSERT( mBitmapFonts.count( mLastCreatedFontId ) == 0 );
	BitmapCharacterListStorage& fontStorage = mBitmapFonts[mLastCreatedFontId];
	{
		rftl::array<GLuint, 256> tempStore;
		glGenTextures( math::integer_cast<GLsizei>( fontStorage.size() ), tempStore.data() );
		CONSUME_ERRORS();
		for( size_t i = 0; i < tempStore.size(); i++ )
		{
			fontStorage[i] = tempStore[i];
		}
	}
	RF_ASSERT( fontStorage.size() == listStorage.size() );
	for( size_t i = 0; i < fontStorage.size(); i++ )
	{
		uint8_t const* const listBuffer = listStorage[i].data();
		GLuint const texID = math::integer_cast<GLuint>( fontStorage[i] );
		glBindTexture( GL_TEXTURE_2D, texID );
		CONSUME_ERRORS();
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_ALPHA,
			math::integer_cast<GLsizei>( charWidth ),
			math::integer_cast<GLsizei>( charHeight ),
			0,
			GL_ALPHA,
			GL_UNSIGNED_BYTE,
			listBuffer );
		CONSUME_ERRORS();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		CONSUME_ERRORS();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		CONSUME_ERRORS();
	}

	return retVal;
}



bool SimpleGL::DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color )
{
	return DrawBitmapFont( fontID, character, pos, z, color, math::AABB4f{ 0.f, 0.f, 1.f, 1.f } );
}



bool SimpleGL::DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color, math::AABB4f texUV )
{
	DeviceTextureID const texID = mBitmapFonts.at( fontID ).at( static_cast<unsigned char>( character ) );
	glColor3f( color.r, color.g, color.b );
	CANT_ERROR();
	return DrawBillboardInternal( texID, pos, z, texUV );
}



bool SimpleGL::DebugRenderText( math::Vector2f pos, float z, const char* fmt, ... )
{
	glColor3f( 0, 0, 0 );
	CANT_ERROR();
	glRasterPos3f( pos.x, pos.y, z );
	CANT_ERROR();
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawTextInternal( fmt, args );
	va_end( args );
	return retVal;
}



bool SimpleGL::DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float z, float width, math::Color3f color )
{
	glBindTexture( GL_TEXTURE_2D, 0 );
	CONSUME_ERRORS();
	glColor3f( color.r, color.g, color.b );
	CANT_ERROR();
	if( width < 1 )
	{
		glLineWidth( 1.1f ); // HACK: Make diagonals fatter, but not horizontal or vertical
		CONSUME_ERRORS();
		glBegin( GL_LINES );
		API_ERROR_BUG();
		glVertex3f( p0.x + mXFudge, p0.y + mYFudge, z );
		CANT_ERROR();
		glVertex3f( p1.x + mXFudge, p1.y + mYFudge, z );
		CANT_ERROR();
		glEnd();
		CONSUME_ERRORS();
	}
	else
	{
		glLineWidth( width );
		CONSUME_ERRORS();
		glBegin( GL_LINES );
		API_ERROR_BUG();
		glVertex3f( p0.x - mXFudge * width, p0.y - mYFudge * width, z );
		CANT_ERROR();
		glVertex3f( p1.x - mXFudge * width, p1.y - mYFudge * width, z );
		CANT_ERROR();
		glEnd();
		CONSUME_ERRORS();
	}
	return true;
}



bool SimpleGL::DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z )
{
	glColor3f( 1, 1, 1 );
	CANT_ERROR();
	DrawBillboardInternal( textureID, pos, z, math::AABB4f{ 0.f, 0.f, 1.f, 1.f } );
	return true;
}



bool SimpleGL::DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z, math::AABB4f texUV )
{
	glColor3f( 1, 1, 1 );
	CANT_ERROR();
	DrawBillboardInternal( textureID, pos, z, texUV );
	return true;
}



bool SimpleGL::BeginFrame()
{
	shim::wglMakeCurrent( mHDC, mHRC );
	// Clear the buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	CONSUME_ERRORS();
	glPushMatrix();
	CONSUME_ERRORS();
	return true;
}



bool SimpleGL::RenderFrame()
{
	glColor3f( 1, 1, 1 );
	CANT_ERROR();
#ifdef SIMPLEGL_DBG_GRID
	{
		for( float horizontal = 0; horizontal <= 1; horizontal += 0.1f )
		{
			glBegin( GL_LINES );
			API_ERROR_BUG();
			glVertex2d( horizontal, 0 );
			CANT_ERROR();
			glVertex2d( horizontal, 1 );
			CANT_ERROR();
			glEnd();
			CONSUME_ERRORS();
		}
		for( float vertical = 0; vertical <= 1; vertical += 0.1f )
		{
			glBegin( GL_LINES );
			API_ERROR_BUG();
			glVertex2d( 0, vertical );
			CANT_ERROR();
			glVertex2d( 1, vertical );
			CANT_ERROR();
			glEnd();
			CONSUME_ERRORS();
		}
	}
#endif
	// Makes sure that all buffered commands are sent to the graphics card.
	//  Note that this does NOT insure that the commands were executed, only
	//  that they have been sent.
	glFlush();
	CONSUME_ERRORS();
	return true;
}



bool SimpleGL::EndFrame()
{
	glPopMatrix();
	CONSUME_ERRORS();
	shim::SwapBuffers( mHDC );
	return true;
}

///////////////////////////////////////////////////////////////////////////////

void SimpleGL::BuildFont( int8_t height )
{
	// Windows font ID
	shim::HFONT font{};

	// Used for good house keeping
	shim::HFONT oldfont{};

	glDeleteLists( font_base, 96 );
	CONSUME_ERRORS();
	// Storage For 96 Characters
	font_base = glGenLists( 96 );
	CONSUME_ERRORS();
	font = shim::CreateFontW(
		-height /*-12*/, // Height of font
		0, // Width of font
		0, // Angle of escapement
		0, // Orientation angle
		shim::kFW_BOLD, // Font weight
		false, // Italic
		false, // Underline
		false, // Strikeout
		shim::kANSI_CHARSET, // Character set identifier
		shim::kOUT_TT_PRECIS, // Output precision
		shim::kCLIP_DEFAULT_PRECIS, // Clipping precision
		shim::kANTIALIASED_QUALITY, // Output quality
		math::integer_cast<shim::DWORD>(
			math::enum_bitcast( shim::kFF_DONTCARE ) |
			math::enum_bitcast( shim::kDEFAULT_PITCH ) ), // Family and pitch
		L"Arial" ); // Font name
	// Selects the font we want
	oldfont = static_cast<shim::HFONT>( shim::SelectObject( mHDC, font ) );
	// Builds 96 characters starting at character 32
	shim::wglUseFontBitmapsW( mHDC, 32, 96, font_base );
	// Builds 96 characters starting at character 32
	//wglUseFontOutlines(mHDC, 32, 96, base, 0, 1,WGL_FONT_LINES, 0);
	// Selects the font we want
	shim::SelectObject( mHDC, oldfont );
	// Delete the font
	shim::DeleteObject( font );
}



bool SimpleGL::DrawTextInternal( char const* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawTextInternal( fmt, args );
	va_end( args );
	return retVal;
}



bool SimpleGL::DrawTextInternal( char const* fmt, va_list args )
{
	if( fmt == nullptr )
	{
		return false;
	}

	char text[256];
	vsprintf_s( text, fmt, args );
	text[255] = '\0';

	// Pushes the display list bits
	glPushAttrib( GL_LIST_BIT );
	CONSUME_ERRORS();

	// Sets the base character to 32
	glListBase( font_base - 32 );
	CONSUME_ERRORS();

	// Draws the display list text
	glCallLists( static_cast<GLsizei>( strlen( text ) ), GL_UNSIGNED_BYTE, text );
	CANT_ERROR();

	// Pops the display list bits
	glPopAttrib();
	CONSUME_ERRORS();

	return true;
}



bool SimpleGL::DrawBillboardInternal( DeviceTextureID textureID, math::AABB4f pos, float z, math::AABB4f texUV )
{
	GLuint const texID = static_cast<GLuint>( textureID );
	glBindTexture( GL_TEXTURE_2D, texID );
	CONSUME_ERRORS();
	float const left = pos.mTopLeft.x;
	float const top = pos.mTopLeft.y;
	float const right = pos.mBottomRight.x;
	float const bottom = pos.mBottomRight.y;
	float const u0 = texUV.mTopLeft.x;
	float const v0 = texUV.mTopLeft.y;
	float const u1 = texUV.mBottomRight.x;
	float const v1 = texUV.mBottomRight.y;
	glBegin( GL_QUADS );
	API_ERROR_BUG();
	{
		glTexCoord2f( u0, v0 );
		CANT_ERROR();
		glVertex3f( left, top, z );
		CANT_ERROR();
		glTexCoord2f( u1, v0 );
		CANT_ERROR();
		glVertex3f( right, top, z );
		CANT_ERROR();
		glTexCoord2f( u1, v1 );
		CANT_ERROR();
		glVertex3f( right, bottom, z );
		CANT_ERROR();
		glTexCoord2f( u0, v1 );
		CANT_ERROR();
		glVertex3f( left, bottom, z );
		CANT_ERROR();
	}
	glEnd();
	CONSUME_ERRORS();
	glBindTexture( GL_TEXTURE_2D, 0 );
	CONSUME_ERRORS();
	return true;
}

///////////////////////////////////////////////////////////////////////////////

#undef CONSUME_ERRORS
#undef CANT_ERROR
#undef API_ERROR_BUG

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "core_platform/windows_inc.h"
#include "gdi_shim.h"

int shim::ChoosePixelFormat( HDC hdc, PIXELFORMATDESCRIPTOR const* ppfd )
{
	return win32::ChoosePixelFormat( static_cast<win32::HDC>( hdc ), reinterpret_cast<win32::PIXELFORMATDESCRIPTOR const*>( ppfd ) );
}

bool shim::SetPixelFormat( HDC hdc, int format, PIXELFORMATDESCRIPTOR const * ppfd )
{
	return win32::SetPixelFormat( static_cast<win32::HDC>( hdc ), format, reinterpret_cast<win32::PIXELFORMATDESCRIPTOR const*>( ppfd ) ) != 0;
}

shim::HGLRC shim::wglCreateContext( HDC hdc )
{
	return win32::wglCreateContext( static_cast<win32::HDC>( hdc ) );
}

bool shim::wglDeleteContext( HGLRC h )
{
	return win32::wglDeleteContext( static_cast<win32::HGLRC>( h ) ) != 0;
}

bool shim::wglMakeCurrent( HDC hdc, HGLRC hglrc )
{
	return win32::wglMakeCurrent( static_cast<win32::HDC>( hdc ), static_cast<win32::HGLRC>( hglrc ) ) != 0;
}

bool shim::SwapBuffers( HDC h )
{
	return win32::SwapBuffers( static_cast<win32::HDC>( h ) ) != 0;
}

shim::HGDIOBJ shim::SelectObject( HDC hdc, HGDIOBJ h )
{
	return win32::SelectObject( static_cast<win32::HDC>( hdc ), static_cast<win32::HGDIOBJ>( h ) );
}

bool shim::DeleteObject( HGDIOBJ ho )
{
	return win32::DeleteObject( static_cast<win32::HGDIOBJ>( ho ) ) != 0;
}

bool shim::wglUseFontBitmapsW( HDC a, DWORD b, DWORD c, DWORD d )
{
	return win32::wglUseFontBitmapsW(
		static_cast<win32::HDC>( a ),
		static_cast<win32::DWORD>( b ),
		static_cast<win32::DWORD>( c ),
		static_cast<win32::DWORD>( d ) ) != 0;
}

shim::HFONT shim::CreateFontW( int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName )
{
	return win32::CreateFontW(
		cHeight,
		cWidth,
		cEscapement,
		cOrientation,
		cWeight,
		static_cast<win32::DWORD>( bItalic ),
		static_cast<win32::DWORD>( bUnderline ),
		static_cast<win32::DWORD>( bStrikeOut ),
		static_cast<win32::DWORD>( iCharSet ),
		static_cast<win32::DWORD>( iOutPrecision ),
		static_cast<win32::DWORD>( iClipPrecision ),
		static_cast<win32::DWORD>( iQuality ),
		static_cast<win32::DWORD>( iPitchAndFamily ),
		static_cast<win32::LPCWSTR>( pszFaceName ) );
}

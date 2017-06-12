#include "stdafx.h"
#include "core_platform/windows_inc.h"
#include "gdi_shim.h"

int shim::ChoosePixelFormat( HDC hdc, PIXELFORMATDESCRIPTOR* ppfd )
{
	return win32::ChoosePixelFormat( ( win32::HDC )hdc, ( win32::PIXELFORMATDESCRIPTOR* )ppfd );
}

bool shim::SetPixelFormat( HDC hdc, int format, PIXELFORMATDESCRIPTOR const * ppfd )
{
	return win32::SetPixelFormat( ( win32::HDC ) hdc, format, ( win32::PIXELFORMATDESCRIPTOR const * ) ppfd ) != 0;
}

shim::HGLRC shim::wglCreateContext( HDC hdc )
{
	return win32::wglCreateContext( ( win32::HDC ) hdc );
}

bool shim::wglDeleteContext( HGLRC h )
{
	return win32::wglDeleteContext( ( win32::HGLRC )h ) != 0;
}

bool shim::wglMakeCurrent( HDC hdc, HGLRC hglrc )
{
	return win32::wglMakeCurrent( ( win32::HDC )hdc, ( win32::HGLRC )hglrc ) != 0;
}

bool shim::SwapBuffers( HDC h )
{
	return win32::SwapBuffers( ( win32::HDC )h ) != 0;
}

shim::HGDIOBJ shim::SelectObject( HDC hdc, HGDIOBJ h )
{
	return win32::SelectObject( ( win32::HDC ) hdc, ( win32::HGDIOBJ ) h );
}

bool shim::DeleteObject( HGDIOBJ ho )
{
	return win32::DeleteObject( ( win32::HGDIOBJ )ho ) != 0;
}

bool shim::wglUseFontBitmapsW( HDC a, DWORD b, DWORD c, DWORD d )
{
	return win32::wglUseFontBitmapsW( ( win32::HDC )a, ( win32::DWORD )b, ( win32::DWORD )c, ( win32::DWORD )d ) != 0;
}

shim::HFONT shim::CreateFontW( int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName )
{
	return win32::CreateFontW( cHeight, cWidth, cEscapement, cOrientation, cWeight, ( win32::DWORD )bItalic, ( win32::DWORD )bUnderline, ( win32::DWORD )bStrikeOut, ( win32::DWORD )iCharSet, ( win32::DWORD )iOutPrecision, ( win32::DWORD )iClipPrecision, ( win32::DWORD )iQuality, ( win32::DWORD )iPitchAndFamily, ( win32::LPCWSTR )pszFaceName );
}

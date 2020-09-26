#include "stdafx.h"
#include "core_platform/inc/windows_inc.h"
#include "win_shim.h"

shim::HDC shim::GetDC( HWND hWnd )
{
	return win32::GetDC( static_cast<win32::HWND>( hWnd ) );
}

int shim::ReleaseDC( HWND hWnd, HDC hDC )
{
	return win32::ReleaseDC(
		static_cast<win32::HWND>( hWnd ),
		static_cast<win32::HDC>( hDC ) );
}

shim::LRESULT WIN32_CALLBACK shim::DefWindowProcW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return win32::DefWindowProcW(
		static_cast<win32::HWND>( hWnd ),
		static_cast<win32::UINT>( Msg ),
		static_cast<win32::WPARAM>( wParam ),
		static_cast<win32::LPARAM>( lParam ) );
}

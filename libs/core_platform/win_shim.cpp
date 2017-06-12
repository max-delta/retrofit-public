#include "stdafx.h"
#include "core_platform/windows_inc.h"
#include "win_shim.h"

shim::HDC shim::GetDC( HWND hWnd )
{
	return win32::GetDC( ( win32::HWND )hWnd );
}

int shim::ReleaseDC( HWND hWnd, HDC hDC )
{
	return win32::ReleaseDC( ( win32::HWND ) hWnd, ( win32::HDC ) hDC );
}

shim::LRESULT WIN32_CALLBACK shim::DefWindowProcW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return win32::DefWindowProcW( ( win32::HWND ) hWnd, ( win32::UINT ) Msg, ( win32::WPARAM ) wParam, ( win32::LPARAM ) lParam );
}

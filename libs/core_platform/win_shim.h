#pragma once

namespace shim
{
//WinDef
typedef void* HWND;
typedef void* HDC;
typedef void* HGLRC;
typedef void* HFONT;
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
typedef wchar_t WCHAR;
typedef const WCHAR *LPCWSTR;
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

//WinUser
#define WIN32_CALLBACK __stdcall
typedef LRESULT (WIN32_CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
HDC GetDC( HWND hWnd );
int ReleaseDC( HWND hWnd, HDC hDC );
LRESULT __stdcall DefWindowProcW(
	HWND hWnd,
	UINT Msg,
	WPARAM wParam,
	LPARAM lParam);
}


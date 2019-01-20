#pragma once

#include "core/compiler.h"

#include "rftl/cstdint"

namespace shim {
//WinDef
typedef void* HWND;
typedef void* HDC;
typedef void* HGLRC;
typedef void* HFONT;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned int* PUINT;
typedef wchar_t WCHAR;
typedef const WCHAR* LPCWSTR;
#ifdef RF_PLATFORM_MSVC
	#if defined( _WIN64 )
		typedef __int64 INT_PTR, *PINT_PTR;
		typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
		typedef __int64 LONG_PTR, *PLONG_PTR;
		typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
	#else
		typedef int INT_PTR, *PINT_PTR;
		typedef unsigned int UINT_PTR, *PUINT_PTR;
		typedef long LONG_PTR, *PLONG_PTR;
		typedef unsigned long ULONG_PTR, *PULONG_PTR;
	#endif
#else
	#if RF_PLATFORM_POINTER_BYTES == 8
		typedef int64_t INT_PTR, *PINT_PTR;
		typedef uint64_t UINT_PTR, *PUINT_PTR;
		typedef int64_t LONG_PTR, *PLONG_PTR;
		typedef uint64_t ULONG_PTR, *PULONG_PTR;
	#elif RF_PLATFORM_POINTER_BYTES == 4
		typedef int32_t INT_PTR, *PINT_PTR;
		typedef uint32_t UINT_PTR, *PUINT_PTR;
		typedef int32_t LONG_PTR, *PLONG_PTR;
		typedef uint32_t ULONG_PTR, *PULONG_PTR;
	#else
		#error Unsupported platform
	#endif
#endif
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

//WinUser
#define WIN32_CALLBACK __stdcall
typedef LRESULT( WIN32_CALLBACK* WNDPROC )( HWND, UINT, WPARAM, LPARAM );
HDC GetDC( HWND hWnd );
int ReleaseDC( HWND hWnd, HDC hDC );
LRESULT __stdcall DefWindowProcW(
	HWND hWnd,
	UINT Msg,
	WPARAM wParam,
	LPARAM lParam );
}

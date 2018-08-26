#pragma once
#include "win_shim.h"

namespace shim
{
//WinGDI
typedef void* HGDIOBJ;
typedef struct tagPIXELFORMATDESCRIPTOR
{
	WORD  nSize;
	WORD  nVersion;
	DWORD dwFlags;
	BYTE  iPixelType;
	BYTE  cColorBits;
	BYTE  cRedBits;
	BYTE  cRedShift;
	BYTE  cGreenBits;
	BYTE  cGreenShift;
	BYTE  cBlueBits;
	BYTE  cBlueShift;
	BYTE  cAlphaBits;
	BYTE  cAlphaShift;
	BYTE  cAccumBits;
	BYTE  cAccumRedBits;
	BYTE  cAccumGreenBits;
	BYTE  cAccumBlueBits;
	BYTE  cAccumAlphaBits;
	BYTE  cDepthBits;
	BYTE  cStencilBits;
	BYTE  cAuxBuffers;
	BYTE  iLayerType;
	BYTE  bReserved;
	DWORD dwLayerMask;
	DWORD dwVisibleMask;
	DWORD dwDamageMask;
} PIXELFORMATDESCRIPTOR;
enum pixel_types
{
	kPFD_TYPE_RGBA = 0,
	kPFD_TYPE_COLORINDEX = 1,
};
enum layer_types
{
	kPFD_MAIN_PLANE = 0,
	kPFD_OVERLAY_PLANE = 1,
	kPFD_UNDERLAY_PLANE = ( -1 ),
};
enum Font_Weights
{
	kFW_DONTCARE = 0,
	kFW_THIN = 100,
	kFW_EXTRALIGHT = 200,
	kFW_LIGHT = 300,
	kFW_NORMAL = 400,
	kFW_MEDIUM = 500,
	kFW_SEMIBOLD = 600,
	kFW_BOLD = 700,
	kFW_EXTRABOLD = 800,
	kFW_HEAVY = 900,
};
enum
{
	kANSI_CHARSET = 0,
	kDEFAULT_CHARSET = 1,
	kSYMBOL_CHARSET = 2,
	kSHIFTJIS_CHARSET = 128,
	kHANGEUL_CHARSET = 129,
	kHANGUL_CHARSET = 129,
	kGB2312_CHARSET = 134,
	kCHINESEBIG5_CHARSET = 136,
	kOEM_CHARSET = 255,
};
enum
{
	kOUT_DEFAULT_PRECIS = 0,
	kOUT_STRING_PRECIS = 1,
	kOUT_CHARACTER_PRECIS = 2,
	kOUT_STROKE_PRECIS = 3,
	kOUT_TT_PRECIS = 4,
	kOUT_DEVICE_PRECIS = 5,
	kOUT_RASTER_PRECIS = 6,
	kOUT_TT_ONLY_PRECIS = 7,
	kOUT_OUTLINE_PRECIS = 8,
	kOUT_SCREEN_OUTLINE_PRECIS = 9,
	kOUT_PS_ONLY_PRECIS = 10,
};
enum
{
	kCLIP_DEFAULT_PRECIS = 0,
	kCLIP_CHARACTER_PRECIS = 1,
	kCLIP_STROKE_PRECIS = 2,
	kCLIP_MASK = 0xf,
	kCLIP_LH_ANGLES = ( 1 << 4 ),
	kCLIP_TT_ALWAYS = ( 2 << 4 ),
};
enum
{
	kDEFAULT_QUALITY = 0,
	kDRAFT_QUALITY = 1,
	kPROOF_QUALITY = 2,
	kNONANTIALIASED_QUALITY = 3,
	kANTIALIASED_QUALITY = 4,
};
enum Font_Families
{
	kFF_DONTCARE = ( 0 << 4 ),  /* Don't care or don't know. */
	kFF_ROMAN = ( 1 << 4 ),  /* Variable stroke mWidth, serifed. */
	kFF_SWISS = ( 2 << 4 ),  /* Variable stroke mWidth, sans-serifed. */
	kFF_MODERN = ( 3 << 4 ),  /* Constant stroke mWidth, serifed or sans-serifed. */
	kFF_SCRIPT = ( 4 << 4 ),  /* Cursive, etc. */
	kFF_DECORATIVE = ( 5 << 4 ),  /* Old English, etc. */
};
enum
{
	kDEFAULT_PITCH = 0,
	kFIXED_PITCH = 1,
	kVARIABLE_PITCH = 2,
	kMONO_FONT = 8,
};
enum PIXELFORMATDESCRIPTOR_flags
{
	kPFD_DOUBLEBUFFER = 0x00000001,
	kPFD_STEREO = 0x00000002,
	kPFD_DRAW_TO_WINDOW = 0x00000004,
	kPFD_DRAW_TO_BITMAP = 0x00000008,
	kPFD_SUPPORT_GDI = 0x00000010,
	kPFD_SUPPORT_OPENGL = 0x00000020,
	kPFD_GENERIC_FORMAT = 0x00000040,
	kPFD_NEED_PALETTE = 0x00000080,
	kPFD_NEED_SYSTEM_PALETTE = 0x00000100,
	kPFD_SWAP_EXCHANGE = 0x00000200,
	kPFD_SWAP_COPY = 0x00000400,
	kPFD_SWAP_LAYER_BUFFERS = 0x00000800,
	kPFD_GENERIC_ACCELERATED = 0x00001000,
	kPFD_SUPPORT_DIRECTDRAW = 0x00002000,
	kPFD_DIRECT3D_ACCELERATED = 0x00004000,
	kPFD_SUPPORT_COMPOSITION = 0x00008000,
};
int ChoosePixelFormat( HDC hdc, PIXELFORMATDESCRIPTOR const* ppfd );
bool SetPixelFormat( HDC hdc, int format, PIXELFORMATDESCRIPTOR const* ppfd );
HGLRC wglCreateContext( HDC );
bool wglDeleteContext( HGLRC );
bool wglMakeCurrent( HDC, HGLRC );
bool SwapBuffers( HDC );
HGDIOBJ SelectObject( HDC hdc, HGDIOBJ h );
bool DeleteObject( HGDIOBJ ho );
bool wglUseFontBitmapsW( HDC, DWORD, DWORD, DWORD );
HFONT CreateFontW( int cHeight, int cWidth, int cEscapement, int cOrientation, int cWeight, DWORD bItalic,
	DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision,
	DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName );

}

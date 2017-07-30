#pragma once

#ifdef PPU_EXPORTS
#define PPU_API __declspec(dllexport)
#else
#define PPU_API __declspec(dllimport)
#endif

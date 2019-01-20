#pragma once

#ifdef SIMPLEGL_EXPORTS
#define SIMPLEGL_API __declspec( dllexport )
#else
#define SIMPLEGL_API __declspec( dllimport )
#endif

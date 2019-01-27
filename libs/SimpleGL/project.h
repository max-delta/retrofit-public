#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef SIMPLEGL_EXPORTS
#define SIMPLEGL_API __declspec( dllexport )
#else
#define SIMPLEGL_API __declspec( dllimport )
#endif
#else
#define SIMPLEGL_API
#endif

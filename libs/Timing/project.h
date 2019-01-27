#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef TIMING_EXPORTS
#define TIMING_API __declspec( dllexport )
#else
#define TIMING_API __declspec( dllimport )
#endif
#else
#define TIMING_API
#endif

#pragma once

#ifdef TIMING_EXPORTS
#define TIMING_API __declspec(dllexport)
#else
#define TIMING_API __declspec(dllimport)
#endif

#pragma once

#ifdef PLATFORMUTILS_EXPORTS
	#define PLATFORMUTILS_API __declspec(dllexport)
#else
	#define PLATFORMUTILS_API __declspec(dllimport)
#endif

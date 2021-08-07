#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef ALLOCATION_EXPORTS
#define ALLOCATION_API __declspec( dllexport )
#else
#define ALLOCATION_API __declspec( dllimport )
#endif
#else
#define ALLOCATION_API
#endif

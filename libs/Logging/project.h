#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef LOGGING_EXPORTS
#define LOGGING_API __declspec( dllexport )
#else
#define LOGGING_API __declspec( dllimport )
#endif
#else
#define LOGGING_API
#endif

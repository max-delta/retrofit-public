#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef RFTYPE_EXPORTS
#define RFTYPE_API __declspec( dllexport )
#else
#define RFTYPE_API __declspec( dllimport )
#endif
#else
#define RFTYPE_API
#endif

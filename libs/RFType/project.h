#pragma once

#ifdef RFTYPE_EXPORTS
#define RFTYPE_API __declspec( dllexport )
#else
#define RFTYPE_API __declspec( dllimport )
#endif

#pragma once

#ifdef PLATFORMINPUT_EXPORTS
#define PLATFORMINPUT_API __declspec( dllexport )
#else
#define PLATFORMINPUT_API __declspec( dllimport )
#endif

#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef PLATFORMINPUT_EXPORTS
#define PLATFORMINPUT_API __declspec( dllexport )
#else
#define PLATFORMINPUT_API __declspec( dllimport )
#endif
#else
#define PLATFORMINPUT_API
#endif

#pragma once

#ifdef SCRIPTINGSQUIRREL_EXPORTS
#define SCRIPTINGSQUIRREL_API __declspec(dllexport)
#else
#define SCRIPTINGSQUIRREL_API __declspec(dllimport)
#endif

#pragma once

#ifdef SCHEDULING_EXPORTS
#define SCHEDULING_API __declspec(dllexport)
#else
#define SCHEDULING_API __declspec(dllimport)
#endif

#pragma once

#ifdef PLATFORMFILESYSTEM_EXPORTS
#define PLATFORMFILESYSTEM_API __declspec(dllexport)
#else
#define PLATFORMFILESYSTEM_API __declspec(dllimport)
#endif

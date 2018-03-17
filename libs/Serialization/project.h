#pragma once

#ifdef SERIALIZATION_EXPORTS
#define SERIALIZATION_API __declspec(dllexport)
#else
#define SERIALIZATION_API __declspec(dllimport)
#endif

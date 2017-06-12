#pragma once

#define WINGDIAPI __declspec(dllimport)
#define APIENTRY __stdcall
#include <gl/gl.h>
#undef WINGDIAPI
#undef APIENTRY

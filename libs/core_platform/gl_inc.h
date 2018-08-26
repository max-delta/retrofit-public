#pragma once

#define WINGDIAPI __declspec(dllimport)
#define APIENTRY __stdcall
#include <gl/GL.h>
#undef WINGDIAPI
#undef APIENTRY

#pragma once

#include "core/compiler.h"

#ifdef RF_PLATFORM_MSVC
	#pragma warning(push)
	#pragma warning(disable:4826)
	#pragma warning(disable:4619)
#endif
#include <gtest/gtest.h>
#ifdef RF_PLATFORM_MSVC
	#pragma warning(pop)
#endif

// For warnings caused by usage of GTest macros
#ifdef RF_PLATFORM_CLANG
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

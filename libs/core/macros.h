#pragma once
#include <assert.h>
#include <crtdbg.h>


#define RF_NO_COPY(CLASS) \
	CLASS(CLASS const &) = delete; \
	CLASS & operator =(CLASS const &) = delete;


#define RF_ASSERT(TEST) \
	do \
	{ \
		if( !!!(TEST) && _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "", "FAIL" ) == 1 ) \
		{ \
			__debugbreak(); \
		} \
	} while (false)

#pragma once
#include <assert.h>

// Cuz fuck Windows.h, that's why
void __cdecl __debugbreak( void );
#ifdef _DEBUG
__pragma(pack(push, _CRT_PACKING))
extern "C" {
	__declspec(dllimport) int __cdecl _CrtDbgReport(
		int         _ReportType,
		char const* _FileName,
		int         _Linenumber,
		char const* _ModuleName,
		char const* _Format,
		...);
}
__pragma(pack(pop))
#define _CRT_WARN           0
#define _CRT_ERROR          1
#define _CRT_ASSERT         2
#define _CRT_ERRCNT         3
#endif

#define RF_CONCAT_INNER(LHS, RHS) LHS ## RHS
#define RF_CONCAT(LHS, RHS) RF_CONCAT_INNER(LHS, RHS)
#define RF_STRINGIFY(X) #X

#define RF_NO_COPY(CLASS) \
	CLASS(CLASS const &) = delete; \
	CLASS & operator =(CLASS const &) = delete;
#define RF_NO_MOVE(CLASS) \
	CLASS( CLASS && ) = delete; \
	CLASS& operator =( CLASS && ) = delete;


#ifdef _DEBUG
#define RF_ASSERT(TEST) \
	do \
	{ \
		if( !!!(TEST) && _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "", "FAIL" ) == 1 ) \
		{ \
			__debugbreak(); \
		} \
	} while (false)
#define RF_ASSERT_MSG(TEST, MSG) \
	do \
	{ \
		if( !!!(TEST) && _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, "", MSG ) == 1 ) \
		{ \
			__debugbreak(); \
		} \
	} while (false)
#else
#define RF_ASSERT(TEST)
#define RF_ASSERT_MSG(TEST, MSG)
#endif

#include "stdafx.h"
#include "rf_assert.h"

#include "core/compiler.h"


// Keeping Windows.h quaratined
#if defined( _DEBUG ) && defined( RF_PLATFORM_MSVC )
__pragma( pack( push, _CRT_PACKING ) ) extern "C"
{
	__declspec( dllimport ) int __cdecl _CrtDbgReport(
		int _ReportType,
		char const* _FileName,
		int _Linenumber,
		char const* _ModuleName,
		char const* _Format,
		... );
}
__pragma( pack( pop, _CRT_PACKING ) )
#define _CRT_WARN 0
#define _CRT_ERROR 1
#define _CRT_ASSERT 2
#define _CRT_ERRCNT 3
#endif

namespace RF { namespace assert {
///////////////////////////////////////////////////////////////////////////////

AssertResponse AssertNotification( char const* file, size_t line, char const* failure, char const* message )
{
	#if defined( _DEBUG ) && defined( RF_PLATFORM_MSVC )
	{
		if(
			_CrtDbgReport(
				_CRT_ASSERT,
				file,
				static_cast<int>( line ),
				nullptr,
				"Assertion failed!\n"
				"\n"
				"Message:\n"
				"%s\n"
				"\n"
				"Test:\n"
				"%s\n"
				"\n"
				"[Abort: Crash]\n"
				"[Retry: Debug]\n"
				"[Ignore: Skip all asserts of this type]",
				message,
				failure ) == 1 )
		{
			return AssertResponse::Interrupt;
		}

		return AssertResponse::SkipFurtherInstances;
	}
	#else
	{
		return AssertResponse::Interrupt;
	}
	#endif
}

///////////////////////////////////////////////////////////////////////////////
}}

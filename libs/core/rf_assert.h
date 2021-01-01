#pragma once
#include "core/compiler.h"
#include "core/macros.h"


namespace RF::assert {
///////////////////////////////////////////////////////////////////////////////

enum class AssertResponse
{
	SkipFurtherInstances = 0,
	Interrupt,
};

AssertResponse AssertNotification( char const* file, size_t line, char const* failure, char const* message );

[[noreturn]] void FatalNotification( char const* file, size_t line, char const* trigger, char const* message );

// HACK: The MSVC intrinsic __fastfail is not recognized as non-returning by
//  static code analysis, so it needs to be wrapped for asserts to properly
//  guard their respective code
[[noreturn]] void FatalInterrupt();

///////////////////////////////////////////////////////////////////////////////
}

// Retail builds lack normal assert machinery, and use a fatal path instead
#define RF_RETAIL_FATAL_MSG( TRIGGER, MSG ) \
	::RF::assert::FatalNotification( RF_FILENAME(), static_cast<size_t>( __LINE__ ), TRIGGER, MSG )

// HACK: Intellisense craters in VS 16.4.2 on normal assert macros, despite
//  cpp.hint help, so is always disabled
#if defined( __INTELLISENSE__ )
	#define RF_ASSERT( TEST ) RF_EMPTY_FUNCLET()
	#define RF_ASSERT_MSG( TEST, MSG ) RF_EMPTY_FUNCLET()
	#define RF_DBGFAIL() RF_EMPTY_FUNCLET()
	#define RF_DBGFAIL_MSG( MSG ) RF_EMPTY_FUNCLET()
// Static analysis builds need to assume that asserts can't fail, so they are
//  replaced with non-returning failure paths that can't be skipped
#elif defined( _PREFAST_ )
	#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
		#define ___RF_ASSERT_IMPL___( TEST, MESSAGEEXPRESSION ) \
			do \
			{ \
				if( !( TEST ) ) \
				{ \
					RF_RETAIL_FATAL_MSG( "Analysis assert", #TEST ); \
				} \
			} while( false )
	#else
		#define ___RF_ASSERT_IMPL___( TEST, MESSAGEEXPRESSION ) \
			do \
			{ \
				if( !( TEST ) ) \
				{ \
					::RF::assert::FatalInterrupt(); \
				} \
			} while( false )
	#endif
	#define RF_ASSERT( TEST ) ___RF_ASSERT_IMPL___( TEST, "FAIL" )
	#define RF_ASSERT_MSG( TEST, MSG ) ___RF_ASSERT_IMPL___( TEST, MSG )
	#define RF_DBGFAIL() ___RF_ASSERT_IMPL___( false, "FAIL" )
	#define RF_DBGFAIL_MSG( MSG ) ___RF_ASSERT_IMPL___( false, MSG )
// Normal asserts can be skipped while development is still ongoing
#elif RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	#define ___RF_ASSERT_IMPL___( TEST, MESSAGEEXPRESSION ) \
		do \
		{ \
			static bool ___rf_assert_skip_all_instances = false; \
			if( \
				___rf_assert_skip_all_instances == true || \
				!!( TEST ) ) \
			{ \
				/* NOTE: Can't use '&&', or MSVC asks you if you meant '&' */ \
			} \
			else \
			{ \
				if( \
					::RF::assert::AssertNotification( RF_FILENAME(), static_cast<size_t>( __LINE__ ), #TEST, MESSAGEEXPRESSION ) == ::RF::assert::AssertResponse::Interrupt ) \
				{ \
					RF_SOFTWARE_INTERRUPT(); \
				} \
				else \
				{ \
					___rf_assert_skip_all_instances = true; \
				} \
			} \
		} while( false )
	#define RF_ASSERT( TEST ) ___RF_ASSERT_IMPL___( TEST, "FAIL" )
	#define RF_ASSERT_MSG( TEST, MSG ) ___RF_ASSERT_IMPL___( TEST, MSG )
	#define RF_DBGFAIL() ___RF_ASSERT_IMPL___( false, "FAIL" )
	#define RF_DBGFAIL_MSG( MSG ) ___RF_ASSERT_IMPL___( false, MSG )
// In all other cases, asserts are inert
#else
	#define RF_ASSERT( TEST ) RF_EMPTY_FUNCLET()
	#define RF_ASSERT_MSG( TEST, MSG ) RF_EMPTY_FUNCLET()
	#define RF_DBGFAIL() RF_EMPTY_FUNCLET()
	#define RF_DBGFAIL_MSG( MSG ) RF_EMPTY_FUNCLET()
#endif

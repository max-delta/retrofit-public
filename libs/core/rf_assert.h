#pragma once
#include "core/compiler.h"
#include "core/macros.h"


namespace RF { namespace assert {
///////////////////////////////////////////////////////////////////////////////

enum class AssertResponse
{
	SkipFurtherInstances = 0,
	Interrupt,
};

AssertResponse AssertNotification( char const* file, size_t line, char const* failure, char const* message );

///////////////////////////////////////////////////////////////////////////////
}}

#ifdef _DEBUG
#define ___RF_ASSERT_IMPL___(TEST, MESSAGEEXPRESSION) \
	do \
	{ \
		static bool ___rf_assert_skip_all_instances = false; \
		if( \
			___rf_assert_skip_all_instances == true || \
			!!(TEST) ) \
		{ \
			/* NOTE: Can't use '&&', or MSVC asks you if you meant '&' */ \
		} \
		else \
		{ \
			if( \
				::RF::assert::AssertNotification(__FILE__, static_cast<size_t>(__LINE__), #TEST, MESSAGEEXPRESSION) \
					== ::RF::assert::AssertResponse::Interrupt ) \
			{ \
				RF_SOFTWARE_INTERRUPT(); \
			} \
			else \
			{ \
				___rf_assert_skip_all_instances = true; \
			} \
		} \
	} while (false)
#define RF_ASSERT(TEST) ___RF_ASSERT_IMPL___(TEST, "FAIL")
#define RF_ASSERT_MSG(TEST, MSG) ___RF_ASSERT_IMPL___(TEST, MSG)
#define RF_DBGFAIL() ___RF_ASSERT_IMPL___(false, "FAIL")
#define RF_DBGFAIL_MSG(MSG) ___RF_ASSERT_IMPL___(false, MSG)
#else
#define RF_ASSERT(TEST) RF_EMPTY_FUNCLET()
#define RF_ASSERT_MSG(TEST, MSG) RF_EMPTY_FUNCLET()
#define RF_DBGFAIL() RF_EMPTY_FUNCLET()
#define RF_DBGFAIL_MSG(MSG) RF_EMPTY_FUNCLET()
#endif

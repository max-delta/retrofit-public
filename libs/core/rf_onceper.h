#pragma once
#include "core/macros.h"

#include "rftl/chrono"


// IMPORTANT: ONCEPER is intended for rate-limiting of diagnostic code, and is
//  not intended to be used for actual timing logic. As such, it will likely be
//  compiled out of non-diagnostic builds.
#if RF_IS_ALLOWED( RF_CONFIG_ONCEPER )
	#define ___RF_ONCEPER_IMPL___( RATE, ACTION ) \
		do \
		{ \
			::rftl::chrono::system_clock::duration const ___rf_rate = RATE; \
			static ::rftl::chrono::system_clock::time_point ___rf_lastTime{ ::rftl::chrono::system_clock::duration( 0 ) }; \
			::rftl::chrono::system_clock::time_point const ___rf_now = ::rftl::chrono::system_clock::now(); \
			::rftl::chrono::system_clock::duration const ___rf_timePassed = ___rf_now - ___rf_lastTime; \
			if( ___rf_timePassed > ___rf_rate ) \
			{ \
				___rf_lastTime = ___rf_now; \
				do \
				{ \
					ACTION; \
				} while( false ); \
			} \
		} while( false )
#else
	#define ___RF_ONCEPER_IMPL___( RATE, ACTION ) \
		RF_EMPTY_FUNCLET()
#endif

#define RF_ONCEPER_SECOND( ACTION ) \
	___RF_ONCEPER_IMPL___(::rftl::chrono::seconds( 1 ), ACTION )
#define RF_ONCEPER_MINUTE( ACTION ) \
	___RF_ONCEPER_IMPL___(::rftl::chrono::minutes( 1 ), ACTION )

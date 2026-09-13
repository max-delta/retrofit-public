#pragma once

#include "core/macros.h"
#include "core/rf_assert.h"

// To-do macros, as annotations and as run-time failures
#if RF_IS_ALLOWED( RF_CONFIG_TODO_MESSAGES )
	#define RF_TODO_ANNOTATION( MESSAGE ) \
		RF_BUILD_MESSAGE( RF_CREATE_SOURCED_BUILD_MESSAGE( "TODO: " MESSAGE "" ) )
#else
	#define RF_TODO_ANNOTATION( MESSAGE )
#endif
#define RF_TODO_BREAK_MSG( MESSAGE ) \
	do \
	{ \
		RF_TODO_ANNOTATION( MESSAGE ); \
		RF_DBGFAIL_MSG( "TODO: " MESSAGE "" ); \
	} while( false )
#define RF_TODO_BREAK() \
	RF_TODO_BREAK_MSG( "Implement" )

// To-do macros about language features
#if RF_IS_ALLOWED( RF_CONFIG_LANG_TODO_CHECKS )
	#define RF_CPP20_TODO( MESSAGE ) \
		static_assert( __cplusplus < 202002L, "TODO: " MESSAGE "" )
	#define RF_CPP23_TODO( MESSAGE ) \
		static_assert( __cplusplus < 202302L, "TODO: " MESSAGE "" )
#else
	#define RF_CPP20_TODO( MESSAGE ) RF_TODO_ANNOTATION( MESSAGE )
	#define RF_CPP23_TODO( MESSAGE ) RF_TODO_ANNOTATION( MESSAGE )
#endif

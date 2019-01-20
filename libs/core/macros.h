#pragma once

// For compiler and platform macros
#include "core/compiler.h"

// For assert macros
#include "core/rf_assert.h"

// Performs macro expansions that would not normally be performed when using
//  just the normal preprocessor syntax
#define RF_CONCAT_INNER( LHS, RHS ) LHS##RHS
#define RF_CONCAT( LHS, RHS ) RF_CONCAT_INNER( LHS, RHS )
#define RF_STRINGIFY_INNER( X ) #X
#define RF_STRINGIFY( X ) RF_STRINGIFY_INNER( X )

// In-class decorators to automate common declaration patterns
#define RF_NO_COPY( CLASS ) \
	CLASS( CLASS const& ) = delete; \
	CLASS& operator=( CLASS const& ) = delete;
#define RF_NO_MOVE( CLASS ) \
	CLASS( CLASS&& ) = delete; \
	CLASS& operator=( CLASS&& ) = delete;

// For macro expansion where 'if(...) MACRO(); Code();' would result in an
//  undesired conditional around the non-macro code
#define RF_EMPTY_FUNCLET() do{}while(false)

// Creates a formatted message for a build log that points to the source
// NOTE: The formatting specifically matches the format that Visual Studio can
//  match against to provide double-click and F8 functionality for
#define RF_CREATE_SOURCED_BUILD_MESSAGE( MESSAGE ) \
	"" __FILE__ "(" RF_STRINGIFY( __LINE__ ) "): " MESSAGE ""

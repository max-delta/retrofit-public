#pragma once

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
#define RF_DEFAULT_COPY( CLASS ) \
public: \
	CLASS( CLASS const& ) = default; \
	CLASS& operator=( CLASS const& ) = default;
#define RF_NO_MOVE( CLASS ) \
	CLASS( CLASS&& ) = delete; \
	CLASS& operator=( CLASS&& ) = delete;
#define RF_NO_ASSIGN( CLASS ) \
	CLASS& operator=( CLASS const& ) = delete; \
	CLASS& operator=( CLASS&& ) = delete;
#define RF_NO_INSTANCE( CLASS ) \
	RF_NO_COPY( CLASS ); \
	RF_NO_MOVE( CLASS ); \
	CLASS() = delete; \
	~CLASS() = delete; \
	template<typename __RF_ANYTHING__> \
	CLASS( __RF_ANYTHING__ const& ) = delete; \
	template<typename __RF_ANYTHING__> \
	CLASS( __RF_ANYTHING__&& ) = delete; \
	template<typename __RF_ANYTHING__> \
	operator __RF_ANYTHING__() const = delete;

// Construct(&&) and Operator=(&&) decorators to automate common move patterns
#define RF_MOVE_CONSTRUCT( MEMBER ) \
	MEMBER( rftl::move( rhs.MEMBER ) )
#define RF_MOVE_CLEAR( MEMBER ) \
	rhs.MEMBER = {};
#define RF_MOVE_ASSIGN( MEMBER ) \
	this->MEMBER = rftl::move( rhs.MEMBER );
#define RF_MOVE_ASSIGN_CLEAR( MEMBER ) \
	this->MEMBER = rftl::move( rhs.MEMBER ); \
	RF_MOVE_CLEAR( MEMBER );

// For macro expansion where 'if(...) MACRO(); Code();' would result in an
//  undesired conditional around the non-macro code
#define RF_EMPTY_FUNCLET() \
	do \
	{ \
	} while( false )

#if RF_IS_ALLOWED( RF_CONFIG_FILENAME_MACRO )
	#define RF_FILENAME() __FILE__
#else
	#define RF_FILENAME() "__FILE__"
#endif

// Creates a formatted message for a build log that points to the source
// NOTE: The formatting specifically matches the format that Visual Studio can
//  match against to provide double-click and F8 functionality for
#define RF_CREATE_SOURCED_BUILD_MESSAGE( MESSAGE ) \
	"" RF_FILENAME() "(" RF_STRINGIFY( __LINE__ ) "): " MESSAGE ""

// For compiler and platform macros
#include "core/compiler.h"

// For assert macros
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

#pragma once

// For bare-minimum macros to make preprocessor behave properly
#include "core/preprocessor.h"

// In-class decorators to automate common declaration patterns
#define RF_NO_COPY( CLASS ) \
	CLASS( CLASS const& ) = delete; \
	CLASS& operator=( CLASS const& ) = delete;
#define RF_DEFAULT_COPY( CLASS ) \
public: \
	CLASS( CLASS const& ) = default; \
	CLASS& operator=( CLASS const& ) = default;
#define RF_PRIVATE_COPY( CLASS ) \
private: \
	CLASS( CLASS const& ) = default; \
	CLASS& operator=( CLASS const& ) = default;
#define RF_NO_MOVE( CLASS ) \
	CLASS( CLASS&& ) = delete; \
	CLASS& operator=( CLASS&& ) = delete;
#define RF_PRIVATE_MOVE( CLASS ) \
private: \
	CLASS( CLASS&& ) = default; \
	CLASS& operator=( CLASS&& ) = default;
#define RF_DEFAULT_MOVE( CLASS ) \
public: \
	CLASS( CLASS&& ) = default; \
	CLASS& operator=( CLASS&& ) = default;
#define RF_DEFAULT_EQUALS( CLASS ) \
public: \
	bool operator==( CLASS const& ) const = default;
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

// For TODO macros
#include "core/todo.h"

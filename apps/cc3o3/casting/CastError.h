#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

class CastError
{
	RF_NO_COPY( CastError );

	//
	// Types and constants
public:
	// Helper to clarify intentional nulls (indicating lack of an error)
	static constexpr nullptr_t kNoError = nullptr;


	//
	// Public methods
public:
	CastError(
		act::Environment const& env,
		act::Context const& ctx );
	~CastError();

	// Helper for error creation
	static UniquePtr<CastError> Create(
		act::Environment const& env,
		act::Context const& ctx );


	//
	// Public data
public:
	// The environment at the time of error
	UniquePtr<act::Environment const> mEnv = nullptr;

	// The context at the time of error
	UniquePtr<act::Context const> mCtx = nullptr;

	// If set, an action could not be found with this key
	rftl::string mMissingActionKey;
};

///////////////////////////////////////////////////////////////////////////////
}

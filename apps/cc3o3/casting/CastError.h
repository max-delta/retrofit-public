#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/optional"
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

	// Creates a deep clone
	UniquePtr<CastError> Clone() const;


	//
	// Public data
public:
	// The environment at the time of error
	UniquePtr<act::Environment const> mEnv = nullptr;

	// The context at the time of error
	UniquePtr<act::Context const> mCtx = nullptr;

	RF_TODO_ANNOTATION(
		"Consider moving all supplementary fields to new derived classes, make"
		" the cast error class abstract, and have an abstract virtual function"
		" that specifies what the localization key(s) for the error is(are),"
		" so it can be surfaced up to an end-user, in case it's their fault"
		" for trying to do something invalid ($err_wrong_color+$innate_red)" );

	// If set, an element could not resolve to a desc
	rftl::optional<element::ElementIdentifier> mFailedElementDescLookup = rftl::nullopt;

	// If set, a cast level was too high or low, likely constrained by the desc
	rftl::optional<element::ElementLevel> mCastLevelOutOfBounds = rftl::nullopt;

	// If set, an action could not be found with this key
	rftl::string mMissingActionKey;

	// If set, a cast was determined to be ineligible before actions started
	bool mPreActionCastInvalidatation = false;

	// If set, a cast was unable to locate the combat context
	bool mMissingCombatContext = false;
};

///////////////////////////////////////////////////////////////////////////////
}

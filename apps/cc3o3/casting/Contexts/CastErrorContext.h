#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/Cast.h"

#include "GameAction/Context.h"

#include "core/ptr/unique_ptr.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

// Context for storing a cast error
class CastErrorContext : public act::Context
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( CastErrorContext );

	//
	// Public methods
public:
	explicit CastErrorContext( UniquePtr<CastError>&& castError );
	explicit CastErrorContext( CastError const& castError );

	virtual UniquePtr<Context> Clone() const override;


	//
	// Public data
public:
	UniquePtr<CastError> mCastError;
};

///////////////////////////////////////////////////////////////////////////////
}

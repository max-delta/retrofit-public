#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"

#include "core/macros.h"


namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

struct ROLLBACK_API InputStreamRef
{
	RF_NO_ASSIGN( InputStreamRef );

	InputStreamRef(
		InputStreamIdentifier identifier,
		InputStream& uncommitted,
		InputStream const& committed );
	InputStreamRef( InputStreamRef const& ) = default;

	InputStreamIdentifier const mIdentifier;
	InputStream& mUncommitted;
	InputStream const& mCommitted;
};

///////////////////////////////////////////////////////////////////////////////

struct ROLLBACK_API InputStreamConstRef
{
	RF_NO_ASSIGN( InputStreamConstRef );

	InputStreamConstRef(
		InputStreamIdentifier identifier,
		InputStream const& uncommitted,
		InputStream const& committed );
	InputStreamConstRef( InputStreamConstRef const& ) = default;

	InputStreamIdentifier const mIdentifier;
	InputStream const& mUncommitted;
	InputStream const& mCommitted;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "InputStreamRef.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

InputStreamRef::InputStreamRef(
	InputStreamIdentifier identifier,
	InputStream& uncommitted,
	InputStream const& committed )
	: mIdentifier( identifier )
	, mUncommitted( uncommitted )
	, mCommitted( committed )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

InputStreamConstRef::InputStreamConstRef(
	InputStreamIdentifier identifier,
	InputStream const& uncommitted,
	InputStream const& committed )
	: mIdentifier( identifier )
	, mUncommitted( uncommitted )
	, mCommitted( committed )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}

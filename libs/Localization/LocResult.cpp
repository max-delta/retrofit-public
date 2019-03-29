#include "stdafx.h"
#include "LocResult.h"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

LocResult::LocResult( rftl::u32string const& codePoints )
	: mCodePoints( codePoints )
{
	//
}



LocResult::LocResult( rftl::u32string&& codePoints )
	: mCodePoints( rftl::move( codePoints ) )
{
	//
}



rftl::u32string const& LocResult::GetCodePoints() const
{
	return mCodePoints;
}

///////////////////////////////////////////////////////////////////////////////
}}

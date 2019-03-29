#include "stdafx.h"
#include "LocEngine.h"

#include "Localization/LocQuery.h"
#include "Localization/LocResult.h"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

TextDirection LocEngine::GetTextDirection() const
{
	// HACK: Hard-coded
	// TODO: Implement
	return TextDirection::LeftToRight;
}



LocResult LocEngine::Query( LocQuery const& query ) const
{
	// HACK: Return key
	// TODO: Only do this if localizing is disabled
	// TODO: Implement
	rftl::string id = query.mKey.GetAsString();
	rftl::u32string codePoints( id.begin(), id.end() );
	return LocResult( rftl::move( codePoints ) );
}

///////////////////////////////////////////////////////////////////////////////
}}

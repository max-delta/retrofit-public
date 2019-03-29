#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "rftl/string"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocResult
{
	//
	// Public methods
public:
	explicit LocResult( rftl::u32string const& codePoints );
	explicit LocResult( rftl::u32string&& codePoints );

	rftl::u32string const& GetCodePoints() const;


	//
	// Private data
private:
	rftl::u32string mCodePoints;
};

///////////////////////////////////////////////////////////////////////////////
}}

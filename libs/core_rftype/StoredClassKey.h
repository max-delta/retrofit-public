#pragma once
#include "core_rftype/RFTypeFwd.h"

#include "core_math/Hash.h"

#include "rftl/optional"
#include "rftl/string"
#include "rftl/string_view"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

// When class info is stored in a type database, the conventional way is to
//  store it by a hash of its name
// NOTE: The hash is intended to be unique, so that storage of the name may be
//  optional, and potentially discarded once a hash is made from it
struct StoredClassKey
{
	StoredClassKey();
	StoredClassKey( rftl::string&& name, math::HashVal64 hash );
	StoredClassKey( rftl::string_view name, math::HashVal64 hash );

	bool IsValid() const;
	math::HashVal64 GetHash() const;

	rftl::string mName;
	rftl::optional<math::HashVal64> mHash = rftl::nullopt;
};

///////////////////////////////////////////////////////////////////////////////
}

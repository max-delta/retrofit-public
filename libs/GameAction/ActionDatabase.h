#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/string_view"
#include "rftl/unordered_map"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Holds reusable actions that can be referenced by ID, assumed to be the
//  primary way most games get access to actions, which are presumed to be
//  loaded from disk in data-driven setups
class GAMEACTION_API ActionDatabase final
{
	RF_NO_COPY( ActionDatabase );

	//
	// Types and constants
private:
	using ActionsByID = rftl::unordered_map<rftl::string, UniquePtr<ActionRecord>>;


	//
	// Public methods
public:
	ActionDatabase();

	bool AddAction( rftl::string&& key, UniquePtr<ActionRecord>&& action );
	UniquePtr<ActionRecord> RemoveAction( rftl::string_view const& key );
	WeakPtr<ActionRecord> GetAction( rftl::string_view const& key ) const;


	//
	// Private data
private:
	ActionsByID mActionsByID = {};
};

///////////////////////////////////////////////////////////////////////////////
}

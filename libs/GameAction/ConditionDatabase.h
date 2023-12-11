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
class GAMEACTION_API ConditionDatabase final
{
	RF_NO_COPY( ConditionDatabase );

	//
	// Types and constants
private:
	using ConditionsByID = rftl::unordered_map<rftl::string, UniquePtr<ConditionRecord>>;


	//
	// Public methods
public:
	ConditionDatabase();

	bool AddCondition( rftl::string&& key, UniquePtr<ConditionRecord>&& condition );
	UniquePtr<ConditionRecord> RemoveCondition( rftl::string_view const& key );
	WeakPtr<ConditionRecord> GetCondition( rftl::string_view const& key ) const;


	//
	// Private data
private:
	ConditionsByID mConditionsByID = {};
};

///////////////////////////////////////////////////////////////////////////////
}

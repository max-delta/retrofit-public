#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/CombatantID.h"
#include "cc3o3/elements/ElementFwd.h"

#include "GameAction/ActionFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/extension/static_vector.h"
#include "rftl/optional"
#include "rftl/string"
#include "rftl/string_view"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

class CastingEngine
{
	RF_NO_COPY( CastingEngine );

	//
	// Types and constants
public:
	using FighterIDs = rftl::static_vector<combat::FighterID, combat::kMaxTotalFighters>;


	//
	// Public methods
public:
	CastingEngine(
		WeakPtr<file::VFS const> vfs,
		WeakPtr<combat::CombatEngine const> const& combatEngine,
		WeakPtr<element::ElementDatabase const> const& elementDatabase );
	~CastingEngine();

	bool LoadActionDefinitions( file::VFSPath const& actionDefinitionsDir );

	// Execute a cast, modifying the provided instance
	// NOTE: On success, the return value is empty
	// NOTE: On error, the provided instance will not be modified
	// NOTE: Source and/or target may be ommitted by setting them to explicitly
	//  invalid values, though this may not be supported by the attempted cast
	// NOTE: There may be non-element actions, such as helper actions used by
	//  multiple elements, and those could be keyed in a way that can't be
	//  represented by an element identifier, but still accessible via raw key
	// NOTE: Casting via raw key could fail due to trying to cast a helper that
	//  is missing structural prep that would've been provided by its invoker
	UniquePtr<CastError> ExecuteElementCast(
		combat::CombatInstance& combatInstance,
		combat::FighterID const& source,
		combat::FighterID const& target,
		element::ElementIdentifier identifier,
		element::ElementLevel castedLevel ) const;
	UniquePtr<CastError> ExecuteRawCast(
		combat::CombatInstance& combatInstance,
		combat::FighterID const& source,
		combat::FighterID const& target,
		rftl::string_view const& key,
		rftl::optional<element::ElementDesc> elementDesc,
		element::ElementLevel castedLevel ) const;

	// What combatants a caster could choose to target with an element
	FighterIDs GetValidCastTargets(
		combat::CombatInstance const& combatInstance,
		combat::FighterID sourceID,
		element::ElementIdentifier elementIdentifier ) const;


	//
	// Private methods
private:
	bool LoadActionDefinition(
		file::VFSPath const& actionDefinitionPath,
		rftl::string&& key );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	WeakPtr<combat::CombatEngine const> mCombatEngine;
	WeakPtr<element::ElementDatabase const> mElementDatabase;
	UniquePtr<act::ActionDatabase> const mActionDatabase;
	UniquePtr<act::ConditionDatabase> const mConditionDatabase;
};

///////////////////////////////////////////////////////////////////////////////
}

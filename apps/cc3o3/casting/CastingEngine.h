#pragma once
#include "project.h"

#include "cc3o3/casting/CastingFwd.h"
#include "cc3o3/combat/CombatFwd.h"
#include "cc3o3/elements/ElementFwd.h"

#include "GameAction/ActionDatabase.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

class CastingEngine
{
	RF_NO_COPY( CastingEngine );

	//
	// Public methods
public:
	CastingEngine(
		WeakPtr<file::VFS const> vfs,
		WeakPtr<combat::CombatEngine const> const& combatEngine,
		WeakPtr<element::ElementDatabase const> const& elementDatabase );

	bool LoadActionDefinitions( file::VFSPath const& actionDefinitionsDir );

	WeakPtr<act::ActionRecord const> GetElementActionDefinifion(
		element::ElementIdentifier identifier ) const;

	// There may be non-element actions, such as helper actions used by
	//  multiple elements, and those could be keyed in a way that can't be
	//  represented by an element identifier
	WeakPtr<act::ActionRecord const> GetRawActionDefinifion(
		rftl::string_view const& key ) const;


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
	act::ActionDatabase mActionDatabase;
};

///////////////////////////////////////////////////////////////////////////////
}

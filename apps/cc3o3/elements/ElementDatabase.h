#pragma once
#include "ElementFwd.h"

#include "cc3o3/elements/ElementDesc.h"
#include "cc3o3/company/CompanyFwd.h"

#include "GameAction/ActionDatabase.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/vector"
#include "rftl/deque"
#include "rftl/array"
#include "rftl/string"
#include "rftl/unordered_map"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

class ElementDatabase
{
	RF_NO_COPY( ElementDatabase );

	//
	// Types and constants
public:
	using ElementDescs = rftl::vector<ElementDesc>;
	using ElementIdentifiers = rftl::vector<ElementIdentifier>;
	using ElementCounts = rftl::unordered_map<ElementIdentifier, size_t>;
	using TierUnlocks = rftl::array<size_t, company::kMaxStoryTier>;

private:
	using TierUnlockLookup = rftl::unordered_map<ElementIdentifier, TierUnlocks>;


	//
	// Public methods
public:
	ElementDatabase( WeakPtr<file::VFS const> vfs );

	bool LoadDescTables( file::VFSPath const& descTablesDir );
	bool LoadTierUnlockTables( file::VFSPath const& tierUnlockTablesDir );
	bool LoadActionDefinitions( file::VFSPath const& actionDefinitionsDir );

	ElementIdentifiers GetAllElementIdentifiers() const;
	ElementDesc GetElementDesc( ElementIdentifier identifier ) const;
	ElementCounts GetElementsBasedOnTier( company::StoryTier tier ) const;
	WeakPtr<act::ActionRecord const> GetElementActionDefinifion( ElementIdentifier identifier ) const;

	// There may be non-element actions, such as helper actions used by
	//  multiple elements, and those could be keyed in a way that can't be
	//  represented by an element identifier
	WeakPtr<act::ActionRecord const> GetRawActionDefinifion( rftl::string_view const& key ) const;


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );
	bool LoadDescTable( file::VFSPath const& descTablePath );
	bool LoadTierUnlockTable( file::VFSPath const& tierUnlockTablePath );
	bool LoadActionDefinition( file::VFSPath const& actionDefinitionPath, rftl::string&& key );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	ElementDescs mElementDescs;
	TierUnlockLookup mTierUnlocks;
	act::ActionDatabase mActionDatabase;
};

///////////////////////////////////////////////////////////////////////////////
}

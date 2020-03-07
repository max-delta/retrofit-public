#pragma once
#include "ElementFwd.h"

#include "cc3o3/elements/ElementDesc.h"
#include "cc3o3/company/CompanyFwd.h"

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
	using TierUnlocks = rftl::array<size_t, company::kMaxStoryTier>;

private:
	using TierUnlockLookup = rftl::unordered_map<ElementIdentifier, TierUnlocks>;


	//
	// Public methods
public:
	ElementDatabase( WeakPtr<file::VFS const> vfs );

	bool LoadDescTables( file::VFSPath const& descTablesDir );
	bool LoadTierUnlockTables( file::VFSPath const& tierUnlockTablesDir );

	ElementDesc GetElementDesc( ElementIdentifier identifier ) const;


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );
	bool LoadDescTable( file::VFSPath const& descTablePath );
	bool LoadTierUnlockTable( file::VFSPath const& tierUnlockTablePath );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	ElementDescs mElementDescs;
	TierUnlockLookup mTierUnlocks;
};

///////////////////////////////////////////////////////////////////////////////
}

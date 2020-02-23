#pragma once
#include "ElementFwd.h"

#include "cc3o3/elements/ElementDesc.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/vector"
#include "rftl/deque"
#include "rftl/string"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

class ElementDatabase
{
	RF_NO_COPY( ElementDatabase );

	//
	// Types and constants
public:
	using ElementDescs = rftl::vector<ElementDesc>;


	//
	// Public methods
public:
	ElementDatabase( WeakPtr<file::VFS const> vfs );

	bool LoadDescTables( file::VFSPath const& descTablesDir );

	ElementDesc GetElementDesc( ElementIdentifier identifier ) const;


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );
	bool LoadDescTable( file::VFSPath const& descTablePath );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	ElementDescs mElementDescs;
};

///////////////////////////////////////////////////////////////////////////////
}

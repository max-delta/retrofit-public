#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/string"
#include "rftl/unordered_map"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API PageMapper
{
	//
	// Types
private:
	using Charmap = rftl::unordered_map<char32_t, char>;


	//
	// Public methods
public:
	bool InitializeFromCharmapFile( file::VFS const& vfs, file::VFSPath const& path );

	rftl::string MapTo8Bit( rftl::u32string const& codePoints ) const;


	//
	// Private data
private:
	Charmap mCharmap;
};

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/string"
#include "rftl/unordered_map"


namespace RF { namespace loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocEngine
{
	//
	// Types
private:
	using Keymap = rftl::unordered_map<rftl::string, rftl::u32string>;


	//
	// Public methods
public:
	bool InitializeFromKeymapFile( file::VFS const& vfs, file::VFSPath const& path, TextDirection textDirection );

	void SetKeyDebug( bool value );

	TextDirection GetTextDirection() const;
	LocResult Query( LocQuery const& query ) const;


	//
	// Private data
private:
	TextDirection mTextDirection = TextDirection::kInvalid;
	bool mKeyDebug = false;
	Keymap mKeymap;
};

///////////////////////////////////////////////////////////////////////////////
}}

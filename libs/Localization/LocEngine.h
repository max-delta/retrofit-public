#pragma once
#include "project.h"

#include "Localization/LocFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/extension/string_hash.h"
#include "rftl/string"
#include "rftl/string_view"
#include "rftl/unordered_map"


namespace RF::loc {
///////////////////////////////////////////////////////////////////////////////

class LOCALIZATION_API LocEngine
{
	//
	// Types
private:
	using Keymap =
		rftl::unordered_map<
			rftl::string,
			rftl::u32string,
			rftl::string_hash,
			rftl::equal_to<>>;


	//
	// Public methods
public:
	bool InitializeFromKeymapDirectory( file::VFS const& vfs, file::VFSPath const& path, TextDirection textDirection );

	// When true, results are replaced with debug information about the query
	void SetKeyDebug( bool value );

	TextDirection GetTextDirection() const;
	LocResult Query( LocQuery const& query ) const;


	//
	// Private methods
private:
	static Keymap LoadKeymapFromFile( file::VFS const& vfs, file::VFSPath const& path );

	// Expand all nested keys, return the number of replacements
	// WARNING: It is trivially easy to create an infinitely recursive key, so
	//  a limit on the number of expansions is enforced, and optionally this is
	//  treated as an error
	static size_t ExpandNestedKeys( Keymap& keymap, size_t maxDepthPerKey, bool errorOnMaxDepth );

	static rftl::u32string ReplaceOneToken( Keymap const& keyMap, rftl::string_view key, rftl::u32string str );


	//
	// Private data
private:
	TextDirection mTextDirection = TextDirection::kInvalid;
	bool mKeyDebug = false;
	Keymap mKeymap;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"

#include "GameSprite/SpriteFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/unordered_map"
#include "rftl/deque"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct GeneticsEntry
{
	rftl::string mSpecies = {};
	rftl::string mGender = {};
	bool mPlayable = false;
	rftl::string m9Char = {};
};

///////////////////////////////////////////////////////////////////////////////

class CharacterValidator
{
	RF_NO_COPY( CharacterValidator );

	//
	// Types
public:
	using GeneticsID = rftl::immutable_string;
private:
	using GeneticsStorage = rftl::unordered_map<GeneticsID, GeneticsEntry>;


	//
	// Public methods
public:
	CharacterValidator( WeakPtr<file::VFS const> vfs, WeakPtr<sprite::CharacterCreator const> characterCreator );

	bool LoadGeneticsTable( file::VFSPath const& geneticsTablePath );

	void SanitizeForCharacterCreation( Character& character ) const;
	void SanitizeForGameplay( Character& character ) const;


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;

	// TODO: Only using this for composition logic, it should be seperated and
	//  then re-used by the various systems that need it
	WeakPtr<sprite::CharacterCreator const> mCharacterCreator;

	GeneticsStorage mGeneticsStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}}

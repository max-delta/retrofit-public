#pragma once
#include "project.h"

#include "cc3o3/char/Character.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/unordered_map"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

class CharacterDatabase
{
	RF_NO_COPY( CharacterDatabase );

	//
	// Types
public:
	using CharacterID = rftl::immutable_string;
private:
	using CharacterStorage = rftl::unordered_map<CharacterID, Character>;


	//
	// Public methods
public:
	CharacterDatabase() = default;

	bool SubmitNewCharacter( CharacterID const& id, Character&& character );
	Character FetchExistingCharacter( CharacterID const& id ) const;
	void SubmitOrOverwriteCharacter( CharacterID const& id, Character&& character );
	bool OverwriteExistingCharacter( CharacterID const& id, Character&& character );
	bool DeleteExistingCharacter( CharacterID const& id );
	size_t DeleteAllCharacters();

	bool LoadFromPersistentStorage( file::VFSPath const& directory );
	bool SaveToPersistentStorage( file::VFSPath const& directory );


	//
	// Private data
private:
	CharacterStorage mStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}}

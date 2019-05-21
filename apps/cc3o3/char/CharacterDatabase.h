#pragma once
#include "project.h"

#include "cc3o3/char/Character.h"

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
	bool OverwriteExistingCharacter( CharacterID const& id, Character&& character );
	bool DeleteExistingCharacter( CharacterID const& id );

	bool LoadFromPersistentStorage();
	bool SaveToPersistentStorage();


	//
	// Private data
private:
	CharacterStorage mStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}}

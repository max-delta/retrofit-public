#pragma once
#include "project.h"

#include "cc3o3/char/Character.h"

#include "GameSprite/CompositeCharacter.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/unordered_map"
#include "rftl/vector"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

class CharacterDatabase
{
	RF_NO_COPY( CharacterDatabase );

	//
	// Types
public:
	using CharacterID = rftl::immutable_string;
	using CharacterIDs = rftl::vector<CharacterID>;

private:
	using CharacterStorage = rftl::unordered_map<CharacterID, Character>;
	using CompositeStorage = rftl::unordered_map<CharacterID, sprite::CompositeCharacter>;


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

	sprite::CompositeCharacter FetchExistingComposite( CharacterID const& id ) const;
	void SubmitOrOverwriteComposite( CharacterID const& id, sprite::CompositeCharacter&& composite );

	CharacterIDs GetAllCharacterIDs() const;

	size_t LoadFromPersistentStorage( file::VFSPath const& directory );
	size_t SaveToPersistentStorage( file::VFSPath const& directory );


	//
	// Private data
private:
	CharacterStorage mCharacterStorage;
	CompositeStorage mCompositeStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}}

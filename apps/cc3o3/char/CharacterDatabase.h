#pragma once
#include "project.h"

#include "cc3o3/char/CharData.h"

#include "GameSprite/CompositeCharacter.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"

#include "rftl/extension/immutable_string.h"
#include "rftl/unordered_map"
#include "rftl/vector"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

class CharacterDatabase
{
	RF_NO_COPY( CharacterDatabase );

	//
	// Types
public:
	using CharacterID = rftl::immutable_string;
	using CharacterIDs = rftl::vector<CharacterID>;
	using CharacterMode = rftl::immutable_string;

private:
	using CharacterStorage = rftl::unordered_map<CharacterID, CharData>;
	using CompositeModeStorage = rftl::unordered_map<CharacterMode, sprite::CompositeCharacter>;
	using CompositeStorage = rftl::unordered_map<CharacterID, CompositeModeStorage>;


	//
	// Public methods
public:
	CharacterDatabase() = default;

	bool SubmitNewCharacter( CharacterID const& id, CharData&& character );
	CharData FetchExistingCharacter( CharacterID const& id ) const;
	void SubmitOrOverwriteCharacter( CharacterID const& id, CharData&& character );
	bool OverwriteExistingCharacter( CharacterID const& id, CharData&& character );
	bool DeleteExistingCharacter( CharacterID const& id );
	size_t DeleteAllCharacters();

	sprite::CompositeCharacter FetchExistingComposite( CharacterID const& id, CharacterMode const& mode ) const;
	void SubmitOrOverwriteComposite( CharacterID const& id, CharacterMode const& mode, sprite::CompositeCharacter&& composite );

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
}

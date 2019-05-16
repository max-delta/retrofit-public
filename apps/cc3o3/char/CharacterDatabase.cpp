#include "stdafx.h"
#include "CharacterDatabase.h"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

bool character::CharacterDatabase::SubmitNewCharacter( CharacterID const& id, Character&& character )
{
	if( mStorage.count( id ) != 0 )
	{
		return false;
	}

	mStorage[id] = rftl::move( character );
	return true;
}



Character CharacterDatabase::FetchExistingCharacter( CharacterID const& id ) const
{
	CharacterStorage::const_iterator const iter = mStorage.find( id );
	if( iter == mStorage.end() )
	{
		return Character();
	}
	return iter->second;
}



bool CharacterDatabase::OverwriteExistingCharacter( CharacterID const& id, Character&& character )
{
	CharacterStorage::iterator const iter = mStorage.find( id );
	if( iter == mStorage.end() )
	{
		return false;
	}
	iter->second = rftl::move( character );
	return true;
}



bool CharacterDatabase::DeleteExistingCharacter( CharacterID const& id )
{
	CharacterStorage::const_iterator const iter = mStorage.find( id );
	if( iter == mStorage.end() )
	{
		return false;
	}
	mStorage.erase( iter );
	return true;
}



bool CharacterDatabase::LoadFromPersistentStorage()
{
	// TODO
	return true;
}



bool CharacterDatabase::SaveToPersistentStorage()
{
	// TODO
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}}

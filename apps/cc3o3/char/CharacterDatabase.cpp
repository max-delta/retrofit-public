#include "stdafx.h"
#include "CharacterDatabase.h"

#include "cc3o3/resource/ResourceLoad.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PlatformFilesystem/VFS.h"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

bool character::CharacterDatabase::SubmitNewCharacter( CharacterID const& id, CharData&& character )
{
	if( mCharacterStorage.count( id ) != 0 )
	{
		return false;
	}

	mCharacterStorage[id] = rftl::move( character );
	return true;
}



CharData CharacterDatabase::FetchExistingCharacter( CharacterID const& id ) const
{
	CharacterStorage::const_iterator const iter = mCharacterStorage.find( id );
	if( iter == mCharacterStorage.end() )
	{
		return CharData();
	}
	return iter->second;
}



void CharacterDatabase::SubmitOrOverwriteCharacter( CharacterID const& id, CharData&& character )
{
	mCharacterStorage[id] = rftl::move( character );
}



bool CharacterDatabase::OverwriteExistingCharacter( CharacterID const& id, CharData&& character )
{
	CharacterStorage::iterator const iter = mCharacterStorage.find( id );
	if( iter == mCharacterStorage.end() )
	{
		return false;
	}
	iter->second = rftl::move( character );
	return true;
}



bool CharacterDatabase::DeleteExistingCharacter( CharacterID const& id )
{
	CharacterStorage::const_iterator const iter = mCharacterStorage.find( id );
	if( iter == mCharacterStorage.end() )
	{
		return false;
	}
	mCharacterStorage.erase( iter );
	return true;
}



size_t CharacterDatabase::DeleteAllCharacters()
{
	size_t const retVal = mCharacterStorage.size();
	mCharacterStorage.clear();
	return retVal;
}



sprite::CompositeCharacter CharacterDatabase::FetchExistingComposite( CharacterID const& id, CharacterMode const& mode ) const
{
	CompositeStorage::const_iterator const charIter = mCompositeStorage.find( id );
	if( charIter == mCompositeStorage.end() )
	{
		return sprite::CompositeCharacter();
	}

	CompositeModeStorage const& modes = charIter->second;
	CompositeModeStorage::const_iterator const modeIter = modes.find( mode );
	if( modeIter == modes.end() )
	{
		return sprite::CompositeCharacter();
	}
	return modeIter->second;
}



void CharacterDatabase::SubmitOrOverwriteComposite( CharacterID const& id, CharacterMode const& mode, sprite::CompositeCharacter&& composite )
{
	mCompositeStorage[id][mode] = rftl::move( composite );
}



CharacterDatabase::CharacterIDs CharacterDatabase::GetAllCharacterIDs() const
{
	CharacterIDs retVal;
	for( CharacterStorage::value_type const& entry : mCharacterStorage )
	{
		retVal.emplace_back( entry.first );
	}
	return retVal;
}



size_t CharacterDatabase::LoadFromPersistentStorage( file::VFSPath const& directory )
{
	file::VFS const& vfs = *app::gVfs;

	rftl::vector<file::VFSPath> files;
	vfs.EnumerateDirectoryRecursive( directory, file::VFSMount::Permissions::ReadOnly, files );

	// Enumerate all chars
	rftl::string source;
	size_t charsLoaded = 0;
	for( file::VFSPath const& path : files )
	{
		// Build id from file name
		size_t const numElements = path.NumElements();
		RF_ASSERT( numElements > directory.NumElements() );
		rftl::string id = path.GetElement( numElements - 1 );
		RFLOG_TEST_AND_FATAL( id.size() > 3, path, RFCAT_CC3O3, "Expected file to end with '.oo'" );
		RFLOG_TEST_AND_FATAL( id.at( id.size() - 1 ) == 'o', path, RFCAT_CC3O3, "Expected file to end with '.oo'" );
		RFLOG_TEST_AND_FATAL( id.at( id.size() - 2 ) == 'o', path, RFCAT_CC3O3, "Expected file to end with '.oo'" );
		RFLOG_TEST_AND_FATAL( id.at( id.size() - 3 ) == '.', path, RFCAT_CC3O3, "Expected file to end with '.oo'" );
		id.resize( id.size() - 3 );

		// Extract character
		UniquePtr<CharData> character = resource::LoadFromFile<CharData>( path );
		RFLOG_TEST_AND_FATAL( character != nullptr, path, RFCAT_CC3O3, "Failed to load character" );

		// Store character
		SubmitOrOverwriteCharacter( id, rftl::move( *character ) );
		charsLoaded++;
	}

	return charsLoaded;
}



size_t CharacterDatabase::SaveToPersistentStorage( file::VFSPath const& directory )
{
	// TODO
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
}

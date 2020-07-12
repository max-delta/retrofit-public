#include "stdafx.h"
#include "CharacterDatabase.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameScripting/OOLoader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


namespace RF { namespace cc { namespace character {
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
	rftl::vector<file::VFSPath> folders;
	vfs.EnumerateDirectory( directory, file::VFSMount::Permissions::ReadOnly, files, folders );

	if( folders.empty() == false )
	{
		RFLOG_WARNING( directory, RFCAT_CC3O3, "Found unexpected folders when enumerating character directory" );
	}

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

		// Load file
		source.clear();
		{
			file::FileHandlePtr const fileHandle = vfs.GetFileForRead( path );
			RFLOG_TEST_AND_FATAL( fileHandle != nullptr, path, RFCAT_CC3O3, "Failed to open character for read" );
			file::FileBuffer const fileBuffer = file::FileBuffer( *fileHandle, true );
			size_t const fileSize = fileBuffer.GetSize();
			RFLOG_TEST_AND_FATAL( fileSize > 10, path, RFCAT_CC3O3, "Unreasonably small file" );
			RFLOG_TEST_AND_FATAL( fileSize < 10'000, path, RFCAT_CC3O3, "Unreasonably large file" );
			source.assign( fileBuffer.GetChars() );
			RFLOG_TEST_AND_FATAL( source.size() == fileSize, path, RFCAT_CC3O3, "Unexpected file size after load, expected ASCII with no nulls" );
		}

		script::OOLoader loader;

		// Inject types
		{
			bool success;
			success = loader.InjectReflectedClassByCompileType<CharData>( "CharData" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Description>( "Description" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Genetics>( "Genetics" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Visuals>( "Visuals" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Stats>( "Stats" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
		}

		// Inject source
		{
			bool success;
			success = loader.AddSourceFromBuffer( source );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to load" );
		}

		// Extract character
		CharData character = {};
		{
			bool const success = loader.PopulateClass( "c", character );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to populate" );
		}

		// Store character
		SubmitOrOverwriteCharacter( id, rftl::move( character ) );
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
}}}

#include "stdafx.h"
#include "CharacterDatabase.h"

#include "cc3o3/CommonPaths.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameScripting/OOLoader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"


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
	file::VFS const& vfs = *app::gVfs;

	// TODO: Load user characters as well

	// HACK: One hard-coded char
	// TODO: Enumerate all base chars
	rftl::string source;
	{
		CharacterID const id{ "claire" };
		file::VFSPath const path = paths::BaseCharacters().GetChild( "claire.oo" );

		// Load file
		source.clear();
		{
			file::FileHandlePtr const fileHandle = vfs.GetFileForRead( path );
			RFLOG_TEST_AND_FATAL( fileHandle != nullptr, path, RFCAT_CC3O3, "Failed to open character for read" );
			file::FileBuffer const fileBuffer = file::FileBuffer( *fileHandle, true );
			size_t const fileSize = fileBuffer.GetSize();
			RFLOG_TEST_AND_FATAL( fileSize > 10, path, RFCAT_CC3O3, "Unreasonably small file" );
			RFLOG_TEST_AND_FATAL( fileSize < 10'000, path, RFCAT_CC3O3, "Unreasonably large file" );
			source.reserve( fileSize - sizeof( '\0' ) );
			source.assign( reinterpret_cast<char const*>( fileBuffer.GetData() ) );
			RFLOG_TEST_AND_FATAL( source.size() == fileSize - sizeof( '\0' ), path, RFCAT_CC3O3, "Unexpected file size after load, expected ASCII with no nulls" );
		}

		script::OOLoader loader;

		// Inject types
		{
			bool success;
			success = loader.InjectReflectedClassByCompileType<Character>( "Character" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Description>( "Description" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Genetics>( "Genetics" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Visuals>( "Visuals" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Stats>( "Stats" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<Equipment>( "Equipment" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
			success = loader.InjectReflectedClassByCompileType<ElementSlots>( "ElementSlots" );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to inject" );
		}

		// Inject source
		{
			bool success;
			success = loader.AddSourceFromBuffer( source );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to load" );
		}

		// Extract character
		Character character = {};
		{
			bool const success = loader.PopulateClass( "c", character );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to populate" );
		}

		// Store character
		{
			bool const success = SubmitNewCharacter( id, rftl::move( character ) );
			RFLOG_TEST_AND_FATAL( success, path, RFCAT_CC3O3, "Failed to store" );
		}
	}

	return true;
}



bool CharacterDatabase::SaveToPersistentStorage()
{
	// TODO
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}}

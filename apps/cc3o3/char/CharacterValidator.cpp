#include "stdafx.h"
#include "CharacterValidator.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_math/math_casts.h"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

CharacterValidator::CharacterValidator( WeakPtr<file::VFS const> vfs, WeakPtr<sprite::CharacterCreator const> characterCreator )
	: mVfs( vfs )
	, mCharacterCreator( characterCreator )
{
	//
}



bool CharacterValidator::LoadGeneticsTable( file::VFSPath const& geneticsTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> geneticsTable = LoadCSV( geneticsTablePath );
	if( geneticsTable.empty() )
	{
		RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Failed to load genetics table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = geneticsTable.front();
	geneticsTable.pop_front();
	if(
		header.size() != 5 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "species" ||
		header.at( 2 ) != "gender" ||
		header.at( 3 ) != "playable" ||
		header.at( 4 ) != "9-char" )
	{
		RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics table header, expected 'id, species, gender, playable, 9-char'" );
		return false;
	}
	line++;

	while( geneticsTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = geneticsTable.front();
		if( entry.size() != 5 )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, expected 5 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& species = entry.at( 1 );
		rftl::string const& gender = entry.at( 2 );
		rftl::string const& playableString = entry.at( 3 );
		rftl::string const& nineChar = entry.at( 4 );

		if( id.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, not a valid id", math::integer_cast<int>( line ) );
			return false;
		}

		if( species.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, not a valid species", math::integer_cast<int>( line ) );
			return false;
		}

		if( gender.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, not a valid gender", math::integer_cast<int>( line ) );
			return false;
		}

		bool playable = false;
		if( playableString == "1" )
		{
			playable = true;
		}
		else if( playableString == "0" )
		{
			playable = false;
		}
		else
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, not a valid playability", math::integer_cast<int>( line ) );
			return false;
		}

		if( nineChar.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CC3O3, "Malformed genetics entry at line %i, not a valid 9-char", math::integer_cast<int>( line ) );
			return false;
		}

		GeneticsEntry newEntry = {};
		newEntry.mSpecies = species;
		newEntry.mGender = gender;
		newEntry.mPlayable = playable;
		mGeneticsStorage[id] = rftl::move( newEntry );

		geneticsTable.pop_front();
		line++;
	}

	return true;
}



void CharacterValidator::SanitizeForCharacterCreation( Character& character ) const
{
	RF_TODO_BREAK();
}



void CharacterValidator::SanitizeForGameplay( Character& character ) const
{
	RF_TODO_BREAK();
}

///////////////////////////////////////////////////////////////////////////////

rftl::deque<rftl::deque<rftl::string>> CharacterValidator::LoadCSV( file::VFSPath const& path )
{
	file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get file for read" );
		return {};
	}

	file::FileBuffer const buffer{ *handle.Get(), false };
	if( buffer.GetData() == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get data from file buffer" );
		return {};
	}

	char const* const data = reinterpret_cast<char const*>( buffer.GetData() );
	size_t const size = buffer.GetSize();
	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to read file as csv" );
		return {};
	}

	return csv;
}

///////////////////////////////////////////////////////////////////////////////
}}}

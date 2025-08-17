#include "stdafx.h"
#include "CharacterValidator.h"

#include "cc3o3/char/CharData.h"

#include "GameSprite/CharacterCreator.h"

#include "Serialization/CsvReader.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "rftl/sstream"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static void SanitizePiece(
	sprite::CharacterCreator const& charCreate,
	sprite::CharacterCreator::TagBits const& tagBits,
	rftl::string& targetPiece,
	sprite::CharacterPieceType const pieceType )
{
	bool validPiece = false;
	sprite::CharacterCreator::PieceId fallbackPiece;
	sprite::CharacterCreator::PieceId iter = charCreate.IterateNextValidPiece( {}, pieceType, tagBits );
	RF_ASSERT( iter.empty() == false );
	while( iter.empty() == false )
	{
		if( fallbackPiece.empty() )
		{
			fallbackPiece = iter;
		}

		if( targetPiece == iter )
		{
			validPiece = true;
			break;
		}

		iter = charCreate.IterateNextValidPiece( iter, pieceType, tagBits );
		continue;
	}
	if( validPiece == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid piece" );
		RF_ASSERT( fallbackPiece.empty() == false );
		targetPiece = fallbackPiece;
	}
};

}
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
		RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Failed to load genetics table file" );
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
		RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics table header, expected 'id, species, gender, playable, 9-char'" );
		return false;
	}
	line++;

	while( geneticsTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = geneticsTable.front();
		if( entry.size() != 5 )
		{
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, expected 5 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& species = entry.at( 1 );
		rftl::string const& gender = entry.at( 2 );
		rftl::string const& playableString = entry.at( 3 );
		rftl::string const& nineChar = entry.at( 4 );

		if( id.empty() )
		{
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, not a valid id", math::integer_cast<int>( line ) );
			return false;
		}

		if( species.empty() )
		{
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, not a valid species", math::integer_cast<int>( line ) );
			return false;
		}

		if( gender.empty() )
		{
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, not a valid gender", math::integer_cast<int>( line ) );
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
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, not a valid playability", math::integer_cast<int>( line ) );
			return false;
		}

		if( nineChar.empty() )
		{
			RFLOGF_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line {}, not a valid 9-char", math::integer_cast<int>( line ) );
			return false;
		}

		GeneticsEntry newEntry = {};
		newEntry.mSpecies = species;
		newEntry.mGender = gender;
		newEntry.mPlayable = playable;
		newEntry.m9Char = nineChar;
		mGeneticsStorage[id] = rftl::move( newEntry );

		geneticsTable.pop_front();
		line++;
	}

	return true;
}



rftl::unordered_set<CharacterValidator::GeneticsID> CharacterValidator::GetGeneticsIDs() const
{
	rftl::unordered_set<CharacterValidator::GeneticsID> retVal;
	for( GeneticsStorage::value_type const& pair : mGeneticsStorage )
	{
		retVal.emplace( pair.first );
	}
	return retVal;
}



GeneticsEntry const& CharacterValidator::GetGeneticEntry( GeneticsID const& id ) const
{
	return mGeneticsStorage.at( id );
}



CharacterValidator::GeneticsID CharacterValidator::GetGeneticsID( rftl::string const& species, rftl::string const& gender ) const
{
	for( GeneticsStorage::value_type const& pair : mGeneticsStorage )
	{
		if( pair.second.mSpecies == species && pair.second.mGender == gender )
		{
			return pair.first;
		}
	}

	return GeneticsID();
}



bool CharacterValidator::LoadStatBonusesTable( file::VFSPath const& statBonusesTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> statBonusesTable = LoadCSV( statBonusesTablePath );
	if( statBonusesTable.empty() )
	{
		RFLOG_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Failed to load stat bonuses table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = statBonusesTable.front();
	statBonusesTable.pop_front();
	if(
		header.size() != 9 ||
		header.at( 0 ) != "species" ||
		header.at( 1 ) != "mhealth" ||
		header.at( 2 ) != "physatk" ||
		header.at( 3 ) != "physdef" ||
		header.at( 4 ) != "elematk" ||
		header.at( 5 ) != "elemdef" ||
		header.at( 6 ) != "balance" ||
		header.at( 7 ) != "techniq" ||
		header.at( 8 ) != "elempwr" )
	{
		RFLOG_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses table header, expected 'mhealth, physatk, physdef, elematk, elemdef, balance, techniq, elempwr'" );
		return false;
	}
	line++;

	while( statBonusesTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = statBonusesTable.front();
		if( entry.size() != 9 )
		{
			RFLOGF_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line {}, expected 9 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& species = entry.at( 0 );
		rftl::string const& mhealthString = entry.at( 1 );
		rftl::string const& physatkString = entry.at( 2 );
		rftl::string const& physdefString = entry.at( 3 );
		rftl::string const& elematkString = entry.at( 4 );
		rftl::string const& elemdefString = entry.at( 5 );
		rftl::string const& balanceString = entry.at( 6 );
		rftl::string const& techniqString = entry.at( 7 );
		rftl::string const& elempwrString = entry.at( 8 );

		if( species.empty() )
		{
			RFLOGF_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line {}, not a valid species", math::integer_cast<int>( line ) );
			return false;
		}

		rftl::stringstream ss;

		static constexpr auto load = []( rftl::stringstream& ss, rftl::string const& str, uint8_t& val ) -> bool //
		{
			ss.clear();
			ss << str;
			uint16_t notAChar; // C++ I/O is stupid
			ss >> notAChar;
			val = math::integer_cast<uint8_t>( notAChar );
			RF_CLANG_PUSH();
			RF_CLANG_IGNORE( "-Wtautological-unsigned-zero-compare" ); // Min value may be zero
			if( val < kMinStatValue || val > kMaxStatValue )
			{
				return false;
			}
			RF_CLANG_POP();
			return true;
		};

		Stats stats = {};
		bool validStats =
			load( ss, mhealthString, stats.mMHealth ) &&
			load( ss, physatkString, stats.mPhysAtk ) &&
			load( ss, physdefString, stats.mPhysDef ) &&
			load( ss, elematkString, stats.mElemAtk ) &&
			load( ss, elemdefString, stats.mElemDef ) &&
			load( ss, balanceString, stats.mBalance ) &&
			load( ss, techniqString, stats.mTechniq ) &&
			load( ss, elempwrString, stats.mElemPwr );
		if( validStats == false )
		{
			RFLOGF_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line {}, not a valid modifier", math::integer_cast<int>( line ) );
			return false;
		}

		mStatBonusesStorage[species] = rftl::move( stats );

		statBonusesTable.pop_front();
		line++;
	}

	return true;
}



Stats const& CharacterValidator::GetStatBonuses( SpeciesID const& id ) const
{
	StatBonusesStorage::const_iterator const iter = mStatBonusesStorage.find( id );
	if( iter == mStatBonusesStorage.end() )
	{
		RFLOG_FATAL( nullptr, RFCAT_CHAR, "Species has no stat bonus entries" );
	}
	return iter->second;
}



uint8_t CharacterValidator::CalculateTotalPoints( Stats const& stats )
{
	return math::integer_cast<uint8_t>( 0 +
		stats.mMHealth +
		stats.mPhysAtk +
		stats.mPhysDef +
		stats.mElemAtk +
		stats.mElemDef +
		stats.mBalance +
		stats.mTechniq +
		stats.mElemPwr );
}



void CharacterValidator::SanitizeForCharacterCreation( CharData& character ) const
{
	Description& description = character.mDescription;
	rftl::string& innate = character.mInnate;
	Genetics& genetics = character.mGenetics;
	Visuals& visuals = character.mVisuals;
	Stats& stats = character.mStats;

	// Name
	if( description.mName.empty() )
	{
		RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid name" );
		description.mName = "CHARACTERNAME";
	}

	// TODO: Check for valid titles from a list
	if( description.mTitle.empty() == false )
	{
		RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid title" );
		description.mTitle = "";
	}

	// TODO: Check for valid innates from a list
	if( innate.size() != 3 )
	{
		RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid innate" );
		innate = "inv";
	}

	// Species
	{
		RF_ASSERT( mGeneticsStorage.empty() == false );
		bool validSpecies = false;
		rftl::string fallbackSpecies;
		for( GeneticsStorage::value_type const& pair : mGeneticsStorage )
		{
			if( pair.second.mPlayable == false )
			{
				continue;
			}

			if( fallbackSpecies.empty() )
			{
				fallbackSpecies = pair.second.mSpecies;
			}

			if( genetics.mSpecies == pair.second.mSpecies )
			{
				validSpecies = true;
				break;
			}
		}
		if( validSpecies == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid species" );
			RF_ASSERT( fallbackSpecies.empty() == false );
			genetics.mSpecies = fallbackSpecies;
		}
	}

	// Gender
	{
		RF_ASSERT( mGeneticsStorage.empty() == false );
		bool validGender = false;
		rftl::string fallbackGender;
		for( GeneticsStorage::value_type const& pair : mGeneticsStorage )
		{
			if( pair.second.mPlayable == false )
			{
				continue;
			}

			if( pair.second.mSpecies != genetics.mSpecies )
			{
				continue;
			}

			if( fallbackGender.empty() )
			{
				fallbackGender = pair.second.mGender;
			}

			if( genetics.mGender == pair.second.mGender )
			{
				validGender = true;
				break;
			}
		}
		if( validGender == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid gender" );
			RF_ASSERT( fallbackGender.empty() == false );
			genetics.mGender = fallbackGender;
		}
	}

	// Visuals
	if( visuals.mCompositable )
	{
		sprite::CharacterCreator const& charCreate = *mCharacterCreator;
		sprite::CharacterCreator::TagBits const tagBits =
			charCreate.GetTagsAsFlags(
				{ genetics.mSpecies, genetics.mGender } );

		details::SanitizePiece( charCreate, tagBits, visuals.mBase, sprite::CharacterPieceType::Base );
		details::SanitizePiece( charCreate, tagBits, visuals.mTop, sprite::CharacterPieceType::Top );
		details::SanitizePiece( charCreate, tagBits, visuals.mBottom, sprite::CharacterPieceType::Bottom );
		details::SanitizePiece( charCreate, tagBits, visuals.mHair, sprite::CharacterPieceType::Hair );
		details::SanitizePiece( charCreate, tagBits, visuals.mSpecies, sprite::CharacterPieceType::Species );
	}
	else
	{
		bool const allPiecesEmpty =
			visuals.mBase.empty() &&
			visuals.mTop.empty() &&
			visuals.mBottom.empty() &&
			visuals.mHair.empty() &&
			visuals.mSpecies.empty();
		if( allPiecesEmpty == false )
		{
			RFLOG_WARNING( nullptr, RFCAT_CHAR, "One or more pieces set on a noncompositable character" );
		}
	}

	// Stats
	{
		Stats sanitizedStats = GetStatBonuses( genetics.mSpecies );
		uint16_t availablePoints = math::integer_cast<uint16_t>( kMaxTotalStatPoints - CalculateTotalPoints( sanitizedStats ) );
		RF_ASSERT( availablePoints >= 0 );
		while( availablePoints > 0 )
		{
			static constexpr auto siphon = []( uint16_t& reserve, uint8_t& cur, uint8_t const& desire ) -> bool //
			{
				if(
					reserve > 0 &&
					cur < desire &&
					cur < kMaxPointsPerStat )
				{
					cur++;
					reserve--;
					return true;
				}
				return false;
			};

			Stats const& desiredStats = stats;
			bool const siphoned =
				siphon( availablePoints, sanitizedStats.mMHealth, desiredStats.mMHealth ) ||
				siphon( availablePoints, sanitizedStats.mPhysAtk, desiredStats.mPhysAtk ) ||
				siphon( availablePoints, sanitizedStats.mPhysDef, desiredStats.mPhysDef ) ||
				siphon( availablePoints, sanitizedStats.mElemAtk, desiredStats.mElemAtk ) ||
				siphon( availablePoints, sanitizedStats.mElemDef, desiredStats.mElemDef ) ||
				siphon( availablePoints, sanitizedStats.mBalance, desiredStats.mBalance ) ||
				siphon( availablePoints, sanitizedStats.mTechniq, desiredStats.mTechniq ) ||
				siphon( availablePoints, sanitizedStats.mElemPwr, desiredStats.mElemPwr );
			if( siphoned )
			{
				continue;
			}
			else
			{
				break;
			}
		}

		sanitizedStats.mGridShp = static_cast<GridShape>( math::Clamp<uint8_t>( 0, static_cast<uint8_t>( stats.mGridShp ), kMaxGridShapeValue ) );
		stats = sanitizedStats;
	}
}



void CharacterValidator::SanitizeForGameplay( CharData& character ) const
{
	Stats& stats = character.mStats;

	SanitizeForCharacterCreation( character );

	// Stats
	{
		uint16_t availablePoints = math::integer_cast<uint16_t>( kMaxTotalStatPoints - CalculateTotalPoints( stats ) );
		RF_ASSERT( availablePoints >= 0 );
		while( availablePoints > 0 )
		{
			static constexpr auto grow = []( uint16_t& reserve, uint8_t& dest ) -> bool //
			{
				if(
					reserve > 0 &&
					dest < kMaxPointsPerStat )
				{
					dest++;
					reserve--;
					return true;
				}
				return false;
			};

			// NOTE: Health intentionally ommitted
			// NOTE: Ordered to favor 'simplest' stats for padding
			bool const grown =
				grow( availablePoints, stats.mPhysDef ) ||
				grow( availablePoints, stats.mPhysAtk ) ||
				grow( availablePoints, stats.mElemDef ) ||
				grow( availablePoints, stats.mElemAtk ) ||
				grow( availablePoints, stats.mElemPwr ) ||
				grow( availablePoints, stats.mBalance ) ||
				grow( availablePoints, stats.mTechniq );
			if( grown )
			{
				RFLOG_WARNING( nullptr, RFCAT_CHAR, "Had to pad some stats since not all points were allocated" );
				continue;
			}
			else
			{
				RF_ASSERT( availablePoints == 0 );
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

rftl::deque<rftl::deque<rftl::string>> CharacterValidator::LoadCSV( file::VFSPath const& path )
{
	file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CHAR, "Failed to get file for read" );
		return {};
	}

	file::FileBuffer const buffer{ *handle.Get(), false };
	if( buffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CHAR, "Failed to get data from file buffer" );
		return {};
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( buffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CHAR, "Failed to read file as csv" );
		return {};
	}

	return csv;
}

///////////////////////////////////////////////////////////////////////////////
}

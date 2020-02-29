#include "stdafx.h"
#include "CharacterValidator.h"

#include "cc3o3/char/Character.h"

#include "GameSprite/CharacterCreator.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_math/math_casts.h"

#include "rftl/sstream"
#include "rftl/numeric"
#include "rftl/algorithm"


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
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, expected 5 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& species = entry.at( 1 );
		rftl::string const& gender = entry.at( 2 );
		rftl::string const& playableString = entry.at( 3 );
		rftl::string const& nineChar = entry.at( 4 );

		if( id.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, not a valid id", math::integer_cast<int>( line ) );
			return false;
		}

		if( species.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, not a valid species", math::integer_cast<int>( line ) );
			return false;
		}

		if( gender.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, not a valid gender", math::integer_cast<int>( line ) );
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
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, not a valid playability", math::integer_cast<int>( line ) );
			return false;
		}

		if( nineChar.empty() )
		{
			RFLOG_NOTIFY( geneticsTablePath, RFCAT_CHAR, "Malformed genetics entry at line %i, not a valid 9-char", math::integer_cast<int>( line ) );
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
			RFLOG_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line %i, expected 9 columns", math::integer_cast<int>( line ) );
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
			RFLOG_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line %i, not a valid species", math::integer_cast<int>( line ) );
			return false;
		}

		rftl::stringstream ss;

		static constexpr auto load = []( rftl::stringstream& ss, rftl::string const& str, int8_t& val ) -> bool //
		{
			ss.clear();
			ss << str;
			int16_t notAChar; // C++ I/O is stupid
			ss >> notAChar;
			val = math::integer_cast<int8_t>( notAChar );
			if( val < Stats::kMinModifier || val > Stats::kMaxModifier )
			{
				return false;
			}
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
			RFLOG_NOTIFY( statBonusesTablePath, RFCAT_CHAR, "Malformed stat bonuses entry at line %i, not a valid modifier", math::integer_cast<int>( line ) );
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
	return mStatBonusesStorage.at( id );
}



int8_t CharacterValidator::CalculateTotalPoints( Stats const& stats )
{
	return 0 +
		stats.mMHealth +
		stats.mPhysAtk +
		stats.mPhysDef +
		stats.mElemAtk +
		stats.mElemDef +
		stats.mBalance +
		stats.mTechniq +
		stats.mElemPwr;
}



size_t CharacterValidator::GetMinimumTotalSlots( uint8_t storyTier ) const
{
	SlotsPerElemLevel const minSlots = GetMinimumSlotDistribution( storyTier );
	return rftl::accumulate( minSlots.begin(), minSlots.end(), SlotsPerElemLevel::value_type{} );
}



CharacterValidator::SlotsPerElemLevel CharacterValidator::GetMinimumSlotDistribution( uint8_t storyTier ) const
{
	switch( storyTier )
	{
		case 0:
			// Invalid
			return { 1, 1, 1, 0, 0, 0, 0, 0 };
		case 1:
			// Tutorial
			return { 1, 1, 1, 0, 0, 0, 0, 0 };
		case 2:
			// Tutorial
			return { 2, 1, 1, 0, 0, 0, 0, 0 };
		case 3:
			// Tutorial
			return { 2, 2, 1, 0, 0, 0, 0, 0 };
		case 4:
			// Unlock 4
			return { 2, 2, 1, 1, 0, 0, 0, 0 };
		case 5:
			// Unlock 5
			return { 2, 2, 2, 1, 1, 0, 0, 0 };
		case 6:
			// Unlock 6
			return { 3, 2, 2, 3, 1, 1, 0, 0 };
		case 7:
			// Unlock 7
			return { 3, 3, 3, 3, 2, 2, 1, 0 };
		case 8:
			// Unlock 8
			return { 3, 3, 3, 3, 2, 2, 1, 1 };
		default:
			// Unbalanced
			return { 3, 3, 3, 3, 3, 3, 2, 2 };
	}
}



size_t CharacterValidator::CalculateTotalSlots( Stats::StatModifier elemPower, uint8_t storyTier ) const
{
	size_t const min = GetMinimumTotalSlots( storyTier );
	size_t const power = math::integer_cast<size_t>( elemPower );
	switch( storyTier )
	{
		case 0:
			// Invalid
			return min;
		case 1:
			// Tutorial
			return min;
		case 2:
			// Tutorial
			return min;
		case 3:
			// Tutorial
			return min;
		case 4:
			// Unlock 4
			return min + ( power + 1 ) / 2;
		case 5:
			// Unlock 5
			return min + power * 1;
		case 6:
			// Unlock 6
			return min + power * 1;
		case 7:
			// Unlock 7
			return min + power * 1;
		case 8:
			// Unlock 8
			return min + power * 2;
		default:
			// Unbalanced
			return min + power * 2 + 5;
	}
}



CharacterValidator::SlotsPerElemLevel CharacterValidator::CalculateSlotDistribution( Stats::StatModifier elemPower, Stats::GridShape gridShape, uint8_t storyTier ) const
{
	// NOTE: Non-optimal, but not expected to be called frequently

	// Will level up from the start of the game, and assign slots
	SlotsPerElemLevel expenditures = {};
	for( uint8_t i_story = 1; i_story <= storyTier; i_story++ )
	{
		// We just leveled up, how many slots do we have to spend
		size_t const slotsLastTier = CalculateTotalSlots( elemPower, i_story - 1u ) - GetMinimumTotalSlots( i_story - 1u );
		size_t const slotsThisTier = CalculateTotalSlots( elemPower, i_story ) - GetMinimumTotalSlots( i_story );
		RF_ASSERT( slotsThisTier >= slotsLastTier );
		size_t const newSlotsThisTier = slotsThisTier - slotsLastTier;

		// Spend each slot
		SlotsPerElemLevel const min = GetMinimumSlotDistribution( i_story );
		for( size_t i_expenditure = 0; i_expenditure < newSlotsThisTier; i_expenditure++ )
		{
			// Where are we at now?
			SlotsPerElemLevel currentSlots = min;
			for( size_t i_level = 0; i_level < kMaxElementLevels; i_level++ )
			{
				currentSlots.at( i_level ) += expenditures.at( i_level );
			}

			// How do we rate our choices?
			rftl::array<size_t, kMaxElementLevels> weights = {};
			for( size_t i_level = 0; i_level < kMaxElementLevels; i_level++ )
			{
				size_t const& cur = currentSlots.at( i_level );
				size_t& weight = weights.at( i_level );

				if( cur == 0 )
				{
					// Can't choose this
					weight = 0;
					continue;
				}

				using GridShape = Stats::GridShape;
				switch( gridShape )
				{
					case GridShape::Standard:
						// Prefer earlier slots
						// 3  2  1
						// 4  3  2
						// 5  4  3
						// -------
						// 0  1  2
						weight = ( kMaxSlotsPerElementLevel - cur ) + ( kMaxElementLevels - i_level );
						break;
					case GridShape::Wide:
						// Want all levels to have equal number of slots
						// 1  1  1
						// 2  2  2
						// 3  3  3
						// -------
						// 0  1  2
						weight = kMaxSlotsPerElementLevel - cur;
						break;
					case GridShape::Heavy:
						// Prefer later slots
						// 1  2  3
						// 2  3  4
						// 3  4  5
						// -------
						// 0  1  2
						weight = ( kMaxSlotsPerElementLevel - cur ) + i_level;
						break;
					case GridShape::NumShapeTypes:
					default:
						RF_DBGFAIL();
						break;
				}
			}

			// Find the best choice
			size_t bestWeightLevel;
			{
				// Sort the weights
				rftl::static_vector<rftl::pair<size_t, size_t>, kMaxElementLevels> weightsToLevels;
				for( size_t i_level = 0; i_level < kMaxElementLevels; i_level++ )
				{
					weightsToLevels.emplace_back( { weights.at( i_level ), i_level } );
				}
				rftl::sort( weightsToLevels.begin(), weightsToLevels.end() );
				rftl::reverse( weightsToLevels.begin(), weightsToLevels.end() );

				// Remove everything below the maximum weight found
				size_t const bestWeight = weightsToLevels.front().first;
				while( weightsToLevels.back().first < bestWeight )
				{
					weightsToLevels.pop_back();
				}

				RF_ASSERT( weightsToLevels.empty() == false );
				if( weightsToLevels.size() == 1 )
				{
					// Only one choice
					bestWeightLevel = weightsToLevels.front().second;
				}
				else
				{
					// Competing choices

					rftl::static_vector<size_t, kMaxElementLevels> levelChoices;
					for( rftl::pair<size_t, size_t> const& weightToLevel : weightsToLevels )
					{
						levelChoices.emplace_back( weightToLevel.second );
					}
					rftl::sort(
						levelChoices.begin(),
						levelChoices.end(),
						[&currentSlots, gridShape]( size_t const& lhs, size_t const& rhs ) -> bool //
						{
							// Returning true here for {lhs, rhs} means lhs is
							//  preferred, and is moved towards front of list

							// Prefer to equalize levels
							if( currentSlots.at( lhs ) < currentSlots.at( rhs ) )
							{
								return true;
							}
							else if( currentSlots.at( lhs ) > currentSlots.at( rhs ) )
							{
								return false;
							}

							using GridShape = Stats::GridShape;
							switch( gridShape )
							{
								case GridShape::Standard:
								case GridShape::Wide:
									// Prefer earlier slots
									return lhs < rhs;
								case GridShape::Heavy:
									// Prefer later slots
									return lhs > rhs;
								case GridShape::NumShapeTypes:
								default:
									RF_DBGFAIL();
									return false;
							}
						} );
					bestWeightLevel = levelChoices.front();
				}
			}

			// Spend the point
			expenditures.at( bestWeightLevel )++;
		}
	}

	// Apply the expenditures
	SlotsPerElemLevel retVal = GetMinimumSlotDistribution( storyTier );
	for( size_t i_level = 0; i_level < kMaxElementLevels; i_level++ )
	{
		retVal.at( i_level ) += expenditures.at( i_level );
	}
	return retVal;
}



void CharacterValidator::SanitizeForCharacterCreation( Character& character ) const
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
	if( innate != "yellow" )
	{
		RFLOG_WARNING( nullptr, RFCAT_CHAR, "Invalid innate" );
		innate = "yellow";
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
	{
		sprite::CharacterCreator const& charCreate = *mCharacterCreator;
		sprite::CharacterCreator::TagBits const tagBits =
			charCreate.GetTagsAsFlags(
				{ genetics.mSpecies, genetics.mGender } );

		static constexpr auto sanitizePiece = [](
												  sprite::CharacterCreator const& charCreate,
												  sprite::CharacterCreator::TagBits const& tagBits,
												  rftl::string& targetPiece,
												  sprite::CharacterPieceType const pieceType ) -> void {
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

		sanitizePiece( charCreate, tagBits, visuals.mBase, sprite::CharacterPieceType::Base );
		sanitizePiece( charCreate, tagBits, visuals.mTop, sprite::CharacterPieceType::Top );
		sanitizePiece( charCreate, tagBits, visuals.mBottom, sprite::CharacterPieceType::Bottom );
		sanitizePiece( charCreate, tagBits, visuals.mHair, sprite::CharacterPieceType::Hair );
		sanitizePiece( charCreate, tagBits, visuals.mSpecies, sprite::CharacterPieceType::Species );
	}

	// Stats
	{
		Stats sanitizedStats = GetStatBonuses( genetics.mSpecies );
		int16_t availablePoints = kMaxTotalStatPoints - CalculateTotalPoints( sanitizedStats );
		RF_ASSERT( availablePoints >= 0 );
		while( availablePoints > 0 )
		{
			static constexpr auto siphon = []( int16_t& reserve, int8_t& cur, int8_t const& desire ) -> bool //
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

		sanitizedStats.mGridShp = static_cast<Stats::GridShape>( math::Clamp<uint8_t>( 0, static_cast<uint8_t>( stats.mGridShp ), Stats::kMaxShapeValue ) );
		stats = sanitizedStats;
	}
}



void CharacterValidator::SanitizeForGameplay( Character& character ) const
{
	Stats& stats = character.mStats;
	Equipment& equipment = character.mEquipment;
	ElementSlots& innateElements = character.mInnateElements;
	ElementSlots& equippedElements = character.mEquippedElements;

	SanitizeForCharacterCreation( character );

	// Stats
	{
		int16_t availablePoints = kMaxTotalStatPoints - CalculateTotalPoints( stats );
		RF_ASSERT( availablePoints >= 0 );
		while( availablePoints > 0 )
		{
			static constexpr auto grow = []( int16_t& reserve, int8_t& dest ) -> bool //
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

	// TODO: Check for valid equipment from a list
	equipment = {};

	// TODO: Check for valid innates from a list
	innateElements = {};

	// TODO: Check for valid elements from a list
	equippedElements = {};
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
	if( buffer.GetData() == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CHAR, "Failed to get data from file buffer" );
		return {};
	}

	char const* const data = reinterpret_cast<char const*>( buffer.GetData() );
	size_t const size = buffer.GetSize();
	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CHAR, "Failed to read file as csv" );
		return {};
	}

	return csv;
}

///////////////////////////////////////////////////////////////////////////////
}}}

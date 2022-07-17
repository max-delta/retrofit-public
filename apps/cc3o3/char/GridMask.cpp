#include "stdafx.h"
#include "GridMask.h"

#include "RFType/CreateClassInfoDefinition.h"

// TODO: Add an extension for this
//#include "core_rftype/stl_extensions/array.h"

#include "core_math/math_casts.h"

#include "rftl/algorithm"
#include "rftl/numeric"
#include "rftl/extension/static_vector.h"

RFTYPE_CREATE_META( RF::cc::character::GridMask )
{
	using namespace RF::cc::character;
	// TODO: Add an extension for this (rftl::array<...>)
	//RFTYPE_META().ExtensionProperty( "mSlotsPerElemLevel", &GridMask::mSlotsPerElemLevel );
	RFTYPE_REGISTER_BY_NAME( "GridMask" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::GridMask );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////
namespace details {

GridMask::SlotsPerElemLevel CalcMinimumSlotsPerLevel( company::StoryTier storyTier )
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

}
///////////////////////////////////////////////////////////////////////////////

size_t const& GridMask::GetNumSlotsAtLevel( element::ElementLevel level ) const
{
	size_t const levelOffset = element::AsLevelOffset( level );
	return mSlotsPerElemLevel.at( levelOffset );
}



size_t GridMask::CalcMinimumTotalSlots( company::StoryTier storyTier )
{
	SlotsPerElemLevel const minSlots = CalcMinimumSlots( storyTier ).mSlotsPerElemLevel;
	return rftl::accumulate( minSlots.begin(), minSlots.end(), SlotsPerElemLevel::value_type{} );
}



GridMask GridMask::CalcMinimumSlots( company::StoryTier storyTier )
{
	GridMask retVal = {};
	retVal.mSlotsPerElemLevel = details::CalcMinimumSlotsPerLevel( storyTier );
	return retVal;
}



size_t GridMask::CalcTotalSlots( StatValue elemPower, company::StoryTier storyTier )
{
	size_t const min = CalcMinimumTotalSlots( storyTier );
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



GridMask GridMask::CalcSlots( StatValue elemPower, GridShape gridShape, company::StoryTier storyTier )
{
	// NOTE: Non-optimal, but not expected to be called frequently

	// Will level up from the start of the game, and assign slots
	SlotsPerElemLevel expenditures = {};
	for( uint8_t i_story = 1; i_story <= storyTier; i_story++ )
	{
		// We just leveled up, how many slots do we have to spend
		size_t const slotsLastTier = CalcTotalSlots( elemPower, i_story - 1u ) - CalcMinimumTotalSlots( i_story - 1u );
		size_t const slotsThisTier = CalcTotalSlots( elemPower, i_story ) - CalcMinimumTotalSlots( i_story );
		RF_ASSERT( slotsThisTier >= slotsLastTier );
		size_t const newSlotsThisTier = slotsThisTier - slotsLastTier;

		// Spend each slot
		SlotsPerElemLevel const min = CalcMinimumSlots( i_story ).mSlotsPerElemLevel;
		for( size_t i_expenditure = 0; i_expenditure < newSlotsThisTier; i_expenditure++ )
		{
			// Where are we at now?
			SlotsPerElemLevel currentSlots = min;
			for( element::ElementLevel const& level : element::kElementLevels )
			{
				size_t const levelOffset = element::AsLevelOffset( level );
				currentSlots.at( levelOffset ) += expenditures.at( levelOffset );
			}

			// How do we rate our choices?
			rftl::array<size_t, element::kNumElementLevels> weights = {};
			for( element::ElementLevel const& level : element::kElementLevels )
			{
				size_t const levelOffset = element::AsLevelOffset( level );

				size_t const& cur = currentSlots.at( levelOffset );
				size_t& weight = weights.at( levelOffset );

				if( cur == 0 )
				{
					// Can't choose this
					weight = 0;
					continue;
				}

				switch( gridShape )
				{
					case GridShape::Standard:
						// Prefer earlier slots
						// 3  2  1
						// 4  3  2
						// 5  4  3
						// -------
						// 0  1  2
						weight = ( kMaxSlotsPerElementLevel - cur ) + ( element::kMaxElementLevel - level );
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
						weight = ( kMaxSlotsPerElementLevel - cur ) + ( level - element::kMinElementLevel );
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
				rftl::static_vector<rftl::pair<size_t, size_t>, element::kNumElementLevels> weightsToLevels;
				for( element::ElementLevel const& level : element::kElementLevels )
				{
					size_t const levelOffset = element::AsLevelOffset( level );
					weightsToLevels.emplace_back( { weights.at( levelOffset ), levelOffset } );
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

					rftl::static_vector<size_t, element::kNumElementLevels> levelChoices;
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
	GridMask retVal = CalcMinimumSlots( storyTier );
	for( element::ElementLevel const& level : element::kElementLevels )
	{
		size_t const levelOffset = element::AsLevelOffset( level );
		retVal.mSlotsPerElemLevel.at( levelOffset ) += expenditures.at( levelOffset );
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

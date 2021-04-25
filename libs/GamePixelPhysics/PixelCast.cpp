#include "stdafx.h"
#include "PixelCast.h"

#include "Logging/Logging.h"

#include "core_math/Bitmap.h"
#include "core_math/Vector2.h"

#include "rftl/extension/static_vector.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using StepAttempts = rftl::static_vector<PhysCoord, 3>;



PhysCoord GetStep( Direction::Value direction )
{
	using namespace Direction;

	switch( direction )
	{
		case North:
			return PhysCoord( 0, -1 );
		case East:
			return PhysCoord( 1, 0 );
		case South:
			return PhysCoord( 0, 1 );
		case West:
			return PhysCoord( -1, 0 );
		case NE:
			return PhysCoord( 1, -1 );
		case NW:
			return PhysCoord( -1, -1 );
		case SE:
			return PhysCoord( 1, 1 );
		case SW:
			return PhysCoord( -1, 1 );
		case Invalid:
			return PhysCoord( 0, 0 );
		default:
			RF_DBGFAIL();
			return PhysCoord( 0, 0 );
	}
}



PhysCoord StepCast(
	math::Bitmap const& collisionMap,
	PhysCoord const& pos,
	StepAttempts const& attempts )
{
	RF_ASSERT( pos.x >= 0 );
	RF_ASSERT( pos.y >= 0 );
	RF_ASSERT( pos.x <= math::integer_cast<PhysCoordElem>( collisionMap.GetWidth() ) );
	RF_ASSERT( pos.y <= math::integer_cast<PhysCoordElem>( collisionMap.GetHeight() ) );
	RF_ASSERT( attempts.empty() == false );

	for( PhysCoord const& attempt : attempts )
	{
		if( PixelCast::HasCollision( collisionMap, attempt ) == false )
		{
			return attempt;
		}
	}

	return pos;
}

}
///////////////////////////////////////////////////////////////////////////////

bool PixelCast::HasCollision( math::Bitmap const& collisionMap, PhysCoord const& pos )
{
	if( pos.x < 0 || pos.y < 0 )
	{
		// Out of bounds
		return true;
	}
	size_t const x = math::integer_cast<size_t>( pos.x );
	size_t const y = math::integer_cast<size_t>( pos.y );

	if( collisionMap.IsInBounds( x, y ) == false )
	{
		// Out of bounds
		return true;
	}

	bool const hasCollision = collisionMap.GetBit( x, y );
	return hasCollision;
}



PhysCoord PixelCast::FixOutOfBounds( math::Bitmap const& collisionMap, PhysCoord const& pos )
{
	if( HasCollision( collisionMap, pos ) == false )
	{
		// Already safe
		return pos;
	}

	// Naive scanline approach, wildly unsafe
	RFLOG_WARNING( nullptr, RFCAT_GAMEPIXELPHYSICS, "Trying naive out-of-bounds fix" );
	PhysCoordElem const width = math::integer_cast<PhysCoordElem>( collisionMap.GetWidth() );
	PhysCoordElem const height = math::integer_cast<PhysCoordElem>( collisionMap.GetHeight() );
	for( PhysCoordElem y = 0; y < height; y++ )
	{
		for( PhysCoordElem x = 0; x < width; x++ )
		{
			PhysCoord const testPos{ x, y };
			if( HasCollision( collisionMap, testPos ) == false )
			{
				return testPos;
			}
		}
	}

	// Failed
	return pos;
}




PhysCoord PixelCast::CardinalStepCast(
	math::Bitmap const& collisionMap,
	PhysCoord const& pos,
	Direction::Value direction )
{
	details::StepAttempts attempts;
	{
		using namespace Direction;

		auto const addAttempt = [&attempts, &pos]( Value dir ) -> void //
		{
			RF_ASSERT( dir != Invalid );
			attempts.emplace_back( pos + details::GetStep( dir ) );
		};

		switch( direction )
		{
			// Cardinals should feel precise
			case North:
				addAttempt( North );
				break;
			case East:
				addAttempt( East );
				break;
			case South:
				addAttempt( South );
				break;
			case West:
				addAttempt( West );
				break;
			// Inter-cardinals should feel like a combination of cardinals
			case NE:
				addAttempt( NE );
				addAttempt( North );
				addAttempt( East );
				break;
			case NW:
				addAttempt( NW );
				addAttempt( North );
				addAttempt( West );
				break;
			case SE:
				addAttempt( SE );
				addAttempt( South );
				addAttempt( East );
				break;
			case SW:
				addAttempt( SW );
				addAttempt( South );
				addAttempt( West );
				break;
			case Invalid:
			default:
				RF_DBGFAIL();
				break;
		}
	}
	RF_ASSERT( attempts.empty() == false );

	return details::StepCast( collisionMap, pos, attempts );
}



PhysCoord PixelCast::SlideStepCast(
	math::Bitmap const& collisionMap,
	PhysCoord const& pos,
	Direction::Value direction )
{
	details::StepAttempts attempts;
	{
		using namespace Direction;

		auto const addAttempt = [&attempts, &pos]( Value dir ) -> void //
		{
			RF_ASSERT( dir != Invalid );
			attempts.emplace_back( pos + details::GetStep( dir ) );
		};

		switch( direction )
		{
			// All directions should feel like a quadrant cone
			case North:
				addAttempt( North );
				addAttempt( NE );
				addAttempt( NW );
				break;
			case East:
				addAttempt( East );
				addAttempt( NE );
				addAttempt( SE );
				break;
			case South:
				addAttempt( South );
				addAttempt( SE );
				addAttempt( SW );
				break;
			case West:
				addAttempt( West );
				addAttempt( NW );
				addAttempt( SW );
				break;
			case NE:
				addAttempt( NE );
				addAttempt( North );
				addAttempt( East );
				break;
			case NW:
				addAttempt( NW );
				addAttempt( North );
				addAttempt( West );
				break;
			case SE:
				addAttempt( SE );
				addAttempt( South );
				addAttempt( East );
				break;
			case SW:
				addAttempt( SW );
				addAttempt( South );
				addAttempt( West );
				break;
			case Invalid:
			default:
				RF_DBGFAIL();
				break;
		}
	}
	RF_ASSERT( attempts.empty() == false );

	return details::StepCast( collisionMap, pos, attempts );
}

///////////////////////////////////////////////////////////////////////////////
}

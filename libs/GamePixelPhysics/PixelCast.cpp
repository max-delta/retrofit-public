#include "stdafx.h"
#include "PixelCast.h"

#include "Logging/Logging.h"

#include "core_math/Bitmap.h"
#include "core_math/Vector2.h"

#include "rftl/extension/static_vector.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using StepAttempts = rftl::static_vector<Coord, 3>;



Coord GetStep( Direction::Value direction )
{
	using namespace Direction;

	switch( direction )
	{
		case North:
			return Coord( 0, -1 );
		case East:
			return Coord( 1, 0 );
		case South:
			return Coord( 0, 1 );
		case West:
			return Coord( -1, 0 );
		case NE:
			return Coord( 1, -1 );
		case NW:
			return Coord( -1, -1 );
		case SE:
			return Coord( 1, 1 );
		case SW:
			return Coord( -1, 1 );
		case Invalid:
			return Coord( 0, 0 );
		default:
			RF_DBGFAIL();
			return Coord( 0, 0 );
	}
}



Coord StepCast(
	math::Bitmap const& collisionMap,
	Coord const& pos,
	StepAttempts const& attempts )
{
	RF_ASSERT( pos.x >= 0 );
	RF_ASSERT( pos.y >= 0 );
	RF_ASSERT( pos.x <= math::integer_cast<CoordElem>( collisionMap.GetWidth() ) );
	RF_ASSERT( pos.y <= math::integer_cast<CoordElem>( collisionMap.GetHeight() ) );
	RF_ASSERT( attempts.empty() == false );

	for( Coord const& attempt : attempts )
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

bool PixelCast::HasCollision( math::Bitmap const& collisionMap, Coord const& pos )
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



Coord PixelCast::FixOutOfBounds( math::Bitmap const& collisionMap, Coord const& pos )
{
	if( HasCollision( collisionMap, pos ) == false )
	{
		// Already safe
		return pos;
	}

	// Naive scanline approach, wildly unsafe
	RFLOG_WARNING( nullptr, RFCAT_GAMEPIXELPHYSICS, "Trying naive out-of-bounds fix" );
	CoordElem const width = math::integer_cast<CoordElem>( collisionMap.GetWidth() );
	CoordElem const height = math::integer_cast<CoordElem>( collisionMap.GetHeight() );
	for( CoordElem y = 0; y < height; y++ )
	{
		for( CoordElem x = 0; x < width; x++ )
		{
			Coord const testPos{ x, y };
			if( HasCollision( collisionMap, testPos ) == false )
			{
				return testPos;
			}
		}
	}

	// Failed
	return pos;
}




Coord PixelCast::CardinalStepCast(
	math::Bitmap const& collisionMap,
	Coord const& pos,
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



Coord PixelCast::SlideStepCast(
	math::Bitmap const& collisionMap,
	Coord const& pos,
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

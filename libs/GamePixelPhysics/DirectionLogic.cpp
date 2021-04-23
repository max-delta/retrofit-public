#include "stdafx.h"
#include "DirectionLogic.h"

#include "rftl/extension/algorithms.h"
#include "rftl/extension/static_vector.h"


namespace RF::phys {
///////////////////////////////////////////////////////////////////////////////
namespace details {

using Cardinals = rftl::static_vector<Direction::Value, 4>;



Direction::Value GetConflictOf( Direction::Value cardinal )
{
	using namespace Direction;
	RF_ASSERT( rftl::contains( kCardinals, cardinal ) );
	if( cardinal == North )
	{
		return South;
	}
	if( cardinal == South )
	{
		return North;
	}
	if( cardinal == East )
	{
		return West;
	}
	if( cardinal == West )
	{
		return East;
	}

	RF_DBGFAIL();
	return Invalid;
}



// Index 0 is primary
Cardinals DecomposeStack(
	Direction::Value primary,
	Direction::Value secondary,
	Direction::Value latent )
{
	using namespace Direction;

	Cardinals retVal;

	auto const addIfUnique = [&retVal]( Value dir ) -> void //
	{
		if( rftl::contains( retVal, dir ) == false )
		{
			retVal.emplace_back( dir );
		}
	};

	Value const stack[3] = { primary, secondary, latent };
	for( Value const& layer : stack )
	{
		for( Value const& cardinal : kCardinals )
		{
			bool const set = ( layer & cardinal ) != 0;
			if( set )
			{
				addIfUnique( cardinal );
			}
		}
	}

	return retVal;
}



void ConvertStack(
	Direction::Value& primary,
	Direction::Value& secondary,
	Direction::Value& latent,
	Cardinals&& stack )
{
	using namespace Direction;

	primary = Invalid;
	secondary = Invalid;
	latent = Invalid;

	if( stack.empty() )
	{
		return;
	}

	// Put first direction as primary
	primary = stack.front();
	stack.erase( stack.begin() );
	RF_ASSERT( primary != Invalid );
	RF_ASSERT( rftl::contains( kCardinals, primary ) );

	for( Value const& dir : stack )
	{
		RF_ASSERT( dir != Invalid );
		RF_ASSERT( dir != primary );
		RF_ASSERT( rftl::contains( kCardinals, dir ) );
		if( secondary != Invalid || DirectionLogic::HasConflicts( dir | primary ) )
		{
			latent |= dir;
		}
		else
		{
			RF_ASSERT( secondary == Invalid );
			bool const createsInterCardinal = rftl::contains( kInterCardinals, primary | dir );
			if( createsInterCardinal )
			{
				secondary = dir;
			}
			else
			{
				latent |= dir;
			}
		}
	}
}

}
///////////////////////////////////////////////////////////////////////////////

bool DirectionLogic::HasConflicts( Direction::Value dir )
{
	using namespace Direction;

	for( Value const& conflict : kConflicts )
	{
		bool const hasConflict = ( dir & conflict ) == conflict;
		if( hasConflict )
		{
			return true;
		}
	}
	return false;
}



void DirectionLogic::CleanStack(
	Direction::Value& primary,
	Direction::Value& secondary,
	Direction::Value& latent )
{
	// Decompose and convert back
	details::Cardinals stack = details::DecomposeStack( primary, secondary, latent );
	details::ConvertStack( primary, secondary, latent, rftl::move( stack ) );
}



void DirectionLogic::UpdateStack(
	Direction::Value& primary,
	Direction::Value& secondary,
	Direction::Value& latent,
	Direction::Value adds,
	Direction::Value removes )
{
	using namespace Direction;

	// Decompose for easy adds/removes
	details::Cardinals stack = details::DecomposeStack( primary, secondary, latent );

	// Remove from the adds
	adds &= ~removes;

	// Remove from the stack
	for( Value const& cardinal : kCardinals )
	{
		bool const shouldRemove = ( removes & cardinal ) != 0;
		if( shouldRemove )
		{
			rftl::erase_all( stack, cardinal );
		}
	}

	static constexpr auto const performAdd = []( details::Cardinals& stack, Value dir ) -> void //
	{
		using Iter = details::Cardinals::iterator;
		Iter const same = rftl::find( stack.begin(), stack.end(), dir );
		Iter const conflict = rftl::find( stack.begin(), stack.end(), details::GetConflictOf( dir ) );

		if( same == stack.end() )
		{
			// Direction doesn't exist

			if( conflict == stack.end() )
			{
				// Neither exist
				// Add to end
				stack.emplace_back( dir );
			}
			else
			{
				// Only conflict exists
				// Insert before conflict
				stack.insert( conflict, dir );
			}
		}
		else
		{
			// Direction exists

			if( conflict == stack.end() )
			{
				// Only direction exists

				if( same == stack.begin() )
				{
					// Is most important
					// Leave as-is
				}
				else
				{
					// Is not most important
					// Add to end
					stack.emplace_back( dir );
				}
			}
			else
			{
				// Both exist

				if( same < conflict )
				{
					// Direction beats conflict
					// Leave as-is
				}
				else
				{
					// Direction is beaten by conflict
					// Swap them
					rftl::swap( *same, *conflict );
				}
			}
		}
	};

	// Process adds
	for( Value const& cardinal : kCardinals )
	{
		bool const shouldAdd = ( adds & cardinal ) != 0;
		if( shouldAdd )
		{
			performAdd( stack, cardinal );
		}
	}

	// Convert back
	details::ConvertStack( primary, secondary, latent, rftl::move( stack ) );
}

///////////////////////////////////////////////////////////////////////////////
}

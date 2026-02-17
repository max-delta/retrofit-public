#include "stdafx.h"
#include "MergeController.h"

#include "GameInput/RawController.h"

#include "Logging/Logging.h"

#include "rftl/extension/algorithms.h"
#include "rftl/extension/stretch_vector.h"
#include "rftl/deque"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

MergeController::MergeController()
	: GameController( "Merge" )
{
	//
}



MergeController::Sources MergeController::GetSources() const
{
	return mSources;
}



void MergeController::AddSource( Source const& source )
{
	RF_ASSERT( source != nullptr );
	mSources.emplace_back( source );
}



void MergeController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	using Buffer = rftl::stretch_vector<GameCommand, 4>;
	using Buffers = rftl::stretch_vector<Buffer, 6>;
	Buffers tempBuffers;
	tempBuffers.reserve( mSources.size() );

	// For each source...
	for( Source const& source : mSources )
	{
		if( source == nullptr )
		{
			RF_DBGFAIL();
			continue;
		}

		// Store the commands
		Buffer& tempBuffer = tempBuffers.emplace_back();
		rftl::virtual_back_inserter_iterator<GameCommand, Buffer> adder( tempBuffer );
		source->GetGameCommandStream( adder, maxCommands );
	}

	// Will move read heads along parallel buffers
	using ReadHead = Buffer::const_iterator;
	using ReadHeads = rftl::stretch_vector<ReadHead, 16>;
	ReadHeads readHeads = {};
	readHeads.resize( tempBuffers.size() );
	for( size_t i = 0; i < tempBuffers.size(); i++ )
	{
		readHeads.at( i ) = tempBuffers.at( i ).begin();
	}

	// Merge buffers until empty
	Buffer outgoing = {};
	while( true )
	{
		// Will try to get the earliest buffer each loop
		ReadHead* earliestReadHead = nullptr;

		// For each buffer...
		for( size_t i = 0; i < tempBuffers.size(); i++ )
		{
			Buffer const& speculativeBuffer = tempBuffers.at( i );
			ReadHead& speculativeReadHead = readHeads.at( i );
			RF_ASSERT( speculativeReadHead >= speculativeBuffer.begin() );
			RF_ASSERT( speculativeReadHead <= speculativeBuffer.end() );
			if( speculativeReadHead >= speculativeBuffer.end() )
			{
				// Empty buffer, skip
				continue;
			}

			if( earliestReadHead == nullptr )
			{
				// No current read head, use this one
				earliestReadHead = &speculativeReadHead;
				continue;
			}

			GameCommand const& speculativeCommand = *speculativeReadHead;
			GameCommand const& earliestCommand = **earliestReadHead;
			if( speculativeCommand.mTime >= earliestCommand.mTime )
			{
				// Speculative command is better candidate than current
				earliestReadHead = &speculativeReadHead;
				continue;
			}

			// Current candidate is best
			continue;
		}

		if( earliestReadHead == nullptr )
		{
			// Out of commands, done with buffer
			break;
		}

		// Consume chosen command
		GameCommand const& chosenCommand = **earliestReadHead;
		( *earliestReadHead )++;

		// Store
		if( outgoing.size() == maxCommands )
		{
			RFLOG_WARNING( nullptr, RFCAT_GAMEINPUT, "Buffer overflow, discarding some commands" );
			rftl::pop_front_via_erase( outgoing );
		}
		RF_ASSERT( outgoing.size() < maxCommands );
		outgoing.emplace_back( chosenCommand );
	}

	for( GameCommand const& command : outgoing )
	{
		parser( command );
	}
}



void MergeController::TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	// For each source...
	for( Source const& source : mSources )
	{
		if( source == nullptr )
		{
			RF_DBGFAIL();
			continue;
		}

		source->TruncateBuffers( earliestTime, latestTime );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "HotkeyController.h"

#include "GameInput/RawController.h"

#include "rftl/deque"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

WeakPtr<RawController> HotkeyController::GetSource() const
{
	return mSource;
}



void HotkeyController::SetSource( WeakPtr<RawController> const& source )
{
	mSource = source;
}



void HotkeyController::SetCommandMapping( CommandMapping const& mapping )
{
	mCommandMapping = mapping;
}



void HotkeyController::GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const
{
	rftl::deque<GameCommand> tempBuffer;

	auto const onElement = [this, &tempBuffer, maxCommands]( RawCommand const& element ) -> void {
		CommandMapping::const_iterator const mapping = mCommandMapping.find( element.mType );
		if( mapping != mCommandMapping.end() )
		{
			{
				if( tempBuffer.size() == maxCommands )
				{
					// TODO: Warn on buffer overflow
					tempBuffer.pop_front();
				}
				RF_ASSERT( tempBuffer.size() < maxCommands );
				tempBuffer.emplace_back( GameCommand{ mapping->second, element.mTime } );
			}
		}
	};
	rftl::virtual_callable_iterator<RawCommand, decltype( onElement )> converter( onElement );
	mSource->GetRawCommandStream( converter );

	for( GameCommand const& command : tempBuffer )
	{
		parser( command );
	}
}



void HotkeyController::TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	mSource->TruncateBuffers( earliestTime, latestTime );
}

///////////////////////////////////////////////////////////////////////////////
}}

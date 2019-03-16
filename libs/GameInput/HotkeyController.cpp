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



void HotkeyController::GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const
{
	rftl::deque<Command> tempBuffer;

	auto const onElement = [this, &tempBuffer, maxCommands]( RawController::Command const& element ) -> void
	{
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
				tempBuffer.emplace_back( Command{ mapping->second, element.mTime } );
			}
		}
	};
	rftl::virtual_callable_iterator<RawController::Command, decltype( onElement )> converter( onElement );
	mSource->GetRawCommandStream( converter );

	for( Command const& command : tempBuffer )
	{
		parser( command );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}

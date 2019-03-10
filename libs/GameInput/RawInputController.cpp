#include "stdafx.h"
#include "RawInputController.h"

#include "core_math/math_clamps.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void RawInputController::SetPhysicalMapping( PhysicalMapping const& mapping )
{
	mPhysicalMapping = mapping;
}



void RawInputController::SetLogicalMapping( LogicalMapping const& mapping )
{
	mLogicalMapping = mapping;
}



void RawInputController::SetSignalMapping( SignalMapping const& mapping )
{
	mSignalMapping = mapping;
}



void RawInputController::ConsumeInput( InputDevice& inputDevice )
{
	// TODO
}



void RawInputController::GetRawCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const
{
	size_t numEmitted = 0;
	for( Command const& command : mCommandBuffer )
	{
		if( numEmitted >= maxCommands )
		{
			break;
		}
		parser( command );
		numEmitted++;
	}
}



void RawInputController::GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const
{
	for( SignalMapping::value_type const& mapping : mSignalMapping )
	{
		iter( mapping.second );
	}
}



void RawInputController::GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, RawSignalType type ) const
{
	SignalBufferMap::const_iterator const iter = mSignalBufferMap.find( type );
	if( iter == mSignalBufferMap.end() )
	{
		return;
	}

	SignalBuffer const& signalBuffer = iter->second;
	size_t numEmitted = 0;
	for( Signal const& signal : signalBuffer )
	{
		if( numEmitted >= maxSamples )
		{
			break;
		}
		sampler( signal );
		numEmitted++;
	}
}



void RawInputController::GetTextStream( rftl::u16string& text, size_t maxLen ) const
{
	text.clear();
	text.reserve( math::Min( maxLen, mTextBuffer.size() ) );
	for( char16_t const& ch : mTextBuffer )
	{
		text.push_back( ch );
		if( text.size() == maxLen )
		{
			break;
		}
	}
}



void RawInputController::ClearTextStream()
{
	mTextBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////
}}

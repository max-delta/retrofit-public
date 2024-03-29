#include "stdafx.h"
#include "RawInputController.h"

#include "Logging/Logging.h"

#include "core_input/InputDevice.h"
#include "core_math/math_clamps.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static rftl::string CreateIdentifier( rftl::string_view subIdentifier )
{
	RF_ASSERT( subIdentifier.empty() == false );
	return rftl::string( "RawInput<" ).append( subIdentifier ).append( ">" );
}


}
///////////////////////////////////////////////////////////////////////////////

RawInputController::RawInputController( rftl::string_view subIdentifier )
	: RawController( details::CreateIdentifier( subIdentifier ) )
{
	//
}



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
	DigitalInputComponent* const digital = inputDevice.mDigitalComponent;
	AnalogInputComponent* const analog = inputDevice.mAnalogComponent;
	TextInputComponent* const text = inputDevice.mTextComponent;

	if( digital != nullptr )
	{
		// Physical
		{
			auto const onElement = [this]( DigitalInputComponent::PhysicalEvent const& element ) -> void {
				PhysicalMapping::const_iterator const key = mPhysicalMapping.find( element.mCode );
				if( key != mPhysicalMapping.end() )
				{
					PhysicalMapping::mapped_type::const_iterator const state = key->second.find( element.mNewState );
					if( state != key->second.end() )
					{
						if( mCommandBuffer.size() == kMaxCommandBufferLength )
						{
							// TODO: Warn on buffer overflow
							mCommandBuffer.pop_front();
						}
						RF_ASSERT( mCommandBuffer.size() < kMaxCommandBufferLength );
						mCommandBuffer.emplace_back( RawCommand{ state->second, element.mTime } );
					}
				}
			};
			rftl::virtual_callable_iterator<DigitalInputComponent::PhysicalEvent, decltype( onElement )> converter( onElement );
			digital->GetPhysicalEventStream( converter );
			digital->ClearPhysicalEventStream();
		}

		// Logical
		{
			auto const onElement = [this]( DigitalInputComponent::LogicalEvent const& element ) -> void {
				LogicalMapping::const_iterator const key = mLogicalMapping.find( element.mCode );
				if( key != mLogicalMapping.end() )
				{
					LogicalMapping::mapped_type::const_iterator const state = key->second.find( element.mNewState );
					if( state != key->second.end() )
					{
						if( mCommandBuffer.size() == kMaxCommandBufferLength )
						{
							// TODO: Warn on buffer overflow
							mCommandBuffer.pop_front();
						}
						RF_ASSERT( mCommandBuffer.size() < kMaxCommandBufferLength );
						mCommandBuffer.emplace_back( RawCommand{ state->second, element.mTime } );
					}
				}
			};
			rftl::virtual_callable_iterator<DigitalInputComponent::LogicalEvent, decltype( onElement )> converter( onElement );
			digital->GetLogicalEventStream( converter );
			digital->ClearLogicalEventStream();
		}
	}

	if( analog != nullptr )
	{
		time::CommonClock::time_point const now = time::FrameClock::now();
		for( SignalMapping::value_type const& mapping : mSignalMapping )
		{
			AnalogSignalValue const value = analog->GetCurrentSignalValue( mapping.first );
			SignalBuffer& buffer = mSignalBufferMap[mapping.second];
			if( buffer.size() == kMaxSignalBufferLength )
			{
				buffer.pop_front();
			}
			RF_ASSERT( buffer.size() < kMaxSignalBufferLength );
			buffer.emplace_back( RawSignal{ value, now } );
		}
	}

	if( text != nullptr )
	{
		rftl::u16string temp;
		text->GetTextStream( temp );
		if( temp.size() > kMaxTextBufferLength )
		{
			RFLOG_WARNING( nullptr, RFCAT_GAMEINPUT, "Buffer overflow, discarding some text characters" );
		}
		for( char16_t ch : temp )
		{
			if( mTextBuffer.size() == kMaxTextBufferLength )
			{
				mTextBuffer.pop_front();
			}
			RF_ASSERT( mTextBuffer.size() < kMaxTextBufferLength );
			mTextBuffer.emplace_back( ch );
		}
		text->ClearTextStream();
	}
}



void RawInputController::GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser, size_t maxCommands ) const
{
	size_t const numToRead = math::Min( mCommandBuffer.size(), maxCommands );
	for( size_t i = mCommandBuffer.size() - numToRead; i < mCommandBuffer.size(); i++ )
	{
		parser( mCommandBuffer[i] );
	}
}



void RawInputController::GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter, size_t maxTypes ) const
{
	for( SignalMapping::value_type const& mapping : mSignalMapping )
	{
		iter( mapping.second );
	}
}



void RawInputController::GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, size_t maxSamples, RawSignalType type ) const
{
	SignalBufferMap::const_iterator const iter = mSignalBufferMap.find( type );
	if( iter == mSignalBufferMap.end() )
	{
		return;
	}

	SignalBuffer const& signalBuffer = iter->second;
	size_t const numToRead = math::Min( signalBuffer.size(), maxSamples );
	for( size_t i = signalBuffer.size() - numToRead; i < signalBuffer.size(); i++ )
	{
		sampler( signalBuffer[i] );
	}
}



void RawInputController::GetTextStream( rftl::u16string& text, size_t maxLen ) const
{
	text.clear();
	size_t const numToRead = math::Min( mTextBuffer.size(), maxLen );
	text.reserve( numToRead );
	for( size_t i = mTextBuffer.size() - numToRead; i < mTextBuffer.size(); i++ )
	{
		text.push_back( mTextBuffer[i] );
	}
}



void RawInputController::ClearTextStream()
{
	mTextBuffer.clear();
}



void RawInputController::TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime )
{
	while( mCommandBuffer.empty() == false && mCommandBuffer.front().mTime < earliestTime )
	{
		mCommandBuffer.pop_front();
	}
	while( mCommandBuffer.empty() == false && mCommandBuffer.back().mTime > latestTime )
	{
		mCommandBuffer.pop_back();
	}

	for( SignalBufferMap::value_type& signal : mSignalBufferMap )
	{
		SignalBuffer& buffer = signal.second;
		while( buffer.empty() == false && buffer.front().mTime < earliestTime )
		{
			buffer.pop_front();
		}
		while( buffer.empty() == false && buffer.back().mTime > latestTime )
		{
			buffer.pop_back();
		}
	}

	// TODO: Text, when the interface supports time-per-char
}

///////////////////////////////////////////////////////////////////////////////
}

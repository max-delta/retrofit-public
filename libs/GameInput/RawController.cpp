#include "stdafx.h"
#include "RawController.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void RawController::GetRawCommandStream( rftl::virtual_iterator<Command>& parser ) const
{
	return GetRawCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawCommandStream( rftl::virtual_iterator<Command>& parser, time::FrameClock::time_point earliestTime ) const
{
	auto const onElement = [&parser, &earliestTime]( RawController::Command const& element ) -> void
	{
		if( element.mTime >= earliestTime )
		{
			parser( element );
		}
	};
	rftl::virtual_callable_iterator<RawController::Command, decltype( onElement )> filter( onElement );
	GetRawCommandStream( filter );
}



void RawController::GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter ) const
{
	return GetKnownSignals( iter, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, RawSignalType type ) const
{
	return GetRawSignalStream( sampler, rftl::numeric_limits<size_t>::max(), type );
}



void RawController::GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, time::FrameClock::time_point earliestTime, RawSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime]( RawController::Signal const& element ) -> void
	{
		if( element.mTime >= earliestTime )
		{
			sampler( element );
		}
	};
	rftl::virtual_callable_iterator<RawController::Signal, decltype( onElement )> filter( onElement );
	GetRawSignalStream( filter, type );
}



void RawController::GetTextStream( rftl::u16string& text ) const
{
	return GetTextStream( text, rftl::numeric_limits<size_t>::max() );
}

///////////////////////////////////////////////////////////////////////////////
}}

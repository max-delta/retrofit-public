#include "stdafx.h"
#include "RawController.h"

#include "rftl/limits"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

void RawController::GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser ) const
{
	return GetRawCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) const
{
	auto const onElement = [&parser, &earliestTime, &latestTime]( RawCommand const& element ) -> void {
		if( element.mTime >= earliestTime && element.mTime <= latestTime )
		{
			parser( element );
		}
	};
	rftl::virtual_callable_iterator<RawCommand, decltype( onElement )> filter( onElement );
	GetRawCommandStream( filter );
}



void RawController::GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter ) const
{
	return GetKnownSignals( iter, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, RawSignalType type ) const
{
	return GetRawSignalStream( sampler, rftl::numeric_limits<size_t>::max(), type );
}



void RawController::GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime, RawSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime, &latestTime]( RawSignal const& element ) -> void {
		if( element.mTime >= earliestTime && element.mTime <= latestTime )
		{
			sampler( element );
		}
	};
	rftl::virtual_callable_iterator<RawSignal, decltype( onElement )> filter( onElement );
	GetRawSignalStream( filter, type );
}



void RawController::GetTextStream( rftl::u16string& text ) const
{
	return GetTextStream( text, rftl::numeric_limits<size_t>::max() );
}

///////////////////////////////////////////////////////////////////////////////
}

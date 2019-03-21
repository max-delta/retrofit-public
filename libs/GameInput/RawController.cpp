#include "stdafx.h"
#include "RawController.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void RawController::GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser ) const
{
	return GetRawCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawCommandStream( rftl::virtual_iterator<RawCommand>& parser, time::FrameClock::time_point earliestTime ) const
{
	auto const onElement = [&parser, &earliestTime]( RawCommand const& element ) -> void
	{
		if( element.mTime >= earliestTime )
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



void RawController::GetRawSignalStream( rftl::virtual_iterator<RawSignal>& sampler, time::FrameClock::time_point earliestTime, RawSignalType type ) const
{
	auto const onElement = [&sampler, &earliestTime]( RawSignal const& element ) -> void
	{
		if( element.mTime >= earliestTime )
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
}}

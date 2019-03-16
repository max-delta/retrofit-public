#include "stdafx.h"
#include "RawController.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

void RawController::GetRawCommandStream( rftl::virtual_iterator<Command>& parser ) const
{
	return GetRawCommandStream( parser, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetKnownSignals( rftl::virtual_iterator<RawSignalType>& iter ) const
{
	return GetKnownSignals( iter, rftl::numeric_limits<size_t>::max() );
}



void RawController::GetRawSignalStream( rftl::virtual_iterator<Signal>& sampler, RawSignalType type ) const
{
	return GetRawSignalStream( sampler, rftl::numeric_limits<size_t>::max(), type );
}



void RawController::GetTextStream( rftl::u16string& text ) const
{
	return GetTextStream( text, rftl::numeric_limits<size_t>::max() );
}

///////////////////////////////////////////////////////////////////////////////
}}

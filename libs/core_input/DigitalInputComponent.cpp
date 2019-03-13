#include "stdafx.h"
#include "DigitalInputComponent.h"

#include "rftl/limits"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

bool DigitalInputComponent::WasActivatedPhysical( PhysicalCode code ) const
{
	return GetPreviousPhysicalState( code ) == DigitalPinState::Inactive &&
		GetCurrentPhysicalState( code ) == DigitalPinState::Active;
}



bool DigitalInputComponent::WasDeactivatedPhysical( PhysicalCode code ) const
{
	return GetPreviousPhysicalState( code ) == DigitalPinState::Active &&
		GetCurrentPhysicalState( code ) == DigitalPinState::Inactive;
}



bool DigitalInputComponent::WasActivatedLogical( LogicalCode code ) const
{
	return GetPreviousLogicalState( code ) == DigitalPinState::Inactive &&
		GetCurrentLogicalState( code ) == DigitalPinState::Active;
}



bool DigitalInputComponent::WasDeactivatedLogical( LogicalCode code ) const
{
	return GetPreviousLogicalState( code ) == DigitalPinState::Active &&
		GetCurrentLogicalState( code ) == DigitalPinState::Inactive;
}



void DigitalInputComponent::GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser ) const
{
	return GetPhysicalEventStream( parser, rftl::numeric_limits<size_t>::max() );
}



void DigitalInputComponent::GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser ) const
{
	return GetLogicalEventStream( parser, rftl::numeric_limits<size_t>::max() );
}

///////////////////////////////////////////////////////////////////////////////
}}

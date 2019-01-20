#include "stdafx.h"
#include "DigitalInputComponent.h"

namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

bool DigitalInputComponent::WasActivatedPhysical( PhysicalCode code ) const
{
	return GetPreviousPhysicalState( code ) == PinState::Inactive &&
		GetCurrentPhysicalState( code ) == PinState::Active;
}



bool DigitalInputComponent::WasDeactivatedPhysical( PhysicalCode code ) const
{
	return GetPreviousPhysicalState( code ) == PinState::Active &&
		GetCurrentPhysicalState( code ) == PinState::Inactive;
}



bool DigitalInputComponent::WasActivatedLogical( LogicalCode code ) const
{
	return GetPreviousLogicalState( code ) == PinState::Inactive &&
		GetCurrentLogicalState( code ) == PinState::Active;
}



bool DigitalInputComponent::WasDeactivatedLogical( LogicalCode code ) const
{
	return GetPreviousLogicalState( code ) == PinState::Active &&
		GetCurrentLogicalState( code ) == PinState::Inactive;
}

///////////////////////////////////////////////////////////////////////////////
}}

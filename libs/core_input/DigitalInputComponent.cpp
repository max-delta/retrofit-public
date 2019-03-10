#include "stdafx.h"
#include "DigitalInputComponent.h"

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

///////////////////////////////////////////////////////////////////////////////
}}

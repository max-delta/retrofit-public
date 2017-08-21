#pragma once
#include <stdint.h>
#include <iterator>

namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class InputComponent
{
	virtual void OnTick() = 0;
};


class DigitalInputComponent : public InputComponent
{
	typedef uint8_t PhysicalCode;
	typedef uint8_t LogicalCode;
	enum PinState : bool
	{
		Active = true,
		Inactive = false
	};
	virtual void OnTick() override = 0;
	virtual PinState GetCurrentPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetPreviousPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetCurrentLogicalState( LogicalCode code ) const = 0;
	virtual PinState GetPreviousLogicalState( LogicalCode code ) const = 0;

	template<typename Container>
	virtual void GetPhysicalEventStream( std::back_insert_iterator<Container> inserter, size_t maxEvents ) const = 0;

	#error WIP, still just sketching out ideas
};

///////////////////////////////////////////////////////////////////////////////
}}

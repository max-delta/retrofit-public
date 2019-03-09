#pragma once
#include "core_input/InputComponent.h"

#include "Timing/clocks.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class DigitalInputComponent : public InputComponent
{
	RF_NO_COPY( DigitalInputComponent );

public:
	typedef uint8_t PhysicalCode;
	typedef uint8_t LogicalCode;

	enum PinState : bool
	{
		Active = true,
		Inactive = false
	};
	DigitalInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Digital;
	}

	virtual PhysicalCode GetMaxPhysicalCode() const = 0;
	virtual LogicalCode GetMaxLogicalCode() const = 0;

	virtual rftl::u16string GetLogicalName( LogicalCode code ) const = 0;

	virtual PinState GetCurrentPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetPreviousPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetCurrentLogicalState( LogicalCode code ) const = 0;
	virtual PinState GetPreviousLogicalState( LogicalCode code ) const = 0;
	bool WasActivatedPhysical( PhysicalCode code ) const;
	bool WasDeactivatedPhysical( PhysicalCode code ) const;
	bool WasActivatedLogical( LogicalCode code ) const;
	bool WasDeactivatedLogical( LogicalCode code ) const;

	struct PhysicalEvent
	{
		PhysicalEvent( PhysicalCode code, PinState state )
			: mCode( code )
			, mNewState( state )
			, mTime( time::FrameClock::now() )
		{
		}
		PhysicalCode mCode;
		PinState mNewState;
		time::FrameClock::time_point mTime;
	};
	struct LogicalEvent
	{
		LogicalEvent( LogicalCode code, PinState state )
			: mCode( code )
			, mNewState( state )
			, mTime( time::FrameClock::now() )
		{
		}
		LogicalCode mCode;
		PinState mNewState;
		time::FrameClock::time_point mTime;
	};

	virtual void GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const = 0;
	virtual void GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const = 0;

	virtual void ClearPhysicalEventStream() = 0;
	virtual void ClearLogicalEventStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

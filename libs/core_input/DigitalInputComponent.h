#pragma once
#include "core_input/InputComponent.h"

#include "Timing/FrameClock.h"

#include "rftl/extension/virtual_iterator.h"
#include "rftl/string"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class DigitalInputComponent : public InputComponent
{
	RF_NO_COPY( DigitalInputComponent );

public:
	DigitalInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual InputComponentType GetType() override final
	{
		return InputComponentType::Digital;
	}

	virtual PhysicalCode GetMaxPhysicalCode() const = 0;
	virtual LogicalCode GetMaxLogicalCode() const = 0;

	virtual rftl::u16string GetLogicalName( LogicalCode code ) const = 0;

	virtual DigitalPinState GetCurrentPhysicalState( PhysicalCode code ) const = 0;
	virtual DigitalPinState GetPreviousPhysicalState( PhysicalCode code ) const = 0;
	virtual DigitalPinState GetCurrentLogicalState( LogicalCode code ) const = 0;
	virtual DigitalPinState GetPreviousLogicalState( LogicalCode code ) const = 0;
	bool WasActivatedPhysical( PhysicalCode code ) const;
	bool WasDeactivatedPhysical( PhysicalCode code ) const;
	bool WasActivatedLogical( LogicalCode code ) const;
	bool WasDeactivatedLogical( LogicalCode code ) const;

	struct PhysicalEvent
	{
		PhysicalEvent( PhysicalCode code, DigitalPinState state )
			: mCode( code )
			, mNewState( state )
			, mTime( time::FrameClock::now() )
		{
		}
		PhysicalCode mCode;
		DigitalPinState mNewState;
		time::CommonClock::time_point mTime;
	};
	struct LogicalEvent
	{
		LogicalEvent( LogicalCode code, DigitalPinState state )
			: mCode( code )
			, mNewState( state )
			, mTime( time::FrameClock::now() )
		{
		}
		LogicalCode mCode;
		DigitalPinState mNewState;
		time::CommonClock::time_point mTime;
	};

	void GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser ) const;
	virtual void GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const = 0;
	void GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser ) const;
	virtual void GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const = 0;

	virtual void ClearPhysicalEventStream() = 0;
	virtual void ClearLogicalEventStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "core_input/InputComponent.h"

#include <iterator>


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

template<typename EventType>
struct EventParser
{
	virtual void OnEvent( EventType const& event ) = 0;
};

template<typename EventType, typename Container>
struct BufferCopyEventParser : public EventParser<EventType>
{
	BufferCopyEventParser( Container& containerRef )
		: m_Inserter( containerRef )
	{
		//
	}

	virtual void OnEvent( EventType const& event )
	{
		m_Inserter = event;
	}

	std::back_insert_iterator<Container> m_Inserter;
};



class DigitalInputComponent : public InputComponent
{
public:
	typedef uint8_t PhysicalCode;
	typedef uint8_t LogicalCode;

	enum PinState : bool
	{
		Active = true,
		Inactive = false
	};
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Digital;
	}

	virtual PhysicalCode GetMaxPhysicalCode() const = 0;
	virtual LogicalCode GetMaxLogicalCode() const = 0;

	virtual std::u16string GetLogicalName( LogicalCode code ) const = 0;

	virtual PinState GetCurrentPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetPreviousPhysicalState( PhysicalCode code ) const = 0;
	virtual PinState GetCurrentLogicalState( LogicalCode code ) const = 0;
	virtual PinState GetPreviousLogicalState( LogicalCode code ) const = 0;

	struct PhysicalEvent
	{
		PhysicalEvent( PhysicalCode code, PinState state )
			: m_Code( code )
			, m_NewState( state )
		{
		}
		PhysicalCode m_Code;
		PinState m_NewState;
	};
	struct LogicalEvent
	{
		LogicalEvent( LogicalCode code, PinState state )
			: m_Code( code )
			, m_NewState( state )
		{
		}
		LogicalCode m_Code;
		PinState m_NewState;
	};

	virtual void GetPhysicalEventStream( EventParser<PhysicalEvent>& parser, size_t maxEvents ) const = 0;
	virtual void GetLogicalEventStream( EventParser<LogicalEvent>& parser, size_t maxEvents ) const = 0;

	virtual void ClearPhysicalEventStream() = 0;
	virtual void ClearLogicalEventStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

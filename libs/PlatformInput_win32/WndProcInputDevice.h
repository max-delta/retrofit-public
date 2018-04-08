#pragma once
#include "project.h"

#include "core_input/InputDevice.h"

#include "core_platform/win_shim.h"

#include "rftl/limits"
#include "rftl/deque"
#include "rftl/bitset"

class PLATFORMINPUT_API RF::input::InputDevice;
class PLATFORMINPUT_API RF::input::DigitalInputComponent;
class PLATFORMINPUT_API RF::input::AnalogInputComponent;
class PLATFORMINPUT_API RF::input::TextInputComponent;


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class WndProcDigitalInputComponent;
class WndProcAnalogInputComponent;
class WndProcTextInputComponent;

// Easiest, best behavior. If you don't use for text entry and cursor
//  control, it's probably because you're an idiot...
// NOTE: Raw input for mouse movement will be flipped on and processed as
//  seperate relative analog input in here. Responsible developers will
//  avoid using this unless they've got a reasonable case where they want
//  to break support for most user input devices (for example, if you are
//  an FPS that really only works with relative positioning devices, and
//  also have a community that demands sub-pixel DPI motion)
class PLATFORMINPUT_API WndProcInputDevice : public InputDevice
{
	RF_NO_COPY( WndProcInputDevice );

public:
	WndProcInputDevice();
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );


public:
	WndProcDigitalInputComponent& m_Digital;
	WndProcAnalogInputComponent& m_Analog;
	WndProcTextInputComponent& m_Text;
};



class PLATFORMINPUT_API WndProcDigitalInputComponent final : public DigitalInputComponent
{
	RF_NO_COPY( WndProcDigitalInputComponent );

public:
	friend class WndProcInputDevice;
private:
	static constexpr size_t k_NumVKeys = rftl::numeric_limits<unsigned char>::max();
	static constexpr size_t k_NumScanCodes = 1 << 8; // ScanCodes are 8 bits
	static constexpr size_t k_MaxEventStorage = 32;
private:
	typedef rftl::bitset<k_NumVKeys> LogicalCodeStates;
	typedef rftl::bitset<k_NumScanCodes> PhysicalCodeStates;
	typedef rftl::deque<LogicalEvent> LogicalEventBuffer;
	typedef rftl::deque<PhysicalEvent> PhysicalEventBuffer;
public:
	WndProcDigitalInputComponent() = default;
	virtual void OnTick() override;
	virtual PhysicalCode GetMaxPhysicalCode() const override;
	virtual LogicalCode GetMaxLogicalCode() const override;
	virtual rftl::u16string GetLogicalName( LogicalCode code ) const override;
	virtual PinState GetCurrentPhysicalState( PhysicalCode code ) const override;
	virtual PinState GetPreviousPhysicalState( PhysicalCode code ) const override;
	virtual PinState GetCurrentLogicalState( LogicalCode code ) const override;
	virtual PinState GetPreviousLogicalState( LogicalCode code ) const override;
	virtual void GetPhysicalEventStream( EventParser<PhysicalEvent>& parser, size_t maxEvents ) const override;
	virtual void GetLogicalEventStream( EventParser<LogicalEvent>& parser, size_t maxEvents ) const override;
	virtual void ClearPhysicalEventStream() override;
	virtual void ClearLogicalEventStream() override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
	void RecordLogical( uint8_t code, PinState state );
	void RecordPhysical( uint8_t code, PinState state );
private:
	LogicalCodeStates m_CurrentLogicalState;
	LogicalCodeStates m_PreviousLogicalState;
	LogicalEventBuffer m_LogicalEventBuffer;

	PhysicalCodeStates m_CurrentPhysicalState;
	PhysicalCodeStates m_PreviousPhysicalState;
	PhysicalEventBuffer m_PhysicalEventBuffer;
};



class PLATFORMINPUT_API WndProcAnalogInputComponent final : public AnalogInputComponent
{
	RF_NO_COPY( WndProcAnalogInputComponent );

public:
	friend class WndProcInputDevice;
public:
	enum : SignalIndex
	{
		k_CursorAbsoluteX = 0,
		k_CursorAbsoluteY,

		k_NumSignals
	};
private:
	typedef SignalValue SignalValues[k_NumSignals];
public:
	WndProcAnalogInputComponent() = default;
	virtual void OnTick() override;
	virtual SignalIndex GetMaxSignalIndex() const override;
	virtual rftl::u16string GetSignalName( SignalIndex signalIndex ) const override;
	virtual SignalValue GetCurrentSignalValue( SignalIndex signalIndex ) const override;
	virtual SignalValue GetPreviousSignalValue( SignalIndex signalIndex ) const override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
private:
	SignalValues m_CurrentSignalValues;
	SignalValues m_PreviousSignalValues;
};



class PLATFORMINPUT_API WndProcTextInputComponent final : public TextInputComponent
{
	RF_NO_COPY( WndProcTextInputComponent );

public:
	friend class WndProcInputDevice;
private:
	// Assumption is that when caller cares, they will pull from buffer
	//  every frame, and otherwise just let it spin
	static constexpr size_t k_MaxStorage = 16;
private:
	typedef rftl::deque<char16_t> TextBuffer;
public:
	WndProcTextInputComponent() = default;
	virtual void OnTick() override;
	virtual void GetTextStream( rftl::u16string & text, size_t maxLen ) const override;
	virtual void ClearTextStream() override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
private:
	TextBuffer m_TextBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}}

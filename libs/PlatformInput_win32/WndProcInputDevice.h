#pragma once
#include "project.h"

#include "core_input/InputDevice.h"

#include "core_platform/shim/win_shim.h"

#include "rftl/limits"
#include "rftl/deque"
#include "rftl/bitset"

// Forwards
namespace RF::input {
	class PLATFORMINPUT_API InputDevice;
	class PLATFORMINPUT_API DigitalInputComponent;
	class PLATFORMINPUT_API AnalogInputComponent;
	class PLATFORMINPUT_API TextInputComponent;
}


namespace RF::input {
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
	WndProcDigitalInputComponent& mDigital;
	WndProcAnalogInputComponent& mAnalog;
	WndProcTextInputComponent& mText;
};



class PLATFORMINPUT_API WndProcDigitalInputComponent final : public DigitalInputComponent
{
	RF_NO_COPY( WndProcDigitalInputComponent );

public:
	friend class WndProcInputDevice;
private:
	static constexpr size_t kNumVKeys = rftl::numeric_limits<unsigned char>::max();
	static constexpr size_t kNumScanCodes = 1 << 8; // ScanCodes are 8 bits
	static constexpr size_t kMaxEventStorage = 32;
	static constexpr bool kKillKeysOnFocusLost = true; // TODO: Option?
public:
	// Some inputs do not have VK values, so we have to make them up from
	//  unassigned VK values so they won't conflict
	// See: WinUser.h
	static constexpr uint8_t kVkScrollWheelUp = 0x88;
	static constexpr uint8_t kVkScrollWheelDown = 0x89;
private:
	using LogicalCodeStates = rftl::bitset<kNumVKeys>;
	using PhysicalCodeStates = rftl::bitset<kNumScanCodes>;
	using LogicalEventBuffer = rftl::deque<LogicalEvent>;
	using PhysicalEventBuffer = rftl::deque<PhysicalEvent>;
public:
	WndProcDigitalInputComponent() = default;
	virtual void OnTick() override;
	virtual PhysicalCode GetMaxPhysicalCode() const override;
	virtual LogicalCode GetMaxLogicalCode() const override;
	virtual rftl::u16string GetLogicalName( LogicalCode code ) const override;
	virtual DigitalPinState GetCurrentPhysicalState( PhysicalCode code ) const override;
	virtual DigitalPinState GetPreviousPhysicalState( PhysicalCode code ) const override;
	virtual DigitalPinState GetCurrentLogicalState( LogicalCode code ) const override;
	virtual DigitalPinState GetPreviousLogicalState( LogicalCode code ) const override;
	virtual void GetPhysicalEventStream( rftl::virtual_iterator<PhysicalEvent>& parser, size_t maxEvents ) const override;
	virtual void GetLogicalEventStream( rftl::virtual_iterator<LogicalEvent>& parser, size_t maxEvents ) const override;
	virtual void ClearPhysicalEventStream() override;
	virtual void ClearLogicalEventStream() override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
	void RecordLogical( uint8_t code, DigitalPinState state );
	void RecordPhysical( uint8_t code, DigitalPinState state );
	void RecordFocusLoss();
private:
	LogicalCodeStates mCurrentLogicalState;
	LogicalCodeStates mPreviousLogicalState;
	LogicalEventBuffer mLogicalEventBuffer;

	PhysicalCodeStates mCurrentPhysicalState;
	PhysicalCodeStates mPreviousPhysicalState;
	PhysicalEventBuffer mPhysicalEventBuffer;
};



class PLATFORMINPUT_API WndProcAnalogInputComponent final : public AnalogInputComponent
{
	RF_NO_COPY( WndProcAnalogInputComponent );

public:
	friend class WndProcInputDevice;
public:
	enum : AnalogSignalIndex
	{
		k_CursorAbsoluteX = 0,
		k_CursorAbsoluteY,

		k_NumSignals
	};
private:
	using SignalValues = AnalogSignalValue[k_NumSignals];
public:
	WndProcAnalogInputComponent() = default;
	virtual void OnTick() override;
	virtual AnalogSignalIndex GetMaxSignalIndex() const override;
	virtual rftl::u16string GetSignalName( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetCurrentSignalValue( AnalogSignalIndex signalIndex ) const override;
	virtual AnalogSignalValue GetPreviousSignalValue( AnalogSignalIndex signalIndex ) const override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
private:
	SignalValues mCurrentSignalValues;
	SignalValues mPreviousSignalValues;
};



class PLATFORMINPUT_API WndProcTextInputComponent final : public TextInputComponent
{
	RF_NO_COPY( WndProcTextInputComponent );

public:
	friend class WndProcInputDevice;
private:
	// Assumption is that when caller cares, they will pull from buffer
	//  every frame, and otherwise just let it spin
	static constexpr size_t kMaxStorage = 16;
private:
	using TextBuffer = rftl::deque<char16_t>;
public:
	WndProcTextInputComponent() = default;
	virtual void OnTick() override;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const override;
	virtual void ClearTextStream() override;
private:
	shim::LRESULT ExamineTranslatedMessage( shim::HWND hWnd, shim::UINT message, shim::WPARAM wParam, shim::LPARAM lParam, bool& intercepted );
private:
	TextBuffer mTextBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}

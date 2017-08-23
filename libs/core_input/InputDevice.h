#pragma once
#include "core_input/DigitalInputComponent.h"
#include "core_input/AnalogInputComponent.h"
#include "core_input/TextInputComponent.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class InputDevice
{
public:
	void OnTick();
	virtual ~InputDevice() = default;

	UniquePtr<DigitalInputComponent> const m_DigitalComponent;
	UniquePtr<AnalogInputComponent> const m_AnalogComponent;
	UniquePtr<TextInputComponent> const m_TextComponent;

protected:
	InputDevice() = delete;
	InputDevice(
		UniquePtr<DigitalInputComponent> && digital,
		UniquePtr<AnalogInputComponent> && analog,
		UniquePtr<TextInputComponent> && text );
};


#if 0
#error WIP, still just sketching out ideas

class NullInputDevice : public InputDevice
{
public:
	NullInputDevice();
};
class WndProcInputDevice : public InputDevice
{
public:
	WndProcInputDevice();
	// Easiest, best behavior. If you don't use for text entry and cursor
	//  control, it's probably because you're an idiot...
	// NOTE: Raw input for mouse movement will be flipped on and processed as
	//  seperate relative analog input in here. Responsible developers will
	//  avoid using this unless they've got a reasonable case where they want
	//  to break support for most user input devices (for example, if you are
	//  an FPS that really only works with relative positioning devices, and
	//  also have a community that demands sub-pixel DPI motion)
};
class DirectInputDevice : public InputDevice
{
public:
	DirectInputDevice();
	// Most compatible behavior, but basically mandates an input remapping gui,
	//  so your users can map whatever the fuck the '_Rxz2' axis is supposed to
	//  mean on their uber-joystick from the 90s
};
class XInputDevice : public InputDevice
{
public:
	XInputDevice();
	// XBox 360 controller, and absolutely nothing else. Great for shitty PC
	//  ports or generally looking like a budget indie studio...
};
#endif

///////////////////////////////////////////////////////////////////////////////
}}

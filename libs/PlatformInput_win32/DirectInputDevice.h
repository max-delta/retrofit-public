#pragma once
#include "project.h"

#include "core_input/InputDevice.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// Most compatible behavior, but basically mandates an input remapping gui,
//  so your users can map whatever the fuck the '_Rxz2' axis is supposed to
//  mean on their uber-joystick from the 90s
class DirectInputDevice : public InputDevice
{
public:
	DirectInputDevice();
};

///////////////////////////////////////////////////////////////////////////////
}

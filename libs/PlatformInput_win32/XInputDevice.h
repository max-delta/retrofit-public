#pragma once
#include "project.h"

#include "core_input/InputDevice.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// XBox 360 controller, and absolutely nothing else. Great for shitty PC
//  ports or generally looking like a budget indie studio...
class XInputDevice : public InputDevice
{
public:
	XInputDevice();
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "core_input/InputFwd.h"

#include "core/macros.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// Base class for input components so that they can be enumerated and ticked
// NOTE: See derived interfaces for more specific uses
class InputComponent
{
	RF_NO_COPY( InputComponent );

public:
	InputComponent() = default;
	virtual ~InputComponent() = default;
	virtual void OnTick() = 0;
	virtual InputComponentType GetType() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

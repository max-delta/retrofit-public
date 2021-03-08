#pragma once
#include "core_input/InputFwd.h"

#include "core/macros.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

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

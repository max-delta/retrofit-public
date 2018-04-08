#pragma once
#include "rftl/cstdint"

#include "core/macros.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class InputComponent
{
	RF_NO_COPY( InputComponent );

public:
	enum ComponentType : uint8_t
	{
		Invalid = 0,
		Digital,
		Analog,
		Text
	};

	InputComponent() = default;
	virtual ~InputComponent() = default;
	virtual void OnTick() = 0;
	virtual ComponentType GetType() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

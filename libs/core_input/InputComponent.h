#pragma once
#include <stdint.h>


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class InputComponent
{
public:
	enum ComponentType : uint8_t
	{
		Invalid = 0,
		Digital,
		Analog,
		Text
	};

	virtual void OnTick() = 0;
	virtual ComponentType GetType() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

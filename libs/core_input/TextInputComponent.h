#pragma once
#include "core_input/InputComponent.h"

#include <string>


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class TextInputComponent : public InputComponent
{
	RF_NO_COPY( TextInputComponent );

public:
	TextInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual ComponentType GetType() override final
	{
		return ComponentType::Text;
	}

	virtual void GetTextStream( std::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

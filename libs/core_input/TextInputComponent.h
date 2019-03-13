#pragma once
#include "core_input/InputComponent.h"

#include "rftl/string"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class TextInputComponent : public InputComponent
{
	RF_NO_COPY( TextInputComponent );

public:
	TextInputComponent() = default;
	virtual void OnTick() override = 0;
	virtual InputComponentType GetType() override final
	{
		return InputComponentType::Text;
	}

	void GetTextStream( rftl::u16string& text ) const;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const = 0;
	virtual void ClearTextStream() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

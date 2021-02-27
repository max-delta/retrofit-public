#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/input/InputFwd.h"

#include "rftl/vector"
#include "rftl/string"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

class InputHelpers final
{
	RF_NO_INSTANCE( InputHelpers );

public:
	static rftl::vector<ui::FocusEventType> GetMainMenuInputToProcess();
	static rftl::vector<ui::FocusEventType> GetGameMenuInputToProcess( input::PlayerID player );
	static rftl::vector<input::GameCommand> GetGameplayInputToProcess( input::PlayerID player, input::LayerID layer );

	static rftl::string GetMainMenuPageMappedTextBuffer();
	static rftl::string GetMainMenuPageMappedTextBuffer( size_t maxLen );
	static rftl::u16string GetMainMenuUnicodeTextBuffer();
	static rftl::u16string GetMainMenuUnicodeTextBuffer( size_t maxLen );
	static void ClearMainMenuTextBuffer();


	static rftl::vector<ui::FocusEventType> ConvertUICommandsToFocusEvents( rftl::vector<input::GameCommand> const& commands );
};

///////////////////////////////////////////////////////////////////////////////
}

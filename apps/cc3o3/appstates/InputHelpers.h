#pragma once
#include "project.h"

#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/input/InputFwd.h"

#include "rftl/vector"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class InputHelpers final
{
	RF_NO_INSTANCE( InputHelpers );

public:
	static rftl::vector<ui::FocusEventType> GetMainMenuInputToProcess();
	static rftl::vector<ui::FocusEventType> GetGameMenuInputToProcess( input::PlayerID player );

	static rftl::vector<input::GameCommand> GetGameplayInputToProcess( input::PlayerID player, input::LayerID layer );

	static rftl::vector<ui::FocusEventType> ConvertUICommandsToFocusEvents( rftl::vector<input::GameCommand> const& commands );
};

///////////////////////////////////////////////////////////////////////////////
}}}

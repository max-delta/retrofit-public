#pragma once
#include "GameInput/GameController.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/unordered_map"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API HotkeyController final : public GameController
{
	RF_NO_COPY( HotkeyController );

	//
	// Types and constants
public:
	using CommandMapping = rftl::unordered_map<RawCommandType, GameCommandType>;


	//
	// Public methods
public:
	HotkeyController() = default;

	WeakPtr<RawController> GetSource() const;
	void SetSource( WeakPtr<RawController> const& source );

	void SetCommandMapping( CommandMapping const& mapping );

	virtual void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const override;

	//
	// Private data
private:
	WeakPtr<RawController> mSource;
	CommandMapping mCommandMapping;
};

///////////////////////////////////////////////////////////////////////////////
}}

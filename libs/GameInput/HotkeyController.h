#pragma once
#include "GameInput/GameController.h"

#include "core/ptr/weak_ptr.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

class GAMEINPUT_API HotkeyController final : public GameController
{
	//
	// Public methods
public:
	WeakPtr<RawController> GetSource() const;
	void SetSource( WeakPtr<RawController> const& source );

	virtual void GetGameCommandStream( rftl::virtual_iterator<Command>& parser, size_t maxCommands ) const override;
	virtual void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const override;
	virtual void GetGameSignalStream( rftl::virtual_iterator<Signal>& sampler, size_t maxSamples, GameSignalType type ) const override;
	virtual void GetTextStream( rftl::u16string& text, size_t maxLen ) const override;
	virtual void ClearTextStream() override;

	//
	// Private data
private:
	WeakPtr<RawController> mSource;
};

///////////////////////////////////////////////////////////////////////////////
}}

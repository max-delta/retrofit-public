#pragma once
#include "project.h"

#include "GameInput/GameController.h"

#include "core/ptr/weak_ptr.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API PassthroughController final : public GameController
{
	RF_NO_COPY( PassthroughController );

	//
	// Public methods
public:
	PassthroughController() = default;

	WeakPtr<GameController> GetSource() const;
	void SetSource( WeakPtr<GameController> const& source );

	void SuppressCommands( bool suppress );

	virtual void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const override;
	virtual void GetKnownSignals( rftl::virtual_iterator<GameSignalType>& iter, size_t maxTypes ) const;
	virtual void GetGameSignalStream( rftl::virtual_iterator<GameSignal>& sampler, size_t maxSamples, GameSignalType type ) const;
	virtual void TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) override;

	//
	// Private data
private:
	WeakPtr<GameController> mSource;
	bool mSuppressCommands = false;
};

///////////////////////////////////////////////////////////////////////////////
}

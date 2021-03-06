#pragma once
#include "project.h"

#include "GameInput/GameController.h"

#include "Rollback/RollbackFwd.h"

#include "core/ptr/weak_ptr.h"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API RollbackController final : public GameController
{
	RF_NO_COPY( RollbackController );

	//
	// Public methods
public:
	RollbackController() = default;

	WeakPtr<GameController> GetSource() const;
	void SetSource( WeakPtr<GameController> const& source );

	WeakPtr<rollback::RollbackManager> GetRollbackManager() const;
	void SetRollbackManager( WeakPtr<rollback::RollbackManager> const& manager );

	rollback::InputStreamIdentifier GetRollbackIdentifier() const;
	void SetRollbackIdentifier( rollback::InputStreamIdentifier const& identifier );

	time::CommonClock::duration GetArtificialDelay() const;
	void SetArtificialDelay( time::CommonClock::duration const& delay );

	void ProcessInput( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime );

	void AdvanceInputStream( time::CommonClock::time_point lockedFrame, time::CommonClock::time_point newWriteHead );

	virtual void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const override;
	virtual void TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) override;

	//
	// Private data
private:
	WeakPtr<GameController> mSource;
	WeakPtr<rollback::RollbackManager> mManager;
	rollback::InputStreamIdentifier mIdentifier;
	time::CommonClock::duration mArtificalDelay;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameSync/InputFwd.h"
#include "GameSync/RollbackInputPack.h"

#include "Rollback/InputStream.h"

#include "core_math/Hash.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/shared_mutex"
#include "rftl/unordered_set"
#include "rftl/vector"
#include "rftl/deque"


namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API RollbackInputManager
{
	RF_NO_COPY( RollbackInputManager );

	//
	// Types and constants
public:
	using Controller = input::RollbackController;

	using StreamIdentifiers = rftl::unordered_set<rollback::InputStreamIdentifier, math::DirectHash>;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using Controllers = rftl::vector<WeakPtr<Controller>>;


	//
	// Public methods
public:
	RollbackInputManager( WeakPtr<rollback::RollbackManager> rollMan );
	~RollbackInputManager() = default;

	// Partial thread-safety (RollbackManager unsafe)
	void AddController(
		rollback::InputStreamIdentifier const& identifier,
		WeakPtr<Controller> controller );
	void ClearAllControllers();

	// Thread-safe
	void AddLocalStreams( StreamIdentifiers const& identifiers );
	void RemoveLocalStreams( StreamIdentifiers const& identifiers );
	StreamIdentifiers GetLocalStreams() const;

	// Partial thread-safety (RollbackManager unsafe)
	void SubmitNonControllerInputs();

	// Thread-safe
	void TickLocalControllers();

	// Thread-safe
	void AdvanceLocalControllers(
		time::CommonClock::time_point lockedFrame,
		time::CommonClock::time_point newWriteHead );

	// Partial thread-safety (RollbackManager unsafe)
	RollbackInputPack PreparePackForSend(
		time::CommonClock::time_point frameReadyToCommit ) const;
	void ApplyPackFromRemote( RollbackInputPack&& pack );

	// Thread-safe
	void DebugQueueTestInput(
		time::CommonClock::time_point frame,
		rollback::InputStreamIdentifier streamID,
		rollback::InputValue input );
	void DebugClearTestInput();


	//
	// Private data
private:
	WeakPtr<rollback::RollbackManager> const mRollMan;

	mutable ReaderWriterMutex mControllersMutex;
	Controllers mControllers;

	mutable ReaderWriterMutex mLocalStreamsMutex;
	StreamIdentifiers mLocalStreams;

	mutable ReaderWriterMutex mDebugInputMutex;
	rftl::deque<rftl::pair<rollback::InputStreamIdentifier, rollback::InputEvent>> mDebugQueuedTestInput;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameAppState/AppState.h"
#include "GameAppState/AppStateChangeContext.h"

#include "Timing/clocks.h"

#include "core_fsm/FiniteStateMachine.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/optional"


namespace RF { namespace appstate {
///////////////////////////////////////////////////////////////////////////////

class GAMEAPPSTATE_API AppStateManager final : private fsm::FiniteStateMachine<
	AppStateRef,
	AppStateID,
	AppStateChangeContext>
{
	RF_NO_COPY( AppStateManager );

	//
	// Public methods
public:
	AppStateManager();

	void AddState( AppStateID stateID, UniquePtr<AppState>&& state );

	void Start( AppStateID initialStateID );
	void Tick( time::FrameClock::time_point currentTime, time::FrameClock::duration elapsedTimeSinceLastTick );
	void Stop();

	void RequestDeferredStateChange( AppStateID stateID );
	void ApplyDeferredStateChange();


	//
	// Private methods
private:
	virtual AppStateChangeContext CreateStateChangeContext() override;


	//
	// Private data
private:
	UniquePtr<FiniteStateMachine::StateCollection> mBackingStateCollection;
	rftl::optional<StateID> mDeferredStateChange;
};

///////////////////////////////////////////////////////////////////////////////
}}

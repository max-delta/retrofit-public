#pragma once
#include "core_thread/ThreadSignal.h"

#include "rftl/atomic"
#include "rftl/thread"
#include "rftl/functional"
#include "rftl/shared_mutex"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

// Basic worker, meant to offload inconsistent work onto another thread
class AsyncThread final
{
	RF_NO_COPY( AsyncThread );

	//
	// Types and constants
public:
	using PrepSig = void();
	using PrepFunc = rftl::function<PrepSig>;
	using WorkSig = void();
	using WorkFunc = rftl::function<WorkSig>;
	using WorkConditionSig = bool();
	using WorkConditionFunc = rftl::function<WorkConditionSig>;
	using TerminateConditionSig = bool();
	using TerminateConditionFunc = rftl::function<TerminateConditionSig>;

private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using SafetyTime = rftl::chrono::milliseconds;


	//
	// Public methods
public:
	AsyncThread() = default;
	~AsyncThread();

	// NOTE: Prep function is optional, but intended to be used for setting
	//  thread priorities and thread names
	// NOTE: Work condition should not blindly return true, or it will result
	//  in excessive CPU load
	// NOTE: Termination condition may be null, which will cause the thread to
	//  run until explicitly told to stop
	// Thread-safe
	void Init(
		PrepFunc&& prep,
		WorkFunc&& work,
		WorkConditionFunc&& workCheck,
		TerminateConditionFunc&& termCheck );

	// NOTE: Stop is not terminal, work can be started again without re-init
	// Thread-safe
	bool IsStarted() const;
	void Start();
	void Stop();

	// Thread will wake on explicit wakeup, spontaneous wakeup, or on a safety
	//  interval used to protect against lock-ups due to logic errors
	// Thread-safe
	void Wake();
	void SetSafetyWakeupInterval( rftl::chrono::nanoseconds duration );


	//
	// Private methods
private:
	template<typename Lock>
	bool IsStartedGuarded( Lock const& lock ) const;

	static void WorkThread( AsyncThread& parent );


	//
	// Private data
private:
	mutable ReaderWriterMutex mCallbacksMutex;
	PrepFunc mPrep;
	WorkFunc mWork;
	WorkConditionFunc mWorkCondition;
	TerminateConditionFunc mTerminateCondition;

	rftl::atomic<SafetyTime::rep> mSafetyWakeupInterval =
		rftl::chrono::duration_cast<SafetyTime>( rftl::chrono::seconds{ 5 } ).count();

	mutable ReaderWriterMutex mThreadMutex;
	rftl::thread mThread;

	ThreadSignal mWakeSignal;
	rftl::atomic<bool> mRunFlag = false;
};

///////////////////////////////////////////////////////////////////////////////
}

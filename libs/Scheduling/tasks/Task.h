#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/cstdint"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

class SCHEDULING_API Task
{
	RF_NO_COPY( Task );

	//
	// Enums
public:
	enum class State : uint8_t
	{
		Invalid = 0,

		Ready = 1,
		Running = 2,
		Terminate = 3,

		MaxVal = 0x3u // NOTE: Usable as mask
	};

	enum class Priority : uint8_t
	{
		Invalid = 0,

		Idle = 4,
		Normal = 8,
		High = 13,
		RealTime = 24,

		MaxVal = 0x1fu // NOTE: Usable as mask
	};


	//
	// Types and constants
public:
	static constexpr uint8_t kStateMask = static_cast<uint8_t>( State::MaxVal );
	static constexpr uint8_t kPriorityMask = static_cast<uint8_t>( Priority::MaxVal );


	//
	// Public methods
public:
	Task() = default;

	// NOTE: A task that is readied, but never run, may be cancelled and
	//  destroyed without having ever been stepped
	virtual ~Task() = default;

	// Attempts to run a task incrementally, which may or may not result in
	//  the task terminating itself in the course of the step
	// NOTE: Valid return values are:
	//  * Ready - The task will need to be stepped again
	//  * Terminate - The task is complete and will need to be terminated
	virtual State Step() = 0;

	// Aborts an incremental task that previously was stepped without the task
	//  self-terminating
	virtual void Abort()
	{
		// NOTE: Non-incremental tasks will never have this called on them
	}
};

///////////////////////////////////////////////////////////////////////////////
}}

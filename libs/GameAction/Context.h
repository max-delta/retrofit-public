#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core/ptr/ptr_fwd.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// The mutable 'stuff' that gets acted upon by actions, expected to be derived
//  from and specialized for the needs of the game using it
class GAMEACTION_API Context
{
	//
	// Public methods
public:
	Context();
	Context( Context const& rhs );
	virtual ~Context();

	// Contexts must be deep-cloneable to allow for proper isolation when
	//  running actions with multiple complex steps
	virtual UniquePtr<Context> Clone() const = 0;

	// Contexts can enter into an error state (or some contexts could ALWAYS
	//  represent an error state), and this error state can inform calling code
	//  that it should terminate any running logic and propogate the context up
	//  as an error
	virtual bool IsATerminalError() const;
};

///////////////////////////////////////////////////////////////////////////////
}

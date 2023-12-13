#pragma once
#include "project.h"

#include "GameAction/ActionFwd.h"

#include "core_reflect/VirtualClass.h"
#include "core_rftype/CreateClassInfoDeclaration.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

// Base class for executions on a context like 'grow context to include the
//  allies of the target' or 'do damage to the target'
class GAMEACTION_API Step : public RF::reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_DEFAULT_COPY( Step );

	//
	// Public methods
public:
	Step() = default;
	virtual ~Step() = default;

	// Execute a step on a given context in a given environment
	// NOTE: Return value is optional, and is used when the step wishes to
	//  provide a new context to the caller (an example would be for relaying
	//  error information from a failed execution)
	// NOTE: Expectation for normal logic flow is that the provided context is
	//  mutated in-place as needed, or cloned / created / passed to child steps
	virtual UniquePtr<Context> Execute( Environment const& env, Context& ctx ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

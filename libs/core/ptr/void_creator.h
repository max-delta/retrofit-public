#pragma once
#include "core/macros.h"
#include "core/ptr/creation_payload.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Esoteric creator, that only allocates/deletes the ref machinery, and
//  preserves the pointer value
// NOTE: The valid uses for this should be limited beyond just test code
class VoidCreator
{
	RF_NO_INSTANCE( VoidCreator );

	//
	// Public methods
public:
	static CreationPayload<void> Create( void* ptr );


	//
	// Private methods
private:
	static void Delete( void* target, PtrRef* ref, void* userData );
};

///////////////////////////////////////////////////////////////////////////////
}

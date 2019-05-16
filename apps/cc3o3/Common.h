#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF {
namespace cc {
namespace character {
	class CharacterDatabase;
}
}
}

namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
extern WeakPtr<character::CharacterDatabase> gCharacterDatabase;

void SystemStartup();
void SystemShutdown();

///////////////////////////////////////////////////////////////////////////////
}}

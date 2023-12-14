#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF::rollback {
class RollbackManager;
}
namespace RF::sync {
class RollbackInputManager;
}
namespace RF::resource {
class ResourceTypeRegistry;
class ResourceLoader;
class ResourceSaver;
}
namespace RF::sprite {
class CharacterCreator;
}
namespace RF::cc::character {
class CharacterValidator;
class CharacterDatabase;
}
namespace RF::cc::combat {
class CombatEngine;
}
namespace RF::cc::element {
class ElementDatabase;
}
namespace RF::cc::cast {
class CastingEngine;
}
namespace RF::cc::company {
class CompanyManager;
}
namespace RF::cc::encounter {
class EncounterManager;
}
namespace RF::component {
template<typename TypeResolver>
class TypedObjectManager;
}
namespace RF::cc::save {
class SaveManager;
}
namespace RF::cc::state {
struct ComponentResolver;
using ObjectManager = component::TypedObjectManager<ComponentResolver>;
}
namespace RF::cc::campaign {
class CampaignManager;
}

namespace RF::cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
extern WeakPtr<rollback::RollbackManager> gRollbackManager;
extern WeakPtr<sync::RollbackInputManager> gRollbackInputManager;
extern WeakPtr<resource::ResourceTypeRegistry> gResourceTypeRegistry;
extern WeakPtr<resource::ResourceLoader> gResourceLoader;
extern WeakPtr<resource::ResourceSaver> gResourceSaver;
extern WeakPtr<sprite::CharacterCreator> gCharacterCreator;
extern WeakPtr<character::CharacterValidator> gCharacterValidator;
extern WeakPtr<character::CharacterDatabase> gCharacterDatabase;
extern WeakPtr<combat::CombatEngine> gCombatEngine;
extern WeakPtr<element::ElementDatabase> gElementDatabase;
extern WeakPtr<cast::CastingEngine> gCastingEngine;
extern WeakPtr<company::CompanyManager> gCompanyManager;
extern WeakPtr<encounter::EncounterManager> gEncounterManager;
extern WeakPtr<state::ObjectManager> gObjectManager;
extern WeakPtr<save::SaveManager> gSaveManager;
extern WeakPtr<campaign::CampaignManager> gCampaignManager;

void SystemStartup();
void SystemShutdown();

///////////////////////////////////////////////////////////////////////////////
}

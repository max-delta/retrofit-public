#pragma once
#include "project.h"

#include "core/ptr/ptr_fwd.h"

// Forwards
namespace RF::sprite {
class CharacterCreator;
}
namespace RF::cc::character {
class CharacterValidator;
class CharacterDatabase;
}
namespace RF::cc::company {
class CompanyManager;
}
namespace RF::cc::element {
class ElementDatabase;
}
namespace RF::component {
template<typename TypeResolver>
class TypedObjectManager;
}
namespace RF::cc::state {
struct ComponentResolver;
using ObjectManager = component::TypedObjectManager<ComponentResolver>;
}

namespace RF { namespace cc {
///////////////////////////////////////////////////////////////////////////////

// Global systems
extern WeakPtr<sprite::CharacterCreator> gCharacterCreator;
extern WeakPtr<character::CharacterValidator> gCharacterValidator;
extern WeakPtr<character::CharacterDatabase> gCharacterDatabase;
extern WeakPtr<company::CompanyManager> gCompanyManager;
extern WeakPtr<element::ElementDatabase> gElementDatabase;
extern WeakPtr<state::ObjectManager> gObjectManager;

void SystemStartup();
void SystemShutdown();

///////////////////////////////////////////////////////////////////////////////
}}

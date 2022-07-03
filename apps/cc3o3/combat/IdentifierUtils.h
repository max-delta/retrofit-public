#pragma once
#include "cc3o3/combat/CombatFwd.h"

#include "rftl/string_view"


namespace RF::cc::combat {
///////////////////////////////////////////////////////////////////////////////

// TODO: Better solution for enum<->string (probably in C++20)
EntityClass ReadEntityClassFromString( rftl::string_view str );

///////////////////////////////////////////////////////////////////////////////
}

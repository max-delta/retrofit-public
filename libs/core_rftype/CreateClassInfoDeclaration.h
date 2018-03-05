#pragma once

#include "core_reflect/ClassInfo.h"

// Creates static storage within a class, primarily intended to tie the storage
//  to the class so it participates in module exports and imports that the
//  enclosing class specifies
// NOTE: Must be publically accessible, macro enforces this since the resulting
//  compilation errors are extremely non-obvious
// NOTE: Requires either a fully-qualified CLASSTYPE parameter or appropriate
//  namespace wrapping for te definition
// NOTE: Normal definition rules apply, including multiple definitions
// EXAMPLE:
//  struct CLASSTYPE
//  {
//  	RFTYPE_STATIC_CLASSINFO();
//  };
//  RFTYPE_DEFINE_STATIC_CLASSINFO( CLASSTYPE );
#define RFTYPE_STATIC_CLASSINFO() public: static ::RF::reflect::ClassInfo ___RFType_Static_ClassInfo

#pragma once
#include "project.h"

#include "core/macros.h"

#include "rftl/unordered_set"
#include "rftl/vector"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

using ContainerID = uint64_t;
static constexpr ContainerID kInvalidContainerID = 0;
static constexpr ContainerID kRootContainerID = 1;
using ContainerIDList = rftl::vector<ContainerID>;
using ContainerIDSet = rftl::unordered_set<ContainerID>;

using AnchorID = uint64_t;
static constexpr AnchorID kInvalidAnchorID = 0;
using AnchorIDList = rftl::vector<AnchorID>;
using AnchorIDSet = rftl::unordered_set<AnchorID>;

using FontPurposeID = uint16_t;
static constexpr FontPurposeID kInvalidFontPurposeID = 0;

class ContainerManager;
class Controller;
class FontRegistry;

struct Container;
struct Anchor;

///////////////////////////////////////////////////////////////////////////////
}}

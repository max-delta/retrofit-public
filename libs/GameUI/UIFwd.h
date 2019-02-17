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

class ContainerManager;
class UIController;

struct Container;
struct Anchor;

///////////////////////////////////////////////////////////////////////////////
}}

#include "stdafx.h"
#include "project.h"

#include "Allocation/AccessorDefinition.h"

template RF::alloc::Allocator* RF::alloc::GetAllocator<RFTAG_ROLLBACK_SHARED_STATE>();
template RF::alloc::Allocator* RF::alloc::GetAllocator<RFTAG_ROLLBACK_PRIVATE_STATE>();
template RF::alloc::Allocator* RF::alloc::GetAllocator<RFTAG_ROLLBACK_SNAPSHOTS>();

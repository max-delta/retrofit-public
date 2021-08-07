#pragma once
#include "project.h"

#include "RollbackFwd.h"

#include "core_allocate/AllocatorFwd.h"

namespace RF::rollback {
///////////////////////////////////////////////////////////////////////////////

void WriteToSnapshot( Snapshot& dest, Window const& src, time::CommonClock::time_point time, alloc::Allocator& allocator );
void ReadFromSnapshot( Window& dest, Snapshot const& src, alloc::Allocator& allocator );

///////////////////////////////////////////////////////////////////////////////
}

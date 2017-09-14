#pragma once
#include "PPU/PPUFwd.h"
#include "PlatformFilesystem/VFSFwd.h"

/////
// TODO: Replace save/load with reflection-powered de/serializers
/////

namespace RF {
///////////////////////////////////////////////////////////////////////////////

RF::UniquePtr<RF::gfx::FramePackBase> LoadFramePackFromSquirrel( RF::file::VFSPath const& filename );

///////////////////////////////////////////////////////////////////////////////
}

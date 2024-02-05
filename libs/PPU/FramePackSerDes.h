#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"
#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/extension/byte_view.h"
#include "rftl/vector"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackSerDes
{
public:
	static bool SerializeToBuffer( gfx::TextureManager const& texMan, rftl::vector<uint8_t>& buffer, FramePackBase const& framePack );
	static bool DeserializeFromBuffer( rftl::vector<file::VFSPath>& textures, rftl::byte_view const& buffer, UniquePtr<FramePackBase>& framePack );
};

///////////////////////////////////////////////////////////////////////////////
}

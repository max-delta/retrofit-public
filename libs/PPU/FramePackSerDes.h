#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"
#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/vector"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackSerDes
{
public:
	static bool SerializeToBuffer( gfx::TextureManager const& texMan, rftl::vector<uint8_t>& buffer, FramePackBase const& framePack );
	static bool DeserializeFromBuffer( rftl::vector<file::VFSPath>& textures, rftl::vector<uint8_t> const& buffer, UniquePtr<FramePackBase>& framePack );
};

///////////////////////////////////////////////////////////////////////////////
}}

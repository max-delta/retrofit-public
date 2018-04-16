#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "rftl/vector"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackSerDes
{
public:
	static bool SerializeToBuffer( gfx::TextureManager const& texMan, rftl::vector<uint8_t>& buffer, FramePackBase const& framePack );
};

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"

#include "rftl/unordered_map"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

using CharacterAnimKey = rftl::string;



struct GAMESPRITE_API CompositeCharacter
{
	RF_DEFAULT_COPY( CompositeCharacter );

	CompositeCharacter();
	~CompositeCharacter();

	using FramepacksByAnim = rftl::unordered_map<CharacterAnimKey, file::VFSPath>;
	FramepacksByAnim mFramepacksByAnim = {};
};

///////////////////////////////////////////////////////////////////////////////
}

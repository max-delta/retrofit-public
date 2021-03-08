#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"

#include "rftl/unordered_map"


namespace RF::sprite {
///////////////////////////////////////////////////////////////////////////////

enum class CharacterSequenceType : uint8_t
{
	Invalid = 0,

	// Top-down
	// Cardinal directions [N; E; S; W]
	// Pallete reference tile
	// Idle frame and walk cycles
	// 4 frames, 3 unique [Step 1; Mid-step; Step 2]
	P_NESW_i121,

	// As P_NESW_i121, but in 3 rows for layering
	Far_mid_near_P_NESW_i121
};



enum class CharacterAnimKey : uint8_t
{
	Invalid = 0,

	NIdle,
	EIdle,
	SIdle,
	WIdle,

	NWalk,
	EWalk,
	SWalk,
	WWalk
};



struct GAMESPRITE_API CompositeCharacter
{
	RF_DEFAULT_COPY( CompositeCharacter );

	CompositeCharacter();
	~CompositeCharacter();

	CharacterSequenceType mCharacterSequenceType = CharacterSequenceType::Invalid;

	using FramepacksByAnim = rftl::unordered_map<CharacterAnimKey, file::VFSPath>;
	FramepacksByAnim mFramepacksByAnim = {};
};

///////////////////////////////////////////////////////////////////////////////
}

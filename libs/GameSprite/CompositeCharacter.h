#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSPath.h"

#include "rftl/unordered_map"


namespace RF { namespace sprite {
///////////////////////////////////////////////////////////////////////////////

enum class CharacterSequenceType : uint8_t
{
	Invalid = 0,

	// Top-down
	// Cardinal directions [N; E; S; W]
	// Walk cycles only
	// 4 frames, 3 unique [Step 1; Mid-step; Step 2]
	// Empty tile between contiguous sequences
	N3_E3_S3_W3,

	// As N3_E3_S3_W3, but in 3 rows for layering
	Near_far_tail_N3_E3_S3_W3
};



enum class CharacterAnimKey : uint8_t
{
	Invalid = 0,
	NWalk,
	EWalk,
	SWalk,
	WWalk
};



struct GAMESPRITE_API CompositeCharacter
{
	CompositeCharacter();
	~CompositeCharacter();

	CharacterSequenceType mCharacterSequenceType = CharacterSequenceType::Invalid;

	using FramepacksByAnim = rftl::unordered_map<CharacterAnimKey, file::VFSPath>;
	FramepacksByAnim mFramepacksByAnim = {};
};

///////////////////////////////////////////////////////////////////////////////
}}

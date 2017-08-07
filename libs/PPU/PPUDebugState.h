#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core_math/vector2.h"

#include "core/macros.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUDebugState
{
	RF_NO_COPY( PPUDebugState );
	RF_NO_MOVE( PPUDebugState );

	// Constants
public:
	static constexpr size_t k_MaxStrings = 32;
	static constexpr size_t k_MaxLines = 32;


	//
	// Structs
public:
	struct DebugString
	{
		static constexpr size_t k_MaxLen = 127;

		PPUCoordElem m_XCoord;
		PPUCoordElem m_YCoord;
		char m_Text[k_MaxLen + sizeof( '\0' )];
	};
	struct DebugLine
	{
		PPUCoordElem m_XCoord0;
		PPUCoordElem m_YCoord0;
		PPUCoordElem m_XCoord1;
		PPUCoordElem m_YCoord1;
	};


	//
	// Public methods
public:
	PPUDebugState() = default;
	~PPUDebugState() = default;

	void Clear();


	//
	// Public data
public:
	uint8_t m_NumStrings;
	uint8_t m_NumLines;
	DebugString m_Strings[k_MaxStrings];
	DebugLine m_Lines[k_MaxLines];
};

///////////////////////////////////////////////////////////////////////////////
}}

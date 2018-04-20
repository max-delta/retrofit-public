#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/Object.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUState
{
	RF_NO_COPY( PPUState );
	RF_NO_MOVE( PPUState );

	// Constants
public:
	static constexpr size_t k_MaxObjects = 32;
	static constexpr size_t k_MaxStrings = 32;


	//
	// Structs
public:
	struct String
	{
		static constexpr size_t k_MaxLen = 127;

		PPUCoordElem m_XCoord;
		PPUCoordElem m_YCoord;
		PPUCoordElem m_Width;
		PPUCoordElem m_Height;
		char m_Text[k_MaxLen + sizeof( '\0' )];
	};


	//
	// Public methods
public:
	PPUState() = default;
	~PPUState() = default;

	void Clear();


	//
	// Public data
public:
	uint8_t m_NumObjects;
	uint8_t m_NumStrings;
	Object m_Objects[k_MaxObjects];
	String m_Strings[k_MaxStrings];
};

///////////////////////////////////////////////////////////////////////////////
}}

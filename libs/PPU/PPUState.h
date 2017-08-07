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
	Object m_Objects[k_MaxObjects];
};

///////////////////////////////////////////////////////////////////////////////
}}

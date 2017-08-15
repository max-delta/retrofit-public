#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core/macros.h"
#include "core/ptr/ptr_traits.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackBase : public PtrTrait::NoVirtualDestructor
{
	RF_NO_COPY( FramePackBase );
	RF_NO_MOVE( FramePackBase );


	// Structs
public:
	struct GameColliderLayerMapping
	{
		GameLayerID m_GameLayer;
		uint8_t m_ColliderLayer;
	};
	static_assert( sizeof(GameColliderLayerMapping) == 2, "Re-evaluate GameColliderLayerMapping packing" );

	struct TimeSlot
	{
		ManagedTextureID m_TextureReference;
		uint64_t m_ColliderReference;
		uint8_t m_TextureOriginX;
		uint8_t m_TextureOriginY;
		uint8_t m_ColliderOriginX;
		uint8_t m_ColliderOriginY;
		uint32_t reserved : 32;
	};
	static_assert( sizeof(TimeSlot) == 24, "Re-evaluate TimeSlot packing" );


	//
	// Public methods
public:
	uint8_t const* GetTimeSlotSustains() const;
	TimeSlot const* GetTimeSlots() const;

	uint8_t CalculateTimeSlotFromTimeIndex( uint8_t timeIndex ) const;


	//
	// Protected methods
protected:
	FramePackBase() = delete;
	FramePackBase( uint8_t maxTimeSlots );
	~FramePackBase() = default;


	//
	// Public data
public:
	GFXModifierHandlerID m_GFXModifierHandlers[k_NumGFXModifiers];
	GameColliderLayerMapping m_GameColliderLayerMappings[k_NumGameLayers];
	uint8_t m_PreferredSlowdownRate; // 1=60, 2=30, 3=20, 4=15...
	uint8_t m_NumTimeSlots;
	uint8_t reserved3;
	uint8_t reserved4;
	uint8_t reserved5;
	uint8_t reserved6;
	uint8_t reserved7;

	// NOTE: This member acts as both the anchor for indexing into the derived
	//  storage class, as well as this base class's runtime knowledge of the
	//  derived class's compile-time knowledge
	uint8_t const m_MaxTimeSlots;
};

template<size_t TIMESLOTS>
class FramePack final : public FramePackBase
{
	RF_NO_COPY( FramePack );
	RF_NO_MOVE( FramePack );

	// Constants
public:
	// VS2015 bug: Intellisense can't read static array members sized with
	//  constexpr from a template argument. Workaround: Use the template
	//  argument directly instead.
	static constexpr size_t k_MaxTimeSlots = TIMESLOTS;
	static_assert( k_MaxTimeSlots <= 256, "Need to be able to index tme slots using uint8_t" );


	//
	// Public methods
public:
	FramePack();
	~FramePack() = default;


	//
	// Public data
public:
	// TODO: Make private, use safe accessors from base
	uint8_t m_TimeSlotSustains[TIMESLOTS];
	TimeSlot m_TimeSlots[TIMESLOTS];
};

static_assert( alignof( FramePackBase ) == 8, "Double-check FramePack alignment" );
static_assert( alignof( FramePack_4096 ) == 8, "Double-check FramePack alignment" );
static_assert( alignof( FramePack_1024 ) == 8, "Double-check FramePack alignment" );
static_assert( alignof( FramePack_512 ) == 8, "Double-check FramePack alignment" );
static_assert( alignof( FramePack_256 ) == 8, "Double-check FramePack alignment" );
static_assert( sizeof( FramePackBase ) == 88, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_4096 ) <= 4096, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_4096 ) >= 4088, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_1024 ) <= 1024, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_1024 ) >= 1016, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_512 ) <= 512, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_512 ) >= 488, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_256 ) <= 256, "Double-check FramePack storage" );
static_assert( sizeof( FramePack_256 ) >= 240, "Double-check FramePack storage" );

// It is critical that the derived classes can be accessed via consistent
//  offset from the base class
// NOTE: See code in offset getters on base class for clearer explanation of
//  offset math, aided by variable names (which can't be used in these asserts)
static_assert( offsetof( FramePackBase, m_MaxTimeSlots ) == 87, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_4096, m_TimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_1024, m_TimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_512, m_TimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_256, m_TimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert(
	offsetof( FramePack_4096, m_TimeSlotSustains ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_1024, m_TimeSlotSustains ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_512, m_TimeSlotSustains ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_256, m_TimeSlotSustains ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_4096, m_TimeSlots ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_4096::k_MaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_4096::k_MaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_1024, m_TimeSlots ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_1024::k_MaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_1024::k_MaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_512, m_TimeSlots ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_512::k_MaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_512::k_MaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_256, m_TimeSlots ) ==
	offsetof( FramePackBase, m_MaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_256::k_MaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_256::k_MaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );

///////////////////////////////////////////////////////////////////////////////
}}

#include "FramePack.inl"

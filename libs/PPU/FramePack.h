#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/macros.h"
#include "core/ptr/ptr_traits.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackBase : public PtrTrait::NoVirtualDestructor
{
	RF_NO_COPY( FramePackBase );
	RF_NO_MOVE( FramePackBase );


	// Structs
public:
	struct GameColliderLayerMapping
	{
		GameLayerID mGameLayer = 0xffu;
		uint8_t mColliderLayer = 0xffu;
	};
	static_assert( sizeof( GameColliderLayerMapping ) == 2, "Re-evaluate GameColliderLayerMapping packing" );

	struct TimeSlot
	{
		ManagedTextureID mTextureReference = kInvalidManagedTextureID;
		ManagedColliderID mColliderReference = kInvalidManagedTextureID;
		uint8_t mTextureOriginX = 0x0u;
		uint8_t mTextureOriginY = 0x0u;
		uint8_t mColliderOriginX = 0x0u;
		uint8_t mColliderOriginY = 0x0u;
		uint32_t reserved : 32;
	};
	static_assert( sizeof( TimeSlot ) == 24, "Re-evaluate TimeSlot packing" );


	//
	// Public methods
public:
	uint8_t const* GetTimeSlotSustains() const;
	uint8_t* GetMutableTimeSlotSustains();
	TimeSlot const* GetTimeSlots() const;
	TimeSlot* GetMutableTimeSlots();

	uint8_t CalculateTimeSlotFromTimeIndex( uint8_t timeIndex ) const;
	uint8_t CalculateFirstTimeIndexOfTimeSlot( uint8_t timeSlot ) const;

	// Time index should be reset to 0 on a multiple of this value before it rolls
	//  over, or the remainder will not have time to complete a full animation
	// NOTE: Also serves as a calculation of the animation length
	uint8_t CalculateTimeIndexBoundary() const;

	// This will copy over values from another frame pack's base, but keep the
	//  immutable values as they are (like max time slots)
	// NOTE: Does NOT copy over non-base data like slots or sustains
	void CopyBaseValuesFrom( FramePackBase const& rhs );


	//
	// Protected methods
protected:
	FramePackBase() = delete;
	FramePackBase( uint8_t maxTimeSlots );
	~FramePackBase() = default;


	//
	// Public data
public:
	GFXModifierHandlerID mGFXModifierHandlers[kNumGFXModifiers] = {};
	GameColliderLayerMapping mGameColliderLayerMappings[kNumGameLayers] = {};
	uint8_t mPreferredSlowdownRate = 1; // 1=60, 2=30, 3=20, 4=15...
	uint8_t mNumTimeSlots = 0;
	uint8_t reserved3;
	uint8_t reserved4;
	uint8_t reserved5;
	uint8_t reserved6;
	uint8_t reserved7;

	// NOTE: This member acts as both the anchor for indexing into the derived
	//  storage class, as well as this base class's runtime knowledge of the
	//  derived class's compile-time knowledge
	uint8_t const mMaxTimeSlots;
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
	static constexpr size_t kMaxTimeSlots = TIMESLOTS;
	static_assert( kMaxTimeSlots <= 256, "Need to be able to index tme slots using uint8_t" );


	//
	// Public methods
public:
	FramePack();
	~FramePack() = default;


	//
	// Public data
public:
	// TODO: Make private, use safe accessors from base
	uint8_t mTimeSlotSustains[TIMESLOTS];
	TimeSlot mTimeSlots[TIMESLOTS];
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
RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Winvalid-offsetof" ); // Technically incorrect, fine in practice
static_assert( offsetof( FramePackBase, mMaxTimeSlots ) == 87, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_4096, mTimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_1024, mTimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_512, mTimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert( offsetof( FramePack_256, mTimeSlotSustains ) == 88, "Double-check FramePack storage" );
static_assert(
	offsetof( FramePack_4096, mTimeSlotSustains ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_1024, mTimeSlotSustains ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_512, mTimeSlotSustains ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_256, mTimeSlotSustains ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1,
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_4096, mTimeSlots ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_4096::kMaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_4096::kMaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_1024, mTimeSlots ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_1024::kMaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_1024::kMaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_512, mTimeSlots ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_512::kMaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_512::kMaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
static_assert(
	offsetof( FramePack_256, mTimeSlots ) ==
	offsetof( FramePackBase, mMaxTimeSlots ) + 1 +
	sizeof( uint8_t[FramePack_256::kMaxTimeSlots] ) +
	( ( 8 - sizeof( uint8_t[FramePack_256::kMaxTimeSlots] ) % 8 ) % 8 ),
	"Double-check FramePack offset math" );
RF_CLANG_POP();

///////////////////////////////////////////////////////////////////////////////
}

#include "FramePack.inl"

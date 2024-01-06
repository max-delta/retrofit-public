#pragma once
#include "project.h"

#include "cc3o3/input/InputFwd.h"

#include "Rollback/RollbackFwd.h"

#include "core/macros.h"


namespace RF::cc::input {
///////////////////////////////////////////////////////////////////////////////

// TODO: This is obviously gross, replace it
void HardcodedDeviceSetup();
void HardcodedRawSetup();
void HardcodedMainSetup();
void HardcodedPlayerSetup( PlayerID playerID );
void HardcodedHackSetup( PlayerID playerID );
void HardcodedRawTick();
void HardcodedSeverTick();

///////////////////////////////////////////////////////////////////////////////

struct HardcodedRollbackIdentifiers
{
	RF_NO_COPY( HardcodedRollbackIdentifiers );

private:
	static constexpr uint8_t kStride = 2;
	static_assert( kInvalidPlayerID == 0 );

public:
	constexpr HardcodedRollbackIdentifiers( PlayerID playerID )
		: mRollbackStartID( ( ( playerID - 1u ) * kStride ) + 1u )
		, mRollbackGameMenusID( mRollbackStartID + 0u )
		, mRollbackGamplayID( mRollbackStartID + 1u )
	{
	}

private:
	rollback::InputStreamIdentifier const mRollbackStartID;

public:
	rollback::InputStreamIdentifier const mRollbackGameMenusID;
	rollback::InputStreamIdentifier const mRollbackGamplayID;
};

///////////////////////////////////////////////////////////////////////////////
}

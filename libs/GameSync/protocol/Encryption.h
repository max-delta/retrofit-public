#pragma once
#include "project.h"

#include "GameSync/protocol/Structure.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

static constexpr EncryptionMode kExpectedEncryption = EncryptionMode::kMockInert;

///////////////////////////////////////////////////////////////////////////////

struct GAMESYNC_API EncryptionState final
{
	EncryptionMode mMode = EncryptionMode::kUnencrypted;
	EncryptionMode mPending = EncryptionMode::kUnencrypted;
	// TODO: Add data needed for encryption
};

///////////////////////////////////////////////////////////////////////////////

GAMESYNC_API bool PrepareEncryptionRequest( EncryptionState& encryption );
GAMESYNC_API bool PrepareEncryptionResponse( EncryptionState& encryption );
GAMESYNC_API void ApplyPendingEncryption( EncryptionState& encryption );

GAMESYNC_API bool EncryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption );
GAMESYNC_API bool DecryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption );

///////////////////////////////////////////////////////////////////////////////
}

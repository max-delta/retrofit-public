#pragma once
#include "project.h"

#include "GameSync/protocol/Structure.h"

#include "rftl/functional"


// Forwards
namespace RF::sync::protocol {
struct EncryptionState;
}

namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

using OnBatchSig = ReadResult( size_t totalBytes, rftl::byte_view bytes );
using OnBatchFunc = rftl::function<OnBatchSig>;
using OnMessageSig = ReadResult( MessageID const& id, rftl::byte_view& bytes );
using OnMessageFunc = rftl::function<OnMessageSig>;

///////////////////////////////////////////////////////////////////////////////

// Attempt to blindly decode a batch, ignoring state/logic errors, and simply
//  checking whether the messages seem like they follows the basic structure of
//  the protocol and can be decoded
GAMESYNC_API ReadResult TryBlindDecodeBatch(
	rftl::byte_view bytes,
	EncryptionState const& encryption );

// Attempt to decode a batch, with a provided hook to handle messages
GAMESYNC_API ReadResult TryDecodeBatch(
	rftl::byte_view bytes,
	EncryptionState const& encryption,
	OnMessageFunc const& onMessage );

// Attempt to decode a transmission, with a provided hook to handle batches
// NOTE: Batches may not be decodable on their own, requiring additional
//  batches to be combined in order to form a complete payload
GAMESYNC_API ReadResult TryDecodeTransmission(
	rftl::byte_view& bytes,
	EncryptionState const& encryption,
	OnBatchFunc const& onBatch );

///////////////////////////////////////////////////////////////////////////////
}

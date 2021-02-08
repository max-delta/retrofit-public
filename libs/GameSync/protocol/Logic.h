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

///////////////////////////////////////////////////////////////////////////////

// Message need to be batched, encrypted, and may need to be broken up into
//  multiple transmissions
GAMESYNC_API rftl::vector<Buffer> CreateTransmissions(
	Buffer&& messages,
	EncryptionState const& encryption,
	size_t maxTransmissionSize );

// Attempt to decode a transmission, with a provided hook to handle batches
// NOTE: Batches may not be decodable on their own, requiring additional
//  batches to be combined in order to form a complete payload
// NOTE: Will only decode a maximum of one batch per call
GAMESYNC_API ReadResult TryDecodeTransmission(
	rftl::byte_view& bytes,
	EncryptionState const& encryption,
	OnBatchFunc const& onBatch );

///////////////////////////////////////////////////////////////////////////////

// Hello payloads are special in that they cannot be broken up across multiple
//  transmissions, and are never encrypted
GAMESYNC_API Buffer CreateHelloTransmission(
	size_t maxTransmissionSize );
GAMESYNC_API ReadResult TryDecodeHelloTransmission(
	rftl::byte_view& bytes );

// Welcome payloads are special in that they cannot be broken up across multiple
//  transmissions, and are never encrypted
GAMESYNC_API Buffer CreateWelcomeTransmission(
	size_t maxTransmissionSize );
GAMESYNC_API ReadResult TryDecodeWelcomeTransmission(
	rftl::byte_view& bytes );

///////////////////////////////////////////////////////////////////////////////
}

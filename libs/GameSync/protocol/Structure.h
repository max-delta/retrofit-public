#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"

#include "rftl/extension/byte_view.h"
#include "rftl/array"
#include "rftl/vector"
#include "rftl/limits"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

enum class ReadResult : uint8_t
{
	kSuccess = 0,
	kTooSmall,
	kWrongProtocol,
	kWrongEncryption,
	kBadEncryption,
	kUnknownMessage,
	kLogicError,
};

enum class EncryptionMode : uint8_t
{
	kUnencrypted = 0,
	kMockInert
};

using MessageID = rftl::array<char, 4>;
using Buffer = rftl::vector<uint8_t>;

///////////////////////////////////////////////////////////////////////////////

// Every transmission starts by declaring it is part of RetroFit Sync
struct GAMESYNC_API CommonHeader final
{
	static constexpr uint8_t kVersion = 0;
	static constexpr char const kMagic1[] = { 'R', 'F', 'I', 'T' };
	static constexpr char const kMagic2[] = { 'S', 'Y', 'N', kVersion };

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



// Messages are always put into a batch, which may contain multiple messages
//  and/or need to be combined with other batches before decoding
// NOTE: A batch of size 0 is permitted
struct GAMESYNC_API MessageBatch final
{
	static constexpr size_t kMaxBytes = rftl::numeric_limits<uint16_t>::max();
	size_t mTotalBytes = 0;
	size_t mBatchBytes = 0;
	EncryptionMode mEncryption = EncryptionMode::kUnencrypted;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



// All messages are preceded by an identifier, intended to be ASCII readable
struct GAMESYNC_API MessageIdentifier final
{
	MessageID mID = {};

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};

///////////////////////////////////////////////////////////////////////////////
}

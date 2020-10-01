#pragma once

#include "rftl/cstdint"


// Forwards
namespace RF::platform {
class IncomingBufferStitcher;
class OutgoingBufferStream;
}

namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

namespace ChannelFlags {
enum Value : uint8_t
{
	Invalid = 0,
	Reliable = 1 << 0,
	LowLateny = 1 << 1,
	Broadcast = 1 << 2,
};
}

// NOTE: Buffer stitcher used in place of buffer stream, as it extends the
//  interface with the most common usage pattern
using IncomingStream = platform::IncomingBufferStitcher;
using OutgoingStream = platform::OutgoingBufferStream;

using EndpointIdentifier = uint64_t;

struct ChannelSpec;

class LogicalEndpoint;
class EndpointManager;

///////////////////////////////////////////////////////////////////////////////
}

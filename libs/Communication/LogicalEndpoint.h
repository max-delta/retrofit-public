#pragma once
#include "project.h"

#include "Communication/ChannelSpec.h"

#include "core/ptr/weak_shared_ptr.h"

#include "rftl/shared_mutex"
#include "rftl/vector"


namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

class COMMUNICATION_API LogicalEndpoint
{
	RF_NO_COPY( LogicalEndpoint );

	//
	// Types and constants
private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	using IncomingChannel = rftl::pair<WeakSharedPtr<IncomingStream>, ChannelSpec>;
	using OutgoingChannel = rftl::pair<WeakSharedPtr<OutgoingStream>, ChannelSpec>;
	using IncomingChannels = rftl::vector<IncomingChannel>;
	using OutgoingChannels = rftl::vector<OutgoingChannel>;


	//
	// Public methods
public:
	void AddIncomingChannel( WeakSharedPtr<IncomingStream> const& stream, ChannelSpec const& spec );
	void AddOutgoingChannel( WeakSharedPtr<OutgoingStream> const& stream, ChannelSpec const& spec );

	bool HasIncomingStream( WeakPtr<IncomingStream const> const& stream ) const;
	bool HasOutgoingStream( WeakPtr<OutgoingStream const> const& stream ) const;

	void ChooseIncomingChannel( WeakSharedPtr<IncomingStream>& stream, ChannelSpec& spec, ChannelFlags::Value flags );
	void ChooseOutgoingChannel( WeakSharedPtr<OutgoingStream>& stream, ChannelSpec& spec, ChannelFlags::Value flags );


	//
	// Private data
private:
	mutable ReaderWriterMutex mChannelMutex;
	IncomingChannels mIncomingChannels;
	OutgoingChannels mOutgoingChannels;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "LogicalEndpoint.h"


namespace RF::comm {
///////////////////////////////////////////////////////////////////////////////

void LogicalEndpoint::AddIncomingChannel( WeakSharedPtr<IncomingStream> const& stream, ChannelSpec const& spec )
{
	WriterLock const lock( mChannelMutex );
	mIncomingChannels.emplace_back( stream, spec );
}



void LogicalEndpoint::AddOutgoingChannel( WeakSharedPtr<OutgoingStream> const& stream, ChannelSpec const& spec )
{
	WriterLock const lock( mChannelMutex );
	mOutgoingChannels.emplace_back( stream, spec );
}



bool LogicalEndpoint::HasIncomingStream( WeakPtr<IncomingStream const> const& stream ) const
{
	ReaderLock const lock( mChannelMutex );
	for( IncomingChannels::value_type const& channel : mIncomingChannels )
	{
		if( channel.first.Weaken() == stream )
		{
			return true;
		}
	}
	return false;
}



bool LogicalEndpoint::HasOutgoingStream( WeakPtr<OutgoingStream const> const& stream ) const
{
	ReaderLock const lock( mChannelMutex );
	for( OutgoingChannels::value_type const& channel : mOutgoingChannels )
	{
		if( channel.first.Weaken() == stream )
		{
			return true;
		}
	}
	return false;
}



void LogicalEndpoint::ChooseIncomingChannel( WeakSharedPtr<IncomingStream>& stream, ChannelSpec& spec, ChannelFlags::Value flags )
{
	stream = {};
	spec = {};

	ReaderLock const lock( mChannelMutex );

	if( mIncomingChannels.empty() )
	{
		return;
	}

	RF_TODO_ANNOTATION( "Implement" );
	RF_ASSERT( mIncomingChannels.size() == 1 );
	IncomingChannel const& channel = mIncomingChannels.front();
	stream = channel.first;
	spec = channel.second;
}



void LogicalEndpoint::ChooseOutgoingChannel( WeakSharedPtr<OutgoingStream>& stream, ChannelSpec& spec, ChannelFlags::Value flags )
{
	stream = {};
	spec = {};

	ReaderLock const lock( mChannelMutex );

	if( mOutgoingChannels.empty() )
	{
		return;
	}

	RF_TODO_ANNOTATION( "Implement" );
	RF_ASSERT( mOutgoingChannels.size() == 1 );
	OutgoingChannel const& channel = mOutgoingChannels.front();
	stream = channel.first;
	spec = channel.second;
}



void LogicalEndpoint::ChooseIncomingChannel( WeakSharedPtr<IncomingStream>& stream, ChannelFlags::Value flags )
{
	ChannelSpec unused = {};
	ChooseIncomingChannel( stream, unused, flags );
}



void LogicalEndpoint::ChooseOutgoingChannel( WeakSharedPtr<OutgoingStream>& stream, ChannelFlags::Value flags )
{
	ChannelSpec unused = {};
	ChooseOutgoingChannel( stream, unused, flags );
}

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameSync/protocol/Structure.h"

#include "core/meta/TypeList.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

struct GAMESYNC_API MsgHello final
{
	static constexpr MessageID kID = { 'H', 'N', 'D', '1' };
	static constexpr char const kDesc[] = "I'm a new client, I need session data";

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgWelcome final
{
	static constexpr MessageID kID = { 'H', 'N', 'D', '2' };
	static constexpr char const kDesc[] = "I'm your new host";

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};

///////////////////////////////////////////////////////////////////////////////

using AllMessages = TypeList<MsgHello>;

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"
#include "GameSync/protocol/Structure.h"

#include "GameInput/InputFwd.h"

#include "core/meta/TypeList.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

struct GAMESYNC_API MsgHello final
{
	static constexpr MessageID kID = { 'H', 'N', 'D', '1' };
	static constexpr char const kDesc[] = "I'm a new client, I need session data";

	EncryptionMode mModeRequest = EncryptionMode::kUnencrypted;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgWelcome final
{
	static constexpr MessageID kID = { 'H', 'N', 'D', '2' };
	static constexpr char const kDesc[] = "I'm your new host";

	EncryptionMode mModeChange = EncryptionMode::kUnencrypted;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgSessionList final
{
	static constexpr MessageID kID = { 'S', 'S', 'N', 'L' };
	static constexpr char const kDesc[] = "Here's the current session participants";

	ConnectionIdentifier mYourConnectionID = kInvalidConnectionIdentifier;

	using ConnectionEntry = rftl::pair<ConnectionIdentifier, input::PlayerID>;
	using ConnectionEntries = rftl::vector<ConnectionEntry>;
	ConnectionEntries mConnectionEntries;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgClaimPlayer final
{
	static constexpr MessageID kID = { 'P', 'L', 'Y', 'R' };
	static constexpr char const kDesc[] = "I want to claim/release this player";

	input::PlayerID mPlayerID = input::kInvalidPlayerID;
	bool mClaim = false;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};

///////////////////////////////////////////////////////////////////////////////

using AllMessages = TypeList<
	MsgHello,
	MsgWelcome,
	MsgSessionList,
	MsgClaimPlayer>;

///////////////////////////////////////////////////////////////////////////////
}

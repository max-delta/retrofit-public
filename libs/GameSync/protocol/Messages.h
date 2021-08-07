#pragma once
#include "project.h"

#include "GameSync/SyncFwd.h"
#include "GameSync/protocol/Structure.h"
#include "GameSync/RollbackInputPack.h"

#include "GameInput/InputFwd.h"

#include "core/meta/TypeList.h"

#include "rftl/extension/static_string.h"
#include "rftl/unordered_map"


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



struct GAMESYNC_API MsgChat final
{
	static constexpr MessageID kID = { 'C', 'H', 'A', 'T' };
	static constexpr char const kDesc[] = "Chat message text";

	RF_TODO_ANNOTATION( "Convert to static_u8string in C++20" );
	static constexpr size_t kMaxChars = std::numeric_limits<uint8_t>::max();
	using Text = rftl::static_string<kMaxChars>;
	Text mText;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgRollbackInputEvents final
{
	static constexpr MessageID kID = { 'R', 'B', 'I', 'E' };
	static constexpr char const kDesc[] = "Rollback inputs have happened";

	RollbackInputPack mInputPack = {};

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};

///////////////////////////////////////////////////////////////////////////////

// Helper for creating proxy messages
struct GAMESYNC_API MsgProxy
{
	ConnectionIdentifier mSourceConnectionID = kInvalidConnectionIdentifier;
};
template<typename Msg>
struct GAMESYNC_API MsgProxyT : public MsgProxy
{
	Msg mMsg;

	void Append( Buffer& bytes ) const;
	ReadResult TryRead( rftl::byte_view& bytes );
};



struct GAMESYNC_API MsgProxyChat final : public MsgProxyT<MsgChat>
{
	static constexpr MessageID kID = { 'P', 'X', 'C', 'H' };
	static constexpr char const kDesc[] = "PROXY Chat message text";
};

struct GAMESYNC_API MsgProxyRollbackInputEvents final : public MsgProxyT<MsgRollbackInputEvents>
{
	static constexpr MessageID kID = { 'P', 'X', 'R', 'E' };
	static constexpr char const kDesc[] = "PROXY Rollback inputs have happened";
};

///////////////////////////////////////////////////////////////////////////////
}

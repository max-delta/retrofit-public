#include "stdafx.h"
#include "DevTestLobby.h"

#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/input/HardcodedSetup.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameSync/SessionHostManager.h"
#include "GameSync/SessionClientManager.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"
#include "Localization/PageMapper.h"

#include "PlatformUtils_win32/ProcessLaunch.h"

#include "core_unicode/StringConvert.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestLobby::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	UniquePtr<sync::SessionHostManager> mAsHost;
	UniquePtr<sync::SessionClientManager> mAsClient;

	size_t mCursor = 0;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestLobby::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	//InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );

	input::HardcodedPlayerSetup( input::player::P1 );
	input::HardcodedPlayerSetup( input::player::P2 );
	InputHelpers::MakeRemote( input::player::P1 );
	InputHelpers::MakeRemote( input::player::P2 );
}



void DevTestLobby::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestLobby::OnTick( AppStateTickContext& context )
{
	InternalState& internalState = *mInternalState;
	gfx::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool //
	{
		gfx::PPUCoord const pos = gfx::PPUCoord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, -1, font.mFontHeight, font.mManagedFontID, false, math::Color3f::kWhite, fmt, args );
		va_end( args );
		return retVal;
	};


	drawText( 1, 1, "DEV TEST - LOBBY" );

	enum OptionID : size_t
	{
		kLaunchClone = 0,
		kStartHosting = 1,
		kConnectToHost = 2,
		kClaimAPlayer = 3,
		kRelinquishAPlayer = 4,
		kClearText = 5,
		kSendText = 6,
		kNumOptions
	};
	static constexpr char const* kOptionText[] = {
		"Launch clone",
		"Start hosting",
		"Connect to host",
		"Claim a player",
		"Relinquish a player",
		"Clear text",
		"Send text"
	};
	bool selected[kNumOptions] = {};

	size_t& cursor = internalState.mCursor;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		static constexpr size_t kMaxCursorPos = kNumOptions - 1;

		if( focusEvent == ui::focusevent::Command_NavigateUp )
		{
			cursor--;
		}
		else if( focusEvent == ui::focusevent::Command_NavigateDown )
		{
			cursor++;
		}
		cursor = cursor % ( kMaxCursorPos + 1 );
		cursor = math::Clamp<size_t>( 0, cursor, kMaxCursorPos );

		if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
		{
			selected[cursor] = true;
		}
	}

	if( selected[kLaunchClone] )
	{
		platform::process::LaunchSelfClone( true );
	}
	else if( selected[kStartHosting] )
	{
		// TODO: Host
		sync::SessionHostManager::HostSpec const spec{ true, true, 8271, { input::player::P1, input::player::P2 } };
		internalState.mAsHost = DefaultCreator<sync::SessionHostManager>::Create( spec );
		sync::SessionHostManager& asHost = *internalState.mAsHost;
		asHost.StartHostingASession();
	}
	else if( selected[kConnectToHost] )
	{
		// TODO: Connect
		sync::SessionClientManager::ClientSpec const kSpec{ "localhost", 8271 };
		internalState.mAsClient = DefaultCreator<sync::SessionClientManager>::Create(
			kSpec );
		sync::SessionClientManager& asClient = *internalState.mAsClient;
		asClient.StartReceivingASession();
	}
	else if( selected[kClaimAPlayer] )
	{
		static constexpr auto getUnclaimed = []( sync::SessionManager const& manager ) -> input::PlayerID //
		{
			sync::SessionMembers::PlayerIDs const ids = manager.GetSessionMembers().GetUnclaimedPlayerIDs();
			RF_ASSERT( ids.empty() == false );
			return *ids.begin();
		};

		if( internalState.mAsHost != nullptr )
		{
			sync::SessionHostManager& host = *internalState.mAsHost;
			host.AttemptPlayerChange( getUnclaimed( host ), true );
		}
		if( internalState.mAsClient != nullptr )
		{
			sync::SessionClientManager& client = *internalState.mAsClient;
			client.RequestPlayerChange( getUnclaimed( client ), true );
		}
	}
	else if( selected[kRelinquishAPlayer] )
	{
		static constexpr auto getLocal = []( sync::SessionManager const& manager ) -> input::PlayerID //
		{
			sync::SessionMembers::PlayerIDs const ids = manager.GetSessionMembers().GetLocalPlayerIDs();
			RF_ASSERT( ids.empty() == false );
			return *ids.begin();
		};

		if( internalState.mAsHost != nullptr )
		{
			sync::SessionHostManager& host = *internalState.mAsHost;
			host.AttemptPlayerChange( getLocal( host ), false );
		}
		if( internalState.mAsClient != nullptr )
		{
			sync::SessionClientManager& client = *internalState.mAsClient;
			client.RequestPlayerChange( getLocal( client ), false );
		}
	}
	else if( selected[kClearText] )
	{
		InputHelpers::ClearMainMenuTextBuffer();
	}
	else if( selected[kSendText] )
	{
		rftl::string text = unicode::ConvertToUtf8( InputHelpers::GetMainMenuUnicodeTextBuffer( 32 ) );
		if( internalState.mAsHost != nullptr )
		{
			internalState.mAsHost->QueueOutgoingChatMessage( rftl::move( text ) );
		}
		if( internalState.mAsClient != nullptr )
		{
			internalState.mAsClient->QueueOutgoingChatMessage( rftl::move( text ) );
		}
		InputHelpers::ClearMainMenuTextBuffer();
	}

	uint8_t x;
	uint8_t y;

	// Draw options
	x = 2;
	y = 2;
	for( size_t i = 0; i < kNumOptions; i++ )
	{
		if( cursor == i )
		{
			drawText( x, y++, "*%s", kOptionText[i] );
		}
		else
		{
			drawText( x, y++, " %s", kOptionText[i] );
		}
	}

	// Process
	static constexpr auto updateControllers = []( sync::SessionMembers const& members ) -> void //
	{
		using PlayerIDs = sync::SessionMembers::PlayerIDs;
		PlayerIDs const all = members.GetPlayerIDs();
		PlayerIDs const local = members.GetLocalPlayerIDs();
		for( input::PlayerID const& id : all )
		{
			bool const isLocal = local.count( id ) > 0;
			if( isLocal )
			{
				InputHelpers::MakeLocal( id );
			}
			else
			{
				InputHelpers::MakeRemote( id );
			}
		}
	};
	if( internalState.mAsHost != nullptr )
	{
		internalState.mAsHost->ProcessPendingOperations();
		updateControllers( internalState.mAsHost->GetSessionMembers() );
	}
	if( internalState.mAsClient != nullptr )
	{
		internalState.mAsClient->ProcessPendingOperations();
		updateControllers( internalState.mAsClient->GetSessionMembers() );
	}

	// Draw Status
	x = 30;
	y = 2;
	auto const drawSessionMembersText = [&y, &drawText]( uint8_t x, sync::SessionMembers const& members ) -> void //
	{
		drawText( x, y++, "MEMB:" );
		sync::ConnectionIdentifier const& self = members.mLocalConnection;
		auto const drawEntry =
			[&y, &drawText, &self](
				uint8_t x,
				sync::ConnectionIdentifier conn,
				sync::SessionMembers::PlayerIDs players ) -> void //
		{
			rftl::stringstream ss;
			bool init = true;
			for( input::PlayerID const& id : players )
			{
				if( init == false )
				{
					ss << ' ';
				}
				ss << math::integer_cast<uint16_t>( id );
				init = false;
			}
			if( conn == sync::kInvalidConnectionIdentifier )
			{
				drawText( x + 4u, y++, "??? p[%s]", ss.str().c_str() );
			}
			else if( conn == self )
			{
				drawText( x + 4u, y++, "*%-2llu p[%s]", conn, ss.str().c_str() );
			}
			else
			{
				drawText( x + 4u, y++, "-%-2llu p[%s]", conn, ss.str().c_str() );
			}
		};
		sync::SessionMembers::ConnectionPlayerIDs const connEntries = members.GetConnectionPlayerIDs();
		for( sync::SessionMembers::ConnectionPlayerIDs::value_type const& connEntry : connEntries )
		{
			drawEntry( x + 4u, connEntry.first, connEntry.second );
		}
		sync::SessionMembers::PlayerIDs const unclaimed = members.GetUnclaimedPlayerIDs();
		if( unclaimed.empty() == false )
		{
			drawEntry( x + 4u, sync::kInvalidConnectionIdentifier, unclaimed );
		}
	};
	if( internalState.mAsHost != nullptr )
	{
		using namespace sync;

		drawText( x - 4u, y++, "HOST" );
		SessionHostManager& host = *internalState.mAsHost;

		SessionHostManager::Diagnostics const diag = host.ReportDiagnostics();
		drawText( x, y++, "CONN: %llu (+%llu) / %llu",
			diag.mValidConnections,
			diag.mInvalidConnections,
			SessionHostManager::kMaxConnectionCount );
		drawSessionMembersText( x, host.GetSessionMembers() );
	}
	if( internalState.mAsClient != nullptr )
	{
		using namespace sync;

		drawText( x - 4u, y++, "CLIENT" );
		SessionClientManager& client = *internalState.mAsClient;

		SessionClientManager::Diagnostics const diag = client.ReportDiagnostics();
		drawText( x, y++, "CONN: %llu (+%llu)",
			diag.mValidConnections,
			diag.mInvalidConnections );
		drawSessionMembersText( x, client.GetSessionMembers() );
	}

	// Draw chat log
	x = 2;
	y = 15;
	auto const drawChatLog = [x, &y, &drawText]( sync::SessionManager::ChatMessages const& messages ) -> void //
	{
		drawText( x, y++, "CHAT:" );
		for( sync::SessionManager::ChatMessage const& message : messages )
		{
			rftl::string const textBuffer = app::gPageMapper->MapTo8Bit(
				unicode::ConvertToUtf32( message.mText ) );
			drawText( x + 4u, y++, "%-2llu: \"%s\"",
				message.mSourceConnectionID,
				textBuffer.c_str() );
		}
	};
	if( internalState.mAsHost != nullptr )
	{
		sync::SessionHostManager const& host = *internalState.mAsHost;
		drawChatLog( host.GetRecentChatMessages( 5 ) );
	}
	if( internalState.mAsClient != nullptr )
	{
		sync::SessionClientManager const& client = *internalState.mAsClient;
		drawChatLog( client.GetRecentChatMessages( 5 ) );
	}

	// Draw text buffer
	x = 2;
	y = 21;
	{
		rftl::string const textBuffer = InputHelpers::GetMainMenuPageMappedTextBuffer( 32 );
		drawText( x, y++, "TEXT BUFFER: [%s]", textBuffer.c_str() );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "DevTestLobby.h"

#include "cc3o3/cc3o3.h" // HACK: Time reset
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/state/StateFwd.h"
#include "cc3o3/sync/Session.h"
#include "cc3o3/Common.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameSync/SessionHostManager.h"
#include "GameSync/SessionClientManager.h"
#include "GameUI/FontRegistry.h"

#include "Localization/PageMapper.h"
#include "PPU/PPUController.h"
#include "Rollback/AutoVar.h"
#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"

#include "PlatformUtils_win32/ProcessLaunch.h"

#include "core_allocate/Allocator.h"
#include "core_allocate/LinearAllocator.h"
#include "core_unicode/StringConvert.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/virtual_iterator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct DevTestLobby::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	void Bind( rollback::Window& window, state::VariableIdentifier const& parent )
	{
		mP1.Bind( window, parent.GetChild( "p1" ), mAlloc );
		mP2.Bind( window, parent.GetChild( "p2" ), mAlloc );
	}

	// NOTE: Must be before vars so it destructs last
	alloc::AllocatorT<alloc::LinearAllocator<2048>> mAlloc{ ExplicitDefaultConstruct() };

	struct Pos
	{
		RF_NO_COPY( Pos );
		Pos() = default;

		void Bind( rollback::Window& window, state::VariableIdentifier const& parent, alloc::Allocator& allocator )
		{
			mX.Bind( window, parent.GetChild( "x" ), allocator );
			mY.Bind( window, parent.GetChild( "y" ), allocator );
			mXNeg.Bind( window, parent.GetChild( "xn" ), allocator );
			mXPos.Bind( window, parent.GetChild( "xp" ), allocator );

			// HACK: Ensure values are set from zero onwards
			RF_TODO_ANNOTATION( "Set up proper session time logic so these are scoped to the proper times" );
			mX.Write( {}, {} );
			mY.Write( {}, {} );
			mXNeg.Write( {}, {} );
			mXPos.Write( {}, {} );
		}

		rollback::AutoVar<gfx::ppu::CoordElem> mX;
		rollback::AutoVar<gfx::ppu::CoordElem> mY;
		rollback::AutoVar<bool> mXNeg;
		rollback::AutoVar<bool> mXPos;
	};
	Pos mP1;
	Pos mP2;

	size_t mCursor = 0;
};

///////////////////////////////////////////////////////////////////////////////

void DevTestLobby::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ppu.DebugSetBackgroundColor( { 0.f, 0.f, 1.f } );

	input::HardcodedPlayerSetup( input::player::P1 );
	input::HardcodedPlayerSetup( input::player::P2 );
	InputHelpers::MakeRemote( input::player::P1 );
	InputHelpers::MakeRemote( input::player::P2 );

	// Set up mini-game
	{
		rollback::RollbackManager& rollMan = *gRollbackManager;
		rollback::Window& window = rollMan.GetMutableSharedDomain().GetMutableWindow();
		internalState.Bind( window, state::VariableIdentifier( "DevTest", "Lobby" ) );
	}

	// HACK: Time reset to get all clients into same frame
	// TODO: Use save/load snapshot machinery instead on host-triggered sync
	DebugHardTimeReset();
}



void DevTestLobby::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void DevTestLobby::OnTick( AppStateTickContext& context )
{
	using sync::SessionManager;
	using sync::SessionHostManager;
	using sync::SessionClientManager;
	using sync::SessionMembers;
	using sync::ConnectionIdentifier;

	InternalState& internalState = *mInternalState;
	gfx::ppu::PPUController& ppu = *app::gGraphics;


	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, char const* fmt, ... ) -> bool //
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
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
		SessionHostManager::HostSpec const spec{ true, true, 8271, { input::player::P1, input::player::P2 } };
		sync::SetSessionHostManager( DefaultCreator<SessionHostManager>::Create( spec ) );
		sync::gSessionHostManager->StartHostingASession();
	}
	else if( selected[kConnectToHost] )
	{
		// TODO: Connect
		SessionClientManager::ClientSpec const kSpec{ "localhost", 8271 };
		sync::SetSessionClientManager( DefaultCreator<SessionClientManager>::Create( kSpec ) );
		sync::gSessionClientManager->StartReceivingASession();
	}
	else if( selected[kClaimAPlayer] )
	{
		static constexpr auto getUnclaimed = []( SessionManager const& manager ) -> input::PlayerID //
		{
			SessionMembers::PlayerIDs const ids = manager.GetSessionMembers().GetUnclaimedPlayerIDs();
			RF_ASSERT( ids.empty() == false );
			return *ids.begin();
		};

		if( sync::gSessionHostManager != nullptr )
		{
			SessionHostManager& host = *sync::gSessionHostManager;
			host.AttemptPlayerChange( getUnclaimed( host ), true );
		}
		if( sync::gSessionClientManager != nullptr )
		{
			SessionClientManager& client = *sync::gSessionClientManager;
			client.RequestPlayerChange( getUnclaimed( client ), true );
		}
	}
	else if( selected[kRelinquishAPlayer] )
	{
		static constexpr auto getLocal = []( SessionManager const& manager ) -> input::PlayerID //
		{
			SessionMembers::PlayerIDs const ids = manager.GetSessionMembers().GetLocalPlayerIDs();
			RF_ASSERT( ids.empty() == false );
			return *ids.begin();
		};

		if( sync::gSessionHostManager != nullptr )
		{
			SessionHostManager& host = *sync::gSessionHostManager;
			host.AttemptPlayerChange( getLocal( host ), false );
		}
		if( sync::gSessionClientManager != nullptr )
		{
			SessionClientManager& client = *sync::gSessionClientManager;
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
		if( sync::gSessionManager != nullptr )
		{
			sync::gSessionManager->QueueOutgoingChatMessage( rftl::move( text ) );
		}
		InputHelpers::ClearMainMenuTextBuffer();
	}

	// Process mini-game
	{
		static constexpr input::PlayerID kPlayerIDs[] = {
			input::player::P1,
			input::player::P2,
		};
		InternalState::Pos* const positions[] = {
			&internalState.mP1,
			&internalState.mP2,
		};

		for( size_t i = 0; i < 2; i++ )
		{
			InternalState::Pos& pos = *positions[i];
			input::GameController const& controller =
				*app::gInputControllerManager->GetGameController( kPlayerIDs[i], input::layer::CharacterControl );

			// Fetch commands that were entered for this current frame
			rftl::vector<input::GameCommand> commands;
			rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
			controller.GetGameCommandStream( parser, time::FrameClock::now(), time::FrameClock::now() );

			// Process commands
			for( input::GameCommand const& command : commands )
			{
				if( command.mType == input::command::game::WalkWest )
				{
					pos.mXNeg = true;
				}
				else if( command.mType == input::command::game::WalkWestStop )
				{
					pos.mXNeg = false;
				}
				else if( command.mType == input::command::game::WalkEast )
				{
					pos.mXPos = true;
				}
				else if( command.mType == input::command::game::WalkEastStop )
				{
					pos.mXPos = false;
				}
			}

			// Apply movement
			if( pos.mXNeg )
			{
				pos.mX -= 1;
			}
			if( pos.mXPos )
			{
				pos.mX += 1;
			}
			pos.mX = math::Clamp<gfx::ppu::CoordElem>( gfx::ppu::kTileSize, pos.mX, gfx::ppu::kDesiredWidth - gfx::ppu::kTileSize );
			pos.mY = math::Clamp<gfx::ppu::CoordElem>( gfx::ppu::kDesiredHeight - gfx::ppu::kTileSize, pos.mY, gfx::ppu::kDesiredHeight - gfx::ppu::kTileSize );
		}
	}

	// Draw mini-game
	{
		InternalState::Pos const* const positions[] = {
			&internalState.mP1,
			&internalState.mP2
		};

		math::Color3f const colors[] = {
			math::Color3f::kYellow,
			math::Color3f::kWhite
		};

		for( size_t i = 0; i < 2; i++ )
		{
			InternalState::Pos const& pos = *positions[i];
			math::Color3f const& color = colors[i];

			gfx::ppu::Coord const renderPos = gfx::ppu::Coord( pos.mX, pos.mY );
			ppu.DebugDrawAuxText( renderPos, -1, font.mFontHeight, font.mManagedFontID, false, color, "#" );
		}
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

	// Draw Status
	x = 30;
	y = 2;
	auto const drawSessionMembersText = [&y, &drawText]( uint8_t x, SessionMembers const& members ) -> void //
	{
		drawText( x, y++, "MEMB:" );
		ConnectionIdentifier const& self = members.mLocalConnection;
		auto const drawEntry =
			[&y, &drawText, &self](
				uint8_t x,
				ConnectionIdentifier conn,
				SessionMembers::PlayerIDs players ) -> void //
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
		SessionMembers::ConnectionPlayerIDs const connEntries = members.GetConnectionPlayerIDs();
		for( SessionMembers::ConnectionPlayerIDs::value_type const& connEntry : connEntries )
		{
			drawEntry( x + 4u, connEntry.first, connEntry.second );
		}
		SessionMembers::PlayerIDs const unclaimed = members.GetUnclaimedPlayerIDs();
		if( unclaimed.empty() == false )
		{
			drawEntry( x + 4u, sync::kInvalidConnectionIdentifier, unclaimed );
		}
	};
	if( sync::gSessionHostManager != nullptr )
	{
		drawText( x - 4u, y++, "HOST" );
		SessionHostManager& host = *sync::gSessionHostManager;

		SessionHostManager::Diagnostics const diag = host.ReportDiagnostics();
		drawText( x, y++, "CONN: %llu (+%llu) / %llu",
			diag.mValidConnections,
			diag.mInvalidConnections,
			SessionHostManager::kMaxConnectionCount );
	}
	if( sync::gSessionClientManager != nullptr )
	{
		drawText( x - 4u, y++, "CLIENT" );
		SessionClientManager& client = *sync::gSessionClientManager;

		SessionClientManager::Diagnostics const diag = client.ReportDiagnostics();
		drawText( x, y++, "CONN: %llu (+%llu)",
			diag.mValidConnections,
			diag.mInvalidConnections );
	}
	if( sync::gSessionManager != nullptr )
	{
		drawSessionMembersText( x, sync::gSessionManager->GetSessionMembers() );
	}

	// Draw chat log
	x = 2;
	y = 15;
	auto const drawChatLog = [x, &y, &drawText]( SessionManager::ChatMessages const& messages ) -> void //
	{
		drawText( x, y++, "CHAT:" );
		for( SessionManager::ChatMessage const& message : messages )
		{
			rftl::string const textBuffer = app::gPageMapper->MapTo8Bit(
				unicode::ConvertToUtf32( message.mText ) );
			drawText( x + 4u, y++, "%-2llu: \"%s\"",
				message.mSourceConnectionID,
				textBuffer.c_str() );
		}
	};
	if( sync::gSessionManager != nullptr )
	{
		drawChatLog( sync::gSessionManager->GetRecentChatMessages( 5 ) );
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

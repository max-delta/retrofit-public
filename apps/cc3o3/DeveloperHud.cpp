#include "stdafx.h"
#include "DeveloperHud.h"

#include "cc3o3.h"
#include "cc3o3/input/InputFwd.h"
#include "cc3o3/time/TimeFwd.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"

#include "core_math/Lerp.h"


namespace RF { namespace cc { namespace developer {
///////////////////////////////////////////////////////////////////////////////

enum class Mode : uint8_t
{
	Rollback = 0,

	NumModes
};
static constexpr size_t kNumModes = static_cast<size_t>( Mode::NumModes );

static bool sDisplayHud = false;
static Mode sCurrentMode = Mode::Rollback;
static constexpr char kSnapshotName[] = "DEV1";

///////////////////////////////////////////////////////////////////////////////
namespace mode {

void ProcessRollback( RF::input::GameCommand const& command )
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;

	switch( command.mType )
	{
		case input::command::game::DeveloperAction1:
		{
			rollMan.TakeManualSnapshot( kSnapshotName, time::FrameClock::now() );
			break;
		}
		case input::command::game::DeveloperAction2:
		{
			break;
		}
		case input::command::game::DeveloperAction3:
		{
			time::CommonClock::time_point const time = rollMan.LoadManualSnapshot( kSnapshotName );
			rollMan.SetHeadClock( time );
			break;
		}
		default:
			break;
	}
}



void RenderRollback()
{
	using namespace rftl::chrono;
	using rollback::RollbackManager;

	gfx::PPUController& ppu = *app::gGraphics;
	RollbackManager const& rollMan = *app::gRollbackManager;

	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	if( font.mManagedFontID == gfx::kInvalidManagedFontID )
	{
		// No font (not even a backup), so we may still be booting
		ppu.DebugDrawText( gfx::PPUCoord( 16, 16 ), "No font loaded for developer hud" );
		return;
	}
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, ... ) -> bool {
		gfx::PPUCoord const pos = gfx::PPUCoord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, gfx::kNearestLayer, font.mFontHeight, font.mManagedFontID, true, color, fmt, args );
		va_end( args );
		return retVal;
	};

	static constexpr uint8_t kStartX = 4;
	static constexpr uint8_t kStartY = 1;
	uint8_t x = kStartX;
	uint8_t y = kStartY;

	static constexpr auto timeAsIndex = []( time::CommonClock::time_point const& time ) -> size_t {
		return math::integer_cast<size_t>( duration_cast<nanoseconds>( time.time_since_epoch() ) / duration_cast<nanoseconds>( time::kSimulationFrameDuration ) );
	};

	time::CommonClock::time_point const currentTime = time::FrameClock::now();
	drawText( x, y, math::Color3f::kMagenta, "FRM: %llu", timeAsIndex( currentTime ) );
	y++;

	time::CommonClock::time_point const snapshotTime = rollMan.GetSharedDomain().GetManualSnapshot( kSnapshotName )->first;
	drawText( x, y, math::Color3f::kMagenta, "SNP: %llu", timeAsIndex( snapshotTime ) );
	y++;

	SimulationMode const simMode = DebugGetPreviousFrameSimulationMode();
	switch( simMode )
	{
		case SimulationMode::OnRailsReplay:
			drawText( x, y, math::Color3f::kBlue, "SIM: REPLAY" );
			break;
		case SimulationMode::SingleFrameSimulate:
			drawText( x, y, math::Color3f::kGreen, "SIM: PREDICT" );
			break;
		case SimulationMode::RollbackAndSimulate:
			drawText( x, y, math::Color3f::kYellow, "SIM: ROLLBACK" );
			break;
		case SimulationMode::StallSimulation:
			drawText( x, y, math::Color3f::kRed, "SIM: STALL" );
			break;
		case SimulationMode::Invalid:
		default:
			drawText( x, y, math::Color3f::kRed, "SIM: UNKNOWN" );
			break;
	}
	y++;

	// Timeline
	RollbackManager::InputStreams const& committedStreams = rollMan.GetCommittedStreams();
	if( committedStreams.empty() == false )
	{
		RollbackManager::InputStreams const& uncommittedStreams = rollMan.GetUncommittedStreams();
		RF_ASSERT( committedStreams.size() == uncommittedStreams.size() );
		size_t const numStreams = committedStreams.size();

		// Timeline 'now' is defined by the head clock
		time::CommonClock::time_point const timelineNow = rollMan.GetHeadClock();

		// Timeline starts before now
		time::CommonClock::time_point const timelineStart = timelineNow - seconds( 1 );

		// Timeline commit starts at any committed streams commit time
		// NOTE: All committed streams should share the same commit time
		time::CommonClock::time_point const timelineCommit = math::Max( timelineStart, committedStreams.begin()->second.back().mTime );

		// Timeline ends at one full window away from now
		time::CommonClock::time_point const timelineEnd = timelineNow + time::kSimulationFrameDuration * rollback::kMaxChangesInWindow;

		// Will need to lerp data into a graphical representation
		gfx::PPUCoord::ElementType const gfxTimelineStart = gfx::kTileSize;
		gfx::PPUCoord::ElementType const gfxTimelineEnd = gfx::kDesiredWidth - gfx::kTileSize;
		size_t const gfxNumLanes = numStreams;
		gfx::PPUCoord::ElementType const gfxLanesStart = gfx::kTileSize * 1;
		gfx::PPUCoord::ElementType const gfxLanesHeight = 4;
		gfx::PPUCoord::ElementType const gfxLanesEnd =
			gfxLanesStart +
			math::integer_cast<gfx::PPUCoord::ElementType>(
				gfxLanesHeight * gfxNumLanes );

		auto const rescaleToGfx = [&]( time::CommonClock::time_point const& time ) -> gfx::PPUCoord::ElementType {
			return math::Rescale(
				gfxTimelineStart,
				gfxTimelineEnd,
				timelineStart.time_since_epoch().count(),
				timelineEnd.time_since_epoch().count(),
				time.time_since_epoch().count() );
		};

		static constexpr gfx::PPUDepthLayer kBorderDepth = gfx::kNearestLayer + 5;
		static constexpr gfx::PPUDepthLayer kNowDepth = gfx::kNearestLayer + 4;
		static constexpr gfx::PPUDepthLayer kCommitDepth = gfx::kNearestLayer + 3;
		static constexpr gfx::PPUDepthLayer kEventDepth = gfx::kNearestLayer + 2;

		// Border
		ppu.DebugDrawAABB(
			{ gfxTimelineStart, gfxLanesStart, gfxTimelineEnd, gfxLanesEnd },
			1,
			kBorderDepth,
			math::Color3f::kGray25 );

		// Now line
		gfx::PPUCoord::ElementType const gfxTimelineNow = rescaleToGfx( timelineNow );
		ppu.DebugDrawLine(
			{ gfxTimelineNow, gfxLanesStart },
			{ gfxTimelineNow, gfxLanesEnd },
			1,
			kNowDepth,
			math::Color3f::kGray50 );

		// Commit line
		gfx::PPUCoord::ElementType const gfxTimelineCommit = rescaleToGfx( timelineCommit );
		ppu.DebugDrawLine(
			{ gfxTimelineCommit, gfxLanesStart },
			{ gfxTimelineCommit, gfxLanesEnd },
			1,
			kCommitDepth,
			timelineCommit < timelineNow ?
				math::Color3f::kYellow :
				math::Color3f::kGreen );

		size_t i_lane = 0;
		for( RollbackManager::InputStreams::value_type const& streamPair : committedStreams )
		{
			rollback::InputStream const& committedStream = streamPair.second;
			rollback::InputStream const& uncommittedStream = uncommittedStreams.at( streamPair.first );

			bool hasDrawnAFarFutureEvent = false;
			auto const onEvent = [&]( rollback::InputEvent const& event, math::Color3f const& color, bool renderInvalid ) -> void {
				if( event.mValue == rollback::kInvalidInputValue && renderInvalid == false )
				{
					return;
				}
				time::CommonClock::time_point eventTime = event.mTime;
				if( eventTime < timelineStart )
				{
					return;
				}
				if( eventTime > timelineEnd )
				{
					if( hasDrawnAFarFutureEvent )
					{
						return;
					}
					else
					{
						eventTime = timelineEnd;
						hasDrawnAFarFutureEvent = true;
					}
				}
				gfx::PPUCoord::ElementType const gfxTimelineEvent = rescaleToGfx( event.mTime );
				ppu.DebugDrawLine(
					{ gfxTimelineEvent, math::integer_cast<gfx::PPUCoord::ElementType>( gfxLanesStart + ( gfxLanesHeight * ( math::integer_cast<gfx::PPUCoord::ElementType>( i_lane ) + 0 ) ) ) },
					{ gfxTimelineEvent, math::integer_cast<gfx::PPUCoord::ElementType>( gfxLanesStart + ( gfxLanesHeight * ( math::integer_cast<gfx::PPUCoord::ElementType>( i_lane ) + 1 ) ) ) },
					1,
					kEventDepth,
					color );
			};

			for( rollback::InputEvent const& event : committedStream )
			{
				onEvent( event, math::Color3f::kWhite, false );
			}
			for( rollback::InputEvent const& event : uncommittedStream )
			{
				onEvent( event, math::Color3f::kGray75, false );
			}
			rollback::InputEvent const& mostFutureEvent = uncommittedStream.back();
			if( mostFutureEvent.mValue == rollback::kInvalidInputValue )
			{
				if( mostFutureEvent.mTime != timelineCommit )
				{
					if( mostFutureEvent.mTime > timelineNow )
					{
						onEvent( uncommittedStream.back(), math::Color3f::kGreen, true );
					}
					else if( mostFutureEvent.mTime < timelineNow )
					{
						onEvent( uncommittedStream.back(), math::Color3f::kRed, true );
					}
					else
					{
						onEvent( uncommittedStream.back(), math::Color3f::kYellow, true );
					}
				}
			}

			i_lane++;
		}
	}
}

}
///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	rollMan.TakeManualSnapshot( kSnapshotName, time::FrameClock::now() );
}



void ProcessInput()
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	WeakPtr<input::GameController> const controller = controllerManager.GetGameController( input::player::P1, input::layer::Developer );
	if( controller == nullptr )
	{
		// No controller, so we may still be booting
		return;
	}

	// Fetch commands that were entered for this current frame
	rftl::vector<input::GameCommand> commands;
	rftl::virtual_back_inserter_iterator<input::GameCommand, decltype( commands )> parser( commands );
	controller->GetGameCommandStream( parser, time::FrameClock::now(), time::FrameClock::now() );

	for( RF::input::GameCommand const& command : commands )
	{
		switch( command.mType )
		{
			case input::command::game::DeveloperToggle:
			{
				sDisplayHud = !sDisplayHud;
				break;
			}
			case input::command::game::DeveloperCycle:
			{
				sCurrentMode = static_cast<Mode>( ( static_cast<uint8_t>( sCurrentMode ) + 1 ) % kNumModes );
				break;
			}
			case input::command::game::DeveloperAction1:
			case input::command::game::DeveloperAction2:
			case input::command::game::DeveloperAction3:
			{
				switch( sCurrentMode )
				{
					case Mode::Rollback:
						mode::ProcessRollback( command );
						break;
					case Mode::NumModes:
					default:
						RF_DBGFAIL();
						break;
				}
				break;
			}
			default:
				break;
		}
	}
}



void RenderHud()
{
	if( sDisplayHud == false )
	{
		return;
	}

	switch( sCurrentMode )
	{
		case Mode::Rollback:
			mode::RenderRollback();
			return;
		case Mode::NumModes:
		default:
			RF_DBGFAIL();
			return;
	}
}



void Shutdown()
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}}

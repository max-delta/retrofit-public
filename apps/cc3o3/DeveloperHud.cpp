#include "stdafx.h"
#include "DeveloperHud.h"

#include "cc3o3.h"
#include "cc3o3/Common.h"
#include "cc3o3/appstates/InitialLoading.h"
#include "cc3o3/input/HardcodedSetup.h"
#include "cc3o3/time/TimeFwd.h"
#include "cc3o3/ui/UIFwd.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameInput/ControllerManager.h"
#include "GameInput/GameController.h"
#include "GameInput/RawController.h"
#include "GameUI/FontRegistry.h"

#include "PPU/PPUController.h"

#include "Localization/PageMapper.h"
#include "Rollback/RollbackManager.h"
#include "Timing/FrameClock.h"

#include "core_input/InputDevice.h"
#include "core_math/Lerp.h"
#include "core_unicode/StringConvert.h"


namespace RF::cc::developer {
///////////////////////////////////////////////////////////////////////////////

enum class Mode : uint8_t
{
	Rollback = 0,
	Reload,
	InputDevice,

	NumModes
};
static constexpr size_t kNumModes = static_cast<size_t>( Mode::NumModes );

static bool sDisplayHud = false;
static Mode sCurrentMode = Mode::Rollback;
static constexpr char kSnapshotName[] = "DEV1";
static size_t sInputDeviceIndex = 0;

///////////////////////////////////////////////////////////////////////////////
namespace mode {

void ProcessRollback( RF::input::GameCommand const& command )
{
	switch( command.mType )
	{
		case input::command::game::DeveloperAction1:
		{
			DebugTakeSnapshot( kSnapshotName );
			break;
		}
		case input::command::game::DeveloperAction2:
		{
			break;
		}
		case input::command::game::DeveloperAction3:
		{
			DebugLoadSnapshot( kSnapshotName );
			break;
		}
		case input::command::game::DeveloperAction4:
		{
			DebugInstantReplay( rollback::kMaxChangesInWindow );
			break;
		}
		case input::command::game::DeveloperAction5:
		{
			DebugHardTimeReset();
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

	gfx::ppu::PPUController& ppu = *app::gGraphics;
	RollbackManager const& rollMan = *gRollbackManager;

	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	if( font.mManagedFontID == gfx::kInvalidManagedFontID )
	{
		// No font (not even a backup), so we may still be booting
		ppu.DebugDrawText( gfx::ppu::Coord( 16, 16 ), "No font loaded for developer hud" );
		return;
	}
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, ... ) -> bool
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, gfx::ppu::kNearestLayer, font.mFontHeight, font.mManagedFontID, true, color, fmt, args );
		va_end( args );
		return retVal;
	};

	static constexpr uint8_t kStartX = 4;
	static constexpr uint8_t kStartY = 1;
	uint8_t x = kStartX;
	uint8_t y = kStartY;

	static constexpr auto timeAsIndex = []( time::CommonClock::time_point const& time ) -> size_t
	{
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
		time::CommonClock::time_point const timelineStart = timelineNow - ( time::kSimulationFrameDuration * 60 );

		// Timeline commit starts at any committed streams commit time
		// NOTE: All committed streams should share the same commit time
		time::CommonClock::time_point const timelineCommit = math::Max( timelineStart, committedStreams.begin()->second.back().mTime );

		// Timeline ends at one full window away from now
		time::CommonClock::time_point const timelineEnd = timelineNow + time::kSimulationFrameDuration * rollback::kMaxChangesInWindow;

		rollback::Window const& sharedWindow = rollMan.GetSharedDomain().GetWindow();
		rollback::Window const& privateWindow = rollMan.GetPrivateDomain().GetWindow();
		rftl::optional<rollback::InclusiveTimeRange> const sharedWindowStart = sharedWindow.GetEarliestTimes();
		rftl::optional<rollback::InclusiveTimeRange> const sharedWindowEnd = sharedWindow.GetLatestTimes();
		rftl::optional<rollback::InclusiveTimeRange> const privateWindowStart = privateWindow.GetEarliestTimes();
		rftl::optional<rollback::InclusiveTimeRange> const privateWindowEnd = privateWindow.GetLatestTimes();

		// Will need to lerp data into a graphical representation
		gfx::ppu::Coord::ElementType const gfxTimelineStart = gfx::ppu::kTileSize;
		gfx::ppu::Coord::ElementType const gfxTimelineEnd = gfx::ppu::kDesiredWidth - gfx::ppu::kTileSize;
		size_t const gfxNumLanes = numStreams;
		gfx::ppu::Coord::ElementType const gfxLanesStart = gfx::ppu::kTileSize * 1;
		gfx::ppu::Coord::ElementType const gfxLanesHeight = 4;
		gfx::ppu::Coord::ElementType const gfxLanesEnd =
			gfxLanesStart +
			math::integer_cast<gfx::ppu::Coord::ElementType>(
				gfxLanesHeight * gfxNumLanes );
		gfx::ppu::Coord::ElementType const gfxAnnotationStart = gfxLanesEnd + 2;
		gfx::ppu::Coord::ElementType const gfxAnnotationHeight = 4;

		auto const rescaleToGfx = [&]( time::CommonClock::time_point const& time ) -> gfx::ppu::Coord::ElementType
		{
			return math::Rescale(
				gfxTimelineStart,
				gfxTimelineEnd,
				timelineStart.time_since_epoch().count(),
				timelineEnd.time_since_epoch().count(),
				time.time_since_epoch().count() );
		};

		static constexpr gfx::ppu::DepthLayer kBorderDepth = gfx::ppu::kNearestLayer + 5;
		static constexpr gfx::ppu::DepthLayer kNowDepth = gfx::ppu::kNearestLayer + 4;
		static constexpr gfx::ppu::DepthLayer kCommitDepth = gfx::ppu::kNearestLayer + 3;
		static constexpr gfx::ppu::DepthLayer kEventDepth = gfx::ppu::kNearestLayer + 2;
		static constexpr gfx::ppu::DepthLayer kAnnotationDepth = gfx::ppu::kNearestLayer + 4;

		// Border
		ppu.DebugDrawAABB(
			{ gfxTimelineStart, gfxLanesStart, gfxTimelineEnd, gfxLanesEnd },
			1,
			kBorderDepth,
			math::Color3f::kGray25 );

		// Now line
		gfx::ppu::Coord::ElementType const gfxTimelineNow = rescaleToGfx( timelineNow );
		ppu.DebugDrawLine(
			{ gfxTimelineNow, gfxLanesStart },
			{ gfxTimelineNow, gfxLanesEnd },
			1,
			kNowDepth,
			math::Color3f::kGray50 );

		// Commit line
		gfx::ppu::Coord::ElementType const gfxTimelineCommit = rescaleToGfx( timelineCommit );
		ppu.DebugDrawLine(
			{ gfxTimelineCommit, gfxLanesStart },
			{ gfxTimelineCommit, gfxLanesEnd },
			1,
			kCommitDepth,
			timelineCommit < timelineNow ?
				math::Color3f::kYellow :
				math::Color3f::kGreen );

		// Window lines
		auto const drawAnnotationRange = [&]( time::CommonClock::time_point start, time::CommonClock::time_point end, uint8_t rank, math::Color3f color ) -> void //
		{
			RF_ASSERT( start <= end );
			gfx::ppu::CoordElem const gfxStartPos = rescaleToGfx( start );
			gfx::ppu::CoordElem const gfxEndPos = rescaleToGfx( end );
			gfx::ppu::CoordElem const topY = math::integer_cast<gfx::ppu::CoordElem>( gfxAnnotationStart + gfxAnnotationHeight * rank );
			gfx::ppu::CoordElem const bottomY = math::integer_cast<gfx::ppu::CoordElem>( gfxAnnotationStart + gfxAnnotationHeight * ( rank + 1 ) );
			gfx::ppu::CoordElem const spanY = gfxAnnotationStart + gfxAnnotationHeight * rank + gfxAnnotationHeight / 2;
			ppu.DebugDrawLine(
				{ gfxStartPos, topY },
				{ gfxStartPos, bottomY },
				1, kAnnotationDepth, color );
			ppu.DebugDrawLine(
				{ gfxEndPos, topY },
				{ gfxEndPos, bottomY },
				1, kAnnotationDepth, color );
			ppu.DebugDrawLine(
				{ gfxStartPos, spanY },
				{ gfxEndPos, spanY },
				1, kAnnotationDepth, color );
		};
		auto const drawWindow = [&]( rftl::optional<rollback::InclusiveTimeRange> const& startRange, rftl::optional<rollback::InclusiveTimeRange> const& endRange, uint8_t rank ) -> void //
		{
			if( startRange.has_value() )
			{
				RF_ASSERT( endRange.has_value() );
				time::CommonClock::time_point const a = math::Clamp( timelineStart, startRange->first, timelineEnd );
				time::CommonClock::time_point const b = math::Clamp( timelineStart, startRange->second, timelineEnd );
				time::CommonClock::time_point const c = math::Clamp( timelineStart, endRange->second, timelineEnd );
				time::CommonClock::time_point const& d = timelineEnd;
				drawAnnotationRange( a, b, rank, math::Color3f::kOrange );
				drawAnnotationRange( b, c, rank, math::Color3f::kWhite );
				drawAnnotationRange( c, d, rank, math::Color3f::kGray50 );
			}
		};
		drawWindow( sharedWindowStart, sharedWindowEnd, 0 );
		drawWindow( privateWindowStart, privateWindowEnd, 1 );

		size_t i_lane = 0;
		for( RollbackManager::InputStreams::value_type const& streamPair : committedStreams )
		{
			rollback::InputStream const& committedStream = streamPair.second;
			rollback::InputStream const& uncommittedStream = uncommittedStreams.at( streamPair.first );

			bool hasDrawnAFarFutureEvent = false;
			auto const onEvent = [&]( rollback::InputEvent const& event, math::Color3f const& color, bool renderInvalid ) -> void
			{
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
				gfx::ppu::Coord::ElementType const gfxTimelineEvent = rescaleToGfx( event.mTime );
				ppu.DebugDrawLine(
					{ gfxTimelineEvent, math::integer_cast<gfx::ppu::Coord::ElementType>( gfxLanesStart + ( gfxLanesHeight * ( math::integer_cast<gfx::ppu::Coord::ElementType>( i_lane ) + 0 ) ) ) },
					{ gfxTimelineEvent, math::integer_cast<gfx::ppu::Coord::ElementType>( gfxLanesStart + ( gfxLanesHeight * ( math::integer_cast<gfx::ppu::Coord::ElementType>( i_lane ) + 1 ) ) ) },
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



void ProcessReload( RF::input::GameCommand const& command )
{
	switch( command.mType )
	{
		case input::command::game::DeveloperAction1:
		{
			break;
		}
		case input::command::game::DeveloperAction2:
		{
			// TODO: Choose what to reload?
			break;
		}
		case input::command::game::DeveloperAction3:
		{
			break;
		}
		case input::command::game::DeveloperAction4:
		{
			appstate::InitialLoading::ReloadElementDatabase();
			appstate::InitialLoading::ReloadCastingEngine();
			break;
		}
		case input::command::game::DeveloperAction5:
		{
			break;
		}
		default:
			break;
	}
}



void RenderReload()
{
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	if( font.mManagedFontID == gfx::kInvalidManagedFontID )
	{
		// No font (not even a backup), so we may still be booting
		ppu.DebugDrawText( gfx::ppu::Coord( 16, 16 ), "No font loaded for developer hud" );
		return;
	}
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, ... ) -> bool
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, gfx::ppu::kNearestLayer, font.mFontHeight, font.mManagedFontID, true, color, fmt, args );
		va_end( args );
		return retVal;
	};

	static constexpr uint8_t kStartX = 4;
	static constexpr uint8_t kStartY = 1;
	uint8_t x = kStartX;
	uint8_t y = kStartY;

	drawText( x, y, math::Color3f::kMagenta, "RELOAD: ELEM+CAST" );
	y++;
}



void ProcessInputDevice( RF::input::GameCommand const& command )
{
	switch( command.mType )
	{
		case input::command::game::DeveloperAction1:
		{
			break;
		}
		case input::command::game::DeveloperAction2:
		{
			sInputDeviceIndex++;
			break;
		}
		case input::command::game::DeveloperAction3:
		{
			break;
		}
		case input::command::game::DeveloperAction4:
		{
			break;
		}
		case input::command::game::DeveloperAction5:
		{
			input::HardcodedSeverTick();
			break;
		}
		default:
			break;
	}
}



void RenderInputDevice()
{
	gfx::ppu::PPUController& ppu = *app::gGraphics;

	ui::Font const font = app::gFontRegistry->SelectBestFont( ui::font::NarrowQuarterTileMono, app::gGraphics->GetCurrentZoomFactor() );
	if( font.mManagedFontID == gfx::kInvalidManagedFontID )
	{
		// No font (not even a backup), so we may still be booting
		ppu.DebugDrawText( gfx::ppu::Coord( 16, 16 ), "No font loaded for developer hud" );
		return;
	}
	auto const drawText = [&ppu, &font]( uint8_t x, uint8_t y, math::Color3f const& color, char const* fmt, ... ) -> bool
	{
		gfx::ppu::Coord const pos = gfx::ppu::Coord( x * font.mFontHeight / 2, y * ( font.mBaselineOffset + font.mFontHeight ) );
		va_list args;
		va_start( args, fmt );
		bool const retVal = ppu.DebugDrawAuxText( pos, gfx::ppu::kNearestLayer, font.mFontHeight, font.mManagedFontID, true, color, fmt, args );
		va_end( args );
		return retVal;
	};

	static constexpr uint8_t kStartX = 4;
	static constexpr uint8_t kStartY = 1;
	uint8_t x = kStartX;
	uint8_t y = kStartY;

	drawText( x, y, math::Color3f::kMagenta, "INPUTDEVICE" );
	y++;

	// Helper for device drawing
	auto const drawDevice = [&drawText, &x, &y]( WeakPtr<input::InputDevice const> const deviceHandle ) -> void
	{
		if( deviceHandle == nullptr )
		{
			drawText( x, y, math::Color3f::kRed, "NULL DEVICE" );
			y++;
			return;
		}

		input::InputDevice const& device = *deviceHandle;
		drawText( x, y, math::Color3f::kCyan, "ID: <%s>", device.mIdentifier.c_str() );
		y++;

		// Raw input controllers are usually going to eat all the data out of
		//  buffers when they consume the data on a tick, so the buffers will
		//  usually be empty unless special steps are taken to prevent them from
		//  being consumed
		// TODO: There could be some debug mechanism to get at this, but it might
		//  be a one-way operation for something like the WndProc input device, as
		//  there would then be no way to re-enable it (unless it was something
		//  like a temporary time-delayed disable)
		RF_TODO_ANNOTATION( "Some mechanism to block the consumption of input buffers" );
		drawText( x, y, math::Color3f::kYellow, "REMINDER: Buffers usually get consumed" );
		y++;

		{
			// Digital
			if( device.mDigitalComponent != nullptr )
			{
				input::DigitalInputComponent const& digital = *device.mDigitalComponent;

				// Logical events
				{
					using LogicalEvent = input::DigitalInputComponent::LogicalEvent;
					using LogicEvents = rftl::static_vector<LogicalEvent, 8>;
					using LogicEventParser = rftl::virtual_back_inserter_iterator<LogicalEvent, LogicEvents>;
					LogicEvents logicEvents;
					LogicEventParser logicEventParser( logicEvents );
					rftl::stringstream logicStream;

					logicEvents.clear();
					digital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
					logicStream.str( "" );
					for( LogicEvents::value_type const& event : logicEvents )
					{
						logicStream << " " << static_cast<int>( event.mCode ) << ( event.mNewState == input::DigitalPinState::Active ? '#' : '-' );
					}
					drawText( x, y, math::Color3f::kWhite, "  lev: %s", logicStream.str().c_str() );
					y++;
				}

				// Physical events
				{
					using PhysicalEvent = input::DigitalInputComponent::PhysicalEvent;
					using PhysicEvents = rftl::static_vector<PhysicalEvent, 8>;
					using PhysicEventParser = rftl::virtual_back_inserter_iterator<PhysicalEvent, PhysicEvents>;
					PhysicEvents physicEvents;
					PhysicEventParser physicEventParser( physicEvents );
					rftl::stringstream physStream;

					physicEvents.clear();
					device.mDigitalComponent->GetPhysicalEventStream( physicEventParser, physicEvents.max_size() );
					physStream.str( "" );
					for( PhysicEvents::value_type const& event : physicEvents )
					{
						physStream << " " << static_cast<int>( event.mCode ) << ( event.mNewState == input::DigitalPinState::Active ? '#' : '-' );
					}
					drawText( x, y, math::Color3f::kWhite, "  pev: %s", physStream.str().c_str() );
					y++;
				}
			}
			else
			{
				drawText( x, y, math::Color3f::kYellow, "NO DIGITAL" );
				y++;
			}

			// Analog
			if( device.mAnalogComponent != nullptr )
			{
				input::AnalogInputComponent const& analog = *device.mAnalogComponent;

				input::AnalogSignalIndex const maxIndex = analog.GetMaxSignalIndex();
				for( input::AnalogSignalIndex index = 0; index <= maxIndex; index++ )
				{
					rftl::u16string const name16 = analog.GetSignalName( index );
					rftl::string const name = app::gPageMapper->MapTo8Bit(
						unicode::ConvertToUtf32(
							name16 ) );

					input::AnalogSignalValue const value = analog.GetCurrentSignalValue( index );

					drawText( x, y, math::Color3f::kWhite, "  %s: %f", name.c_str(), value );
					y++;
				}
			}
			else
			{
				drawText( x, y, math::Color3f::kYellow, "NO ANALOG" );
				y++;
			}

			// Text
			if( device.mTextComponent != nullptr )
			{
				input::TextInputComponent const& text = *device.mTextComponent;

				rftl::u16string textStream16;
				text.GetTextStream( textStream16, 100 );
				rftl::string const textStream = app::gPageMapper->MapTo8Bit(
					unicode::ConvertToUtf32(
						textStream16 ) );

				drawText( x, y, math::Color3f::kWhite, "  txt: %s", textStream.c_str() );
				y++;
			}
			else
			{
				drawText( x, y, math::Color3f::kYellow, "NO TEXT" );
				y++;
			}
		}
	};

	// Helper for raw controller drawing
	auto const drawRawController = [&drawText, &x, &y]( WeakPtr<input::RawController const> const controllerHandle ) -> void
	{
		if( controllerHandle == nullptr )
		{
			drawText( x, y, math::Color3f::kRed, "NULL RAW CONTROLLER" );
			y++;
			return;
		}

		input::RawController const& controller = *controllerHandle;
		drawText( x, y, math::Color3f::kCyan, "ID: <%s>", controller.mIdentifier.c_str() );
		y++;

		{
			{
				// Raw commands
				{
					using RawCommand = input::RawCommand;
					using RawCommands = rftl::static_vector<RawCommand, 8>;
					using RawCommandParser = rftl::virtual_back_inserter_iterator<RawCommand, RawCommands>;
					RawCommands rawCommands;
					RawCommandParser rawCommandParser( rawCommands );
					rftl::stringstream rawCommandStream;

					rawCommands.clear();
					controller.GetRawCommandStream( rawCommandParser, rawCommands.max_size() );
					rawCommandStream.str( "" );
					for( RawCommands::value_type const& command : rawCommands )
					{
						rawCommandStream << " " << static_cast<int>( command.mType );
					}
					drawText( x, y, math::Color3f::kWhite, "  rcmd: %s", rawCommandStream.str().c_str() );
					y++;
				}

				// Raw signals
				{
					using KnownSignal = input::RawSignalType;
					using KnownSignals = rftl::static_vector<KnownSignal, 8>;
					using KnownSignalSampler = rftl::virtual_back_inserter_iterator<KnownSignal, KnownSignals>;
					KnownSignals knownSignals;
					KnownSignalSampler knownSignalSampler( knownSignals );

					knownSignals.clear();
					controller.GetKnownSignals( knownSignalSampler, knownSignals.max_size() );
					for( size_t index = 0; index < knownSignals.size(); index++ )
					{
						using RawSignal = input::RawSignal;
						using RawSignals = rftl::static_vector<RawSignal, 8>;
						using RawSignalSampler = rftl::virtual_back_inserter_iterator<RawSignal, RawSignals>;
						RawSignals rawSignals;
						RawSignalSampler rawSignalSampler( rawSignals );
						rftl::stringstream rawSignalStream;

						rawSignals.clear();
						controller.GetRawSignalStream( rawSignalSampler, rawSignals.max_size(), 0 );
						rawSignalStream.str( "" );
						for( RawSignals::value_type const& signal : rawSignals )
						{
							rawSignalStream << " " << static_cast<int>( signal.mValue );
						}
						drawText( x, y, math::Color3f::kWhite, "  %z: %s", index, rawSignalStream.str().c_str() );
						y++;
					}
				}

				// Text
				{
					rftl::u16string textStream16;
					controller.GetTextStream( textStream16, 100 );
					rftl::string const textStream = app::gPageMapper->MapTo8Bit(
						unicode::ConvertToUtf32(
							textStream16 ) );

					drawText( x, y, math::Color3f::kWhite, "  txt: %s", textStream.c_str() );
					y++;
				}
			}
		}
	};

	// Helper for game controller drawing
	auto const drawGameController = [&drawText, &x, &y]( WeakPtr<input::GameController const> const controllerHandle ) -> void
	{
		if( controllerHandle == nullptr )
		{
			drawText( x, y, math::Color3f::kRed, "NULL GAME CONTROLLER" );
			y++;
			return;
		}

		input::GameController const& controller = *controllerHandle;
		drawText( x, y, math::Color3f::kCyan, "ID: <%s>", controller.mIdentifier.c_str() );
		y++;

		{
			{
				// Game commands
				{
					using GameCommand = input::GameCommand;
					using GameCommands = rftl::static_vector<GameCommand, 8>;
					using GameCommandParser = rftl::virtual_back_inserter_iterator<GameCommand, GameCommands>;
					GameCommands gameCommands;
					GameCommandParser gameCommandParser( gameCommands );
					rftl::stringstream gameCommandStream;

					gameCommands.clear();
					controller.GetGameCommandStream( gameCommandParser, gameCommands.max_size() );
					gameCommandStream.str( "" );
					for( GameCommands::value_type const& command : gameCommands )
					{
						gameCommandStream << " " << static_cast<int>( command.mType );
					}
					drawText( x, y, math::Color3f::kWhite, "  gcmd: %s", gameCommandStream.str().c_str() );
					y++;
				}

				// Game signals
				{
					using KnownSignal = input::GameSignalType;
					using KnownSignals = rftl::static_vector<KnownSignal, 8>;
					using KnownSignalSampler = rftl::virtual_back_inserter_iterator<KnownSignal, KnownSignals>;
					KnownSignals knownSignals;
					KnownSignalSampler knownSignalSampler( knownSignals );

					knownSignals.clear();
					controller.GetKnownSignals( knownSignalSampler, knownSignals.max_size() );
					for( size_t index = 0; index < knownSignals.size(); index++ )
					{
						using GameSignal = input::GameSignal;
						using GameSignals = rftl::static_vector<GameSignal, 8>;
						using GameSignalSampler = rftl::virtual_back_inserter_iterator<GameSignal, GameSignals>;
						GameSignals gameSignals;
						GameSignalSampler gameSignalSampler( gameSignals );
						rftl::stringstream gameSignalStream;

						gameSignals.clear();
						controller.GetGameSignalStream( gameSignalSampler, gameSignals.max_size(), 0 );
						gameSignalStream.str( "" );
						for( GameSignals::value_type const& signal : gameSignals )
						{
							gameSignalStream << " " << static_cast<int>( signal.mValue );
						}
						drawText( x, y, math::Color3f::kWhite, "  %z: %s", index, gameSignalStream.str().c_str() );
						y++;
					}
				}
			}
		}
	};

#if RF_IS_ALLOWED( RF_CONFIG_INPUT_DEBUG_ACCESS )

	input::ControllerManager const& controllerManager = *app::gInputControllerManager;

	input::ControllerManager::DebugPeekInputDeviceHandles const devices =
		controllerManager.DebugPeekInputDevices();
	input::ControllerManager::DebugPeekRawControllerHandles const rawControllers =
		controllerManager.DebugPeekRawControllers();
	input::ControllerManager::DebugPeekGameControllerHandles const gameControllers =
		controllerManager.DebugPeekGameControllers();
	if( devices.empty() )
	{
		drawText( x, y, math::Color3f::kYellow, "NO DEVICES STORED" );
		y++;
	}
	if( rawControllers.empty() )
	{
		drawText( x, y, math::Color3f::kYellow, "NO RAW CONTROLLERS STORED" );
		y++;
	}
	if( gameControllers.empty() )
	{
		drawText( x, y, math::Color3f::kYellow, "NO GAME CONTROLLERS STORED" );
		y++;
	}

	// Figure out total choices and sanitize current choice
	size_t const numChoices = devices.size() + rawControllers.size() + gameControllers.size();
	if( numChoices == 0 )
	{
		drawText( x, y, math::Color3f::kRed, "NO STORAGE CHOICES" );
		y++;
		return;
	}
	sInputDeviceIndex = sInputDeviceIndex % numChoices;
	RF_ASSERT( sInputDeviceIndex < numChoices );

	// Display list and current selection
	{
		rftl::string choices;
		choices.reserve( numChoices );
		choices.append( devices.size(), 'd' );
		choices.append( rawControllers.size(), 'r' );
		choices.append( gameControllers.size(), 'g' );
		static constexpr char kUpperOffset = 'A' - 'a';
		RF_ASSERT( sInputDeviceIndex < choices.size() );
		choices.at( sInputDeviceIndex ) += kUpperOffset;
		drawText( x, y, math::Color3f::kGreen, "LIST [%s]", choices.c_str() );
		y++;
	}

	// Use the choice to find the right data to display
	size_t rollingChoice = sInputDeviceIndex;
	RF_ASSERT( rollingChoice < numChoices );
	if( rollingChoice < devices.size() )
	{
		drawDevice( devices.at( rollingChoice ) );
		return;
	}
	rollingChoice -= devices.size();
	if( rollingChoice < rawControllers.size() )
	{
		drawRawController( rawControllers.at( rollingChoice ) );
		return;
	}
	rollingChoice -= rawControllers.size();
	if( rollingChoice < gameControllers.size() )
	{
		drawGameController( gameControllers.at( rollingChoice ) );
		return;
	}
	RF_DBGFAIL_MSG( "Bad math / logic" );

#else
	drawText( x, y, math::Color3f::kRed, "DEBUG INTERFACE DISABLED" );
	y++;
#endif
}

}
///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	rollback::RollbackManager& rollMan = *gRollbackManager;
	rollMan.TakeManualSnapshot( kSnapshotName, time::FrameClock::now() );
}



void ProcessInput()
{
	input::ControllerManager const& controllerManager = *app::gInputControllerManager;
	WeakPtr<input::GameController const> const controller = controllerManager.GetGameController( input::player::Global, input::layer::Developer );
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
			case input::command::game::DeveloperAction4:
			case input::command::game::DeveloperAction5:
			{
				switch( sCurrentMode )
				{
					case Mode::Rollback:
						mode::ProcessRollback( command );
						break;
					case Mode::Reload:
						mode::ProcessReload( command );
						break;
					case Mode::InputDevice:
						mode::ProcessInputDevice( command );
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
		case Mode::Reload:
			mode::RenderReload();
			return;
		case Mode::InputDevice:
			mode::RenderInputDevice();
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
}

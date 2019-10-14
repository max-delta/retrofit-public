#include "stdafx.h"
#include "DeveloperHud.h"

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


namespace RF { namespace cc { namespace developer {
///////////////////////////////////////////////////////////////////////////////

static bool sDisplayHud = false;
static constexpr char kSnapshotName[] = "DEV1";

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;
	rollMan.TakeManualSnapshot( kSnapshotName, time::FrameClock::now() );
}



void ProcessInput()
{
	rollback::RollbackManager& rollMan = *app::gRollbackManager;

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
	controller->GetGameCommandStream( parser, time::FrameClock::now() );

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
				break;
			}
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
}



void RenderHud()
{
	using namespace rftl::chrono;

	if( sDisplayHud == false )
	{
		return;
	}

	gfx::PPUController& ppu = *app::gGraphics;
	rollback::RollbackManager const& rollMan = *app::gRollbackManager;

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
}



void Shutdown()
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}}

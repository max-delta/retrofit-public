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

#include "Timing/FrameClock.h"


namespace RF { namespace cc { namespace developer {
///////////////////////////////////////////////////////////////////////////////

static bool sDisplayHud = false;

///////////////////////////////////////////////////////////////////////////////

void Startup()
{
	//
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
	controller->GetGameCommandStream( parser, time::FrameClock::now() );

	for( RF::input::GameCommand const& command : commands )
	{
		switch( command.mType )
		{
			case input::command::game::DeveloperToggle:
				sDisplayHud = !sDisplayHud;
				break;
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


	nanoseconds::rep const frameIndex = duration_cast<nanoseconds>( time::FrameClock::now().time_since_epoch() ) / duration_cast<nanoseconds>( time::kSimulationFrameDuration );
	drawText( x, y, math::Color3f::kMagenta, "FRM: %lli", frameIndex );
	x++;
}



void Shutdown()
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}}

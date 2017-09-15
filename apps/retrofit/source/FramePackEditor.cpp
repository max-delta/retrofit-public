#include "stdafx.h"
#include "FramePackEditor.h"

#include "PPU/PPUController.h"
#include "PlatformInput_win32/WndProcInputDevice.h"


// TODO: Singleton manager
extern RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

namespace RF {
///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::Init()
{
	//
}



void FramePackEditor::Process()
{
	//
}



void FramePackEditor::Render()
{
	using namespace RF;
	gfx::PPUController* const ppu = g_Graphics;
	enum class Mode : uint8_t
	{
		Meta = 0,
		Texture,
		Colliders
	};

	constexpr char k_Footer5[] =
		"<R>:Reload fpack  "
		"<A/D>:Change frame  "
		"<SPACE>:Snap to preview";
	constexpr char k_Footer6[] =
		"<Z>:Meta mode  "
		"<X>:Texture mode  "
		"<C>:Collider mode";

	constexpr size_t k_NumFooterLines = 6;
	constexpr gfx::PPUCoordElem k_TextYOffset = gfx::k_TileSize / 3;
	gfx::PPUCoord const footerStart( gfx::k_TileSize / 4, gfx::k_TileSize * gfx::k_DesiredDiagonalTiles - k_TextYOffset * k_NumFooterLines );
	gfx::PPUCoord const footerLine1Start( footerStart.x, footerStart.y );
	gfx::PPUCoord const footerLine2Start( footerLine1Start.x, footerLine1Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine3Start( footerLine2Start.x, footerLine2Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine4Start( footerLine3Start.x, footerLine3Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine5Start( footerLine4Start.x, footerLine4Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine6Start( footerLine5Start.x, footerLine5Start.y + k_TextYOffset );
	constexpr Mode mode = Mode::Colliders;
	switch( mode )
	{
		case Mode::Meta:
		{
			constexpr char k_Footer4Meta[] =
				"[META]  "
				"<Up/Dwn>:Change preferred framerate  "
				"<DEL>:Delete frame";
			ppu->DebugDrawText( footerLine4Start, k_Footer4Meta );
			break;
		}
		case Mode::Texture:
		{
			constexpr char k_Footer3Texture[] =
				"[TEXTURE]  "
				"<Arrows>:Change offset  "
				"<DEL>:Delete frame  ";
			constexpr char k_Footer4Texture[] =
				"[TEXTURE]  "
				"<B>:Insert before  "
				"<M>:Insert after";
			ppu->DebugDrawText( footerLine3Start, k_Footer3Texture );
			ppu->DebugDrawText( footerLine4Start, k_Footer4Texture );
			break;
		}
		case Mode::Colliders:
		{
			constexpr char k_Footer1Colliders[] =
				"[BOX]  "
				"<Arrows>:Move box  "
				"<CTRL+Arrows>:Resize box  "
				"<1-8>:Change layer";
			constexpr char k_Footer2Colliders[] =
				"[SET]  "
				"<U>:New set  "
				"<O>:Ref old set  "
				"<K>:Clone current set  "
				"<L>:Ref latest clone";
			constexpr char k_Footer3Colliders[] =
				"[COLLIDERS]  "
				"<SHIFT+Arrows>:Change offset  "
				"<PGUP/PGDN>:Select box";
			constexpr char k_Footer4Colliders[] =
				"[COLLIDERS]  "
				"<N>:New box  "
				"<BACK>:Delete box  "
				"<DEL>:Delete frame";
			ppu->DebugDrawText( footerLine1Start, k_Footer1Colliders );
			ppu->DebugDrawText( footerLine2Start, k_Footer2Colliders );
			ppu->DebugDrawText( footerLine3Start, k_Footer3Colliders );
			ppu->DebugDrawText( footerLine4Start, k_Footer4Colliders );
			break;
		}
		default:
			break;
	}
	ppu->DebugDrawText( footerLine5Start, k_Footer5 );
	ppu->DebugDrawText( footerLine6Start, k_Footer6 );
}

///////////////////////////////////////////////////////////////////////////////
}

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
	m_MasterMode = MasterMode::Meta;
}



void FramePackEditor::Process()
{
	input::WndProcDigitalInputComponent const& digital = g_WndProcInput->m_Digital;

	if( digital.WasActivatedLogical( 'Z' ) )
	{
		m_MasterMode = MasterMode::Meta;
	}
	if( digital.WasActivatedLogical( 'X' ) )
	{
		m_MasterMode = MasterMode::Texture;
	}
	if( digital.WasActivatedLogical( 'C' ) )
	{
		m_MasterMode = MasterMode::Colliders;
	}
}



void FramePackEditor::Render()
{
	using namespace RF;
	gfx::PPUController* const ppu = g_Graphics;

	constexpr size_t k_NumFooterLines = 6;
	constexpr gfx::PPUCoordElem k_TextYOffset = gfx::k_TileSize / 3;
	gfx::PPUCoord const footerStart( gfx::k_TileSize / 4, gfx::k_TileSize * gfx::k_DesiredDiagonalTiles - k_TextYOffset * k_NumFooterLines );

	gfx::PPUCoord const footerLine1Start( footerStart.x, footerStart.y );
	gfx::PPUCoord const footerLine2Start( footerLine1Start.x, footerLine1Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine3Start( footerLine2Start.x, footerLine2Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine4Start( footerLine3Start.x, footerLine3Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine5Start( footerLine4Start.x, footerLine4Start.y + k_TextYOffset );
	gfx::PPUCoord const footerLine6Start( footerLine5Start.x, footerLine5Start.y + k_TextYOffset );
	switch( m_MasterMode )
	{
		case MasterMode::Meta:
		{
			constexpr char k_Footer4Meta[] =
				"[META]  "
				"<Up/Dwn>:Change preferred framerate  "
				"<DEL>:Delete frame";
			ppu->DebugDrawText( footerLine4Start, k_Footer4Meta );
			break;
		}
		case MasterMode::Texture:
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
		case MasterMode::Colliders:
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
			RF_ASSERT( false );
			break;
	}
	constexpr char k_Footer5[] =
		"<R>:Reload fpack  "
		"<A/D>:Change frame  "
		"<SPACE>:Snap to preview";
	constexpr char k_Footer6[] =
		"<Z>:Meta mode  "
		"<X>:Texture mode  "
		"<C>:Collider mode";
	ppu->DebugDrawText( footerLine5Start, k_Footer5 );
	ppu->DebugDrawText( footerLine6Start, k_Footer6 );
}

///////////////////////////////////////////////////////////////////////////////
}

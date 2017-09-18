#include "stdafx.h"
#include "FramePackEditor.h"

#include "TEMP_AssetLoadSave.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformFilesystem/VFS.h"

#include "core_platform/winuser_shim.h"


// TODO: Singleton manager
extern RF::UniquePtr<RF::gfx::PPUController> g_Graphics;
extern RF::UniquePtr<RF::input::WndProcInputDevice> g_WndProcInput;

namespace RF {
///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::Init()
{
	m_MasterMode = MasterMode::Meta;
	m_FramePackID = gfx::k_InvalidManagedFramePackID;
	m_EditingFrame = 0;
	m_PreviewSlowdownRate = gfx::k_TimeSlowdownRate_Normal;
	m_PreviewObject = {};
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

	if( digital.WasActivatedLogical( shim::VK_ADD ) )
	{
		Command_ChangePreviewSpeed( true );
	}
	if( digital.WasActivatedLogical( shim::VK_SUBTRACT ) )
	{
		Command_ChangePreviewSpeed( false );
	}

	if( digital.WasActivatedLogical( 'R' ) )
	{
		// Reload FPack
	}
	if( digital.WasActivatedLogical( 'A' ) )
	{
		Command_ChangeEditingFrame( false );
	}
	if( digital.WasActivatedLogical( 'D' ) )
	{
		Command_ChangeEditingFrame( true );
	}
	if( digital.WasActivatedLogical( shim::VK_SPACE ) )
	{
		// Snap to preview
	}

	switch( m_MasterMode )
	{
		case MasterMode::Meta:
		{
			if( digital.WasActivatedLogical( 'U' ) )
			{
				// New FPack
			}
			if( digital.WasActivatedLogical( 'O' ) )
			{
				Command_OpenFramePack();
			}
			if( digital.WasActivatedLogical( shim::VK_UP ) )
			{
				// Increase preferred framerate
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) )
			{
				// Decrease preferred framerate
			}
			if( digital.WasActivatedLogical( shim::VK_DELETE ) )
			{
				// Delete frame
			}
			break;
		}
		case MasterMode::Texture:
		{
			if( digital.WasActivatedLogical( 'B' ) )
			{
				// Insert before
			}
			if( digital.WasActivatedLogical( 'M' ) )
			{
				// Insert after
			}
			if( digital.WasActivatedLogical( shim::VK_UP ) )
			{
				// Chamge offset
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) )
			{
				// Chamge offset
			}
			if( digital.WasActivatedLogical( shim::VK_LEFT ) )
			{
				// Chamge offset
			}
			if( digital.WasActivatedLogical( shim::VK_RIGHT ) )
			{
				// Chamge offset
			}
			if( digital.WasActivatedLogical( shim::VK_DELETE ) )
			{
				// Delete frame
			}
			break;
		}
		case MasterMode::Colliders:
		{
			// TODO
			//constexpr char k_Footer1Colliders[] =
			//	"[BOX]  "
			//	"<Arrows>:Move box  "
			//	"<CTRL+Arrows>:Resize box  "
			//	"<1-8>:Change layer";
			//constexpr char k_Footer2Colliders[] =
			//	"[SET]  "
			//	"<U>:New set  "
			//	"<O>:Ref old set  "
			//	"<K>:Clone current set  "
			//	"<L>:Ref latest clone";
			//constexpr char k_Footer3Colliders[] =
			//	"[COLLIDERS]  "
			//	"<SHIFT+Arrows>:Change offset  "
			//	"<PGUP/PGDN>:Select box";
			//constexpr char k_Footer4Colliders[] =
			//	"[COLLIDERS]  "
			//	"<N>:New box  "
			//	"<BACK>:Delete box  "
			//	"<DEL>:Delete frame";
			break;
		}
		default:
			RF_ASSERT( false );
			break;
	}
}



void FramePackEditor::Render()
{
	using namespace RF;
	gfx::PPUController* const ppu = g_Graphics;

	gfx::PPUCoord const textOffset( 0, gfx::k_TileSize / 3 );

	gfx::PPUCoord const headerOffset( gfx::k_TileSize / 6, gfx::k_TileSize / 3 );
	gfx::PPUCoord const previewHeaderStart = gfx::PPUCoord( 0, 0 ) + headerOffset;

	constexpr size_t k_NumFooterLines = 6;
	gfx::PPUCoord const footerStart( gfx::k_TileSize / 4, gfx::k_TileSize * gfx::k_DesiredDiagonalTiles - textOffset.y * k_NumFooterLines );

	gfx::PPUCoordElem const horizontalPlaneY = math::SnapNearest( footerStart.y, gfx::k_TileSize ) - gfx::k_TileSize;
	gfx::PPUCoordElem const verticalPlaneX = math::SnapNearest<gfx::PPUCoordElem>( ppu->GetWidth() / 2, gfx::k_TileSize );
	gfx::PPUCoordElem const previewOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX / 2, gfx::k_TileSize );
	gfx::PPUCoordElem const editingOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX + verticalPlaneX / 2, gfx::k_TileSize );

	uint8_t animationLength = 0;
	gfx::TimeSlowdownRate preferredSlowdownRate = gfx::k_TimeSlowdownRate_Normal;
	uint8_t numTimeSlots = 0;
	uint8_t slotSustain = 0;

	//
	// Plane lines
	{
		ppu->DebugDrawLine( gfx::PPUCoord( 0, horizontalPlaneY ), gfx::PPUCoord( ppu->GetWidth(), horizontalPlaneY ), 1 );
		ppu->DebugDrawLine( gfx::PPUCoord( verticalPlaneX, 0 ), gfx::PPUCoord( verticalPlaneX, horizontalPlaneY ), 1 );
	}

	//
	// FramePacks
	if( m_FramePackID != gfx::k_InvalidManagedFramePackID )
	{
		gfx::FramePackBase const* const fpack = ppu->DebugGetFramePackManager()->GetResourceFromManagedResourceID( m_FramePackID );
		RF_ASSERT( fpack != nullptr );

		numTimeSlots = fpack->m_NumTimeSlots;
		if( m_EditingFrame == numTimeSlots )
		{
			m_EditingFrame = 0;
		}
		else if( m_EditingFrame > numTimeSlots )
		{
			m_EditingFrame = numTimeSlots - 1;
		}

		animationLength = fpack->CalculateTimeIndexBoundary();
		preferredSlowdownRate = fpack->m_PreferredSlowdownRate;
		slotSustain = fpack->GetTimeSlotSustains()[m_EditingFrame];

		m_PreviewObject.m_FramePackID = m_FramePackID;
		m_PreviewObject.m_MaxTimeIndex = animationLength;
		m_PreviewObject.m_TimeSlowdown = m_PreviewSlowdownRate;
		m_PreviewObject.m_Looping = true;
		m_PreviewObject.m_XCoord = previewOriginX;
		m_PreviewObject.m_YCoord = horizontalPlaneY;
		m_PreviewObject.m_ZLayer = 0;

		ppu->DrawObject( m_PreviewObject );
		m_PreviewObject.Animate();

		gfx::Object editingObject = m_PreviewObject;
		editingObject.m_Paused = true;
		editingObject.m_TimeIndex = fpack->CalculateFirstTimeIndexOfTimeSlot( m_EditingFrame );
		editingObject.m_XCoord = editingOriginX;
		editingObject.m_YCoord = horizontalPlaneY;
		editingObject.m_ZLayer = 0;

		ppu->DrawObject( editingObject );
	}

	//
	// Preview header
	{
		ppu->DebugDrawText( previewHeaderStart, "Preview" );
		gfx::TimeSlowdownRate const previewFPS = 60 / m_PreviewSlowdownRate;
		ppu->DebugDrawText( previewHeaderStart + textOffset, "Preview FPS: %i <-/+> to change", previewFPS );
		gfx::TimeSlowdownRate const dataFPS = 60 / preferredSlowdownRate;
		ppu->DebugDrawText( previewHeaderStart + textOffset * 2, "Data FPS: %i", dataFPS );
		uint16_t const effectiveFrames = animationLength * m_PreviewSlowdownRate;
		ppu->DebugDrawText( previewHeaderStart + textOffset * 3, "Preview frames: %i", effectiveFrames );
	}

	//
	// Editing header
	{
		gfx::PPUCoord const editingHeaderStart = gfx::PPUCoord( verticalPlaneX, 0 ) + headerOffset;
		ppu->DebugDrawText( editingHeaderStart, "Editing" );
		ppu->DebugDrawText( editingHeaderStart + textOffset, "Frame: %i / %i", m_EditingFrame, numTimeSlots - 1 );
		ppu->DebugDrawText( editingHeaderStart + textOffset * 2, "Sustain: %i", slotSustain );
		char const TODOTexture[] = "TODO/TODO.png";
		ppu->DebugDrawText( editingHeaderStart + textOffset * 3, "Texture: %s", TODOTexture );
	}

	//
	// Footer
	{
		gfx::PPUCoord const footerLine1Start( footerStart );
		gfx::PPUCoord const footerLine2Start( footerLine1Start + textOffset );
		gfx::PPUCoord const footerLine3Start( footerLine2Start + textOffset );
		gfx::PPUCoord const footerLine4Start( footerLine3Start + textOffset );
		gfx::PPUCoord const footerLine5Start( footerLine4Start + textOffset );
		gfx::PPUCoord const footerLine6Start( footerLine5Start + textOffset );
		switch( m_MasterMode )
		{
			case MasterMode::Meta:
			{
				constexpr char k_Footer3Meta[] =
					"[META]  "
					"<U>:New FPack  "
					"<O>:Open FPack";
				constexpr char k_Footer4Meta[] =
					"[META]  "
					"<Up/Dwn>:Change preferred framerate  "
					"<DEL>:Delete frame";
				ppu->DebugDrawText( footerLine3Start, k_Footer3Meta );
				ppu->DebugDrawText( footerLine4Start, k_Footer4Meta );
				break;
			}
			case MasterMode::Texture:
			{
				constexpr char k_Footer3Texture[] =
					"[TEXTURE]  "
					"<B>:Insert before  "
					"<M>:Insert after";
				constexpr char k_Footer4Texture[] =
					"[TEXTURE]  "
					"<Arrows>:Change offset  "
					"<DEL>:Delete frame  ";
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
}



void FramePackEditor::Command_ChangePreviewSpeed( bool faster )
{
	if( faster )
	{
		m_PreviewSlowdownRate--;
	}
	else
	{
		m_PreviewSlowdownRate++;
	}
	m_PreviewSlowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, m_PreviewSlowdownRate, 10 );
}



void FramePackEditor::Command_ChangeEditingFrame( bool increase )
{
	if( increase )
	{
		m_EditingFrame++;
	}
	else
	{
		m_EditingFrame--;
	}
}



void FramePackEditor::Command_OpenFramePack()
{
	// HACK
	// TODO: File selector
	file::VFSPath const commonFramepacks = file::VFS::k_Root.GetChild( "assets", "framepacks", "common" );
	OpenFramePack( commonFramepacks.GetChild( "testdigit_loop.fpack.sq" ) );
}

///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::OpenFramePack( file::VFSPath const & path )
{
	UniquePtr<gfx::FramePackBase> fpack = LoadFramePackFromSquirrel( path );
	m_FramePackID = g_Graphics->DebugGetFramePackManager()->LoadNewResourceGetID( "EDITPACK", std::move( fpack ) );
}

///////////////////////////////////////////////////////////////////////////////
}

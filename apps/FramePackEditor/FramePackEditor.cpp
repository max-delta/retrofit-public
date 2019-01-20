#include "stdafx.h"
#include "FramePackEditor.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"

#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformUtils_win32/dialogs.h"
#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "Logging/Logging.h"

#include "core_platform/winuser_shim.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/static_array.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

FramePackEditor::FramePackEditor( WeakPtr<file::VFS> const& vfs )
	: m_Vfs( vfs )
{
	//
}



void FramePackEditor::Init()
{
	gfx::PPUController* const ppu = app::g_Graphics;

	m_MasterMode = MasterMode::Meta;
	m_FramePackID = gfx::k_InvalidManagedFramePackID;
	m_EditingFrame = 0;
	m_PreviewSlowdownRate = gfx::k_TimeSlowdownRate_Normal;
	m_PreviewObject = {};

	file::VFS& vfs = *m_Vfs;
	file::VFSPath const fonts = file::VFS::k_Root.GetChild( "assets", "textures", "fonts" );
	file::FileHandlePtr const fontHandle = vfs.GetFileForRead( fonts.GetChild( "font_narrow_1x.bmp" ) );
	ppu->LoadFont( fontHandle->GetFile() );
}



void FramePackEditor::Process()
{
	input::WndProcDigitalInputComponent const& digital = app::g_WndProcInput->m_Digital;

	typedef input::DigitalInputComponent::LogicalEvent LogicalEvent;
	typedef rftl::static_array<LogicalEvent, 1> LogicEvents;
	typedef input::BufferCopyEventParser<LogicalEvent, LogicEvents> LogicEventParser;
	LogicEvents logicEvents;
	LogicEventParser logicEventParser( logicEvents );
	digital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
	uint8_t mostRecentHold = 0;
	if( logicEvents.empty() == false )
	{
		LogicalEvent mostRecentEvent( 0, input::DigitalInputComponent::PinState::Inactive );
		mostRecentEvent = *logicEvents.rbegin();
		if( mostRecentEvent.m_NewState == input::DigitalInputComponent::PinState::Active )
		{
			RF_ASSERT( mostRecentEvent.m_Time > time::FrameClock::time_point() );
			RF_ASSERT( time::FrameClock::now() >= mostRecentEvent.m_Time );
			time::FrameClock::duration const timePassed = time::FrameClock::now() - mostRecentEvent.m_Time;
			if( timePassed > rftl::chrono::milliseconds( 200 ) )
			{
				mostRecentHold = mostRecentEvent.m_Code;
			}
		}
	}

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

	if( digital.WasActivatedLogical( shim::VK_ADD ) || mostRecentHold == shim::VK_ADD )
	{
		Command_ChangePreviewSpeed( true );
	}
	if( digital.WasActivatedLogical( shim::VK_SUBTRACT ) || mostRecentHold == shim::VK_SUBTRACT )
	{
		Command_ChangePreviewSpeed( false );
	}
	if( digital.WasActivatedLogical( shim::VK_MULTIPLY ) || mostRecentHold == shim::VK_MULTIPLY )
	{
		if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
		{
			Command_BatchChangeSustainCount( true );
		}
		else
		{
			Command_ChangeSustainCount( true );
		}
	}
	if( digital.WasActivatedLogical( shim::VK_DIVIDE ) || mostRecentHold == shim::VK_DIVIDE )
	{
		if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
		{
			Command_BatchChangeSustainCount( false );
		}
		else
		{
			Command_ChangeSustainCount( false );
		}
	}

	if( digital.WasActivatedLogical( 'R' ) )
	{
		Command_ReloadFramePack();
	}
	if( digital.WasActivatedLogical( 'A' ) || mostRecentHold == 'A' )
	{
		Command_ChangeEditingFrame( false );
	}
	if( digital.WasActivatedLogical( 'D' ) || mostRecentHold == 'D' )
	{
		Command_ChangeEditingFrame( true );
	}
	if( digital.WasActivatedLogical( shim::VK_SPACE ) || mostRecentHold == shim::VK_SPACE )
	{
		RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "TODO: Snap to preview" );
	}

	switch( m_MasterMode )
	{
		case MasterMode::Meta:
		{
			if( digital.WasActivatedLogical( 'U' ) )
			{
				Command_Meta_CreateFramePack();
			}
			if( digital.WasActivatedLogical( 'O' ) )
			{
				Command_Meta_OpenFramePack();
			}
			if( digital.WasActivatedLogical( shim::VK_UP ) || mostRecentHold == shim::VK_UP )
			{
				Command_Meta_ChangeDataSpeed( true );
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) || mostRecentHold == shim::VK_DOWN )
			{
				Command_Meta_ChangeDataSpeed( false );
			}
			if( digital.WasActivatedLogical( shim::VK_DELETE ) )
			{
				Command_Meta_DeleteFrame();
			}
			break;
		}
		case MasterMode::Texture:
		{
			if( digital.WasActivatedLogical( 'B' ) )
			{
				Command_Texture_InsertBefore();
			}
			if( digital.WasActivatedLogical( 'M' ) )
			{
				Command_Texture_InsertAfter();
			}
			if( digital.WasActivatedLogical( shim::VK_UP ) || mostRecentHold == shim::VK_UP )
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					Command_Texture_BatchChangeOffset( 0, -1 );
				}
				else
				{
					Command_Texture_ChangeOffset( 0, -1 );
				}
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) || mostRecentHold == shim::VK_DOWN )
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					Command_Texture_BatchChangeOffset( 0, 1 );
				}
				else
				{
					Command_Texture_ChangeOffset( 0, 1 );
				}
			}
			if( digital.WasActivatedLogical( shim::VK_LEFT ) || mostRecentHold == shim::VK_LEFT )
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					Command_Texture_BatchChangeOffset( -1, 0 );
				}
				else
				{
					Command_Texture_ChangeOffset( -1, 0 );
				}
			}
			if( digital.WasActivatedLogical( shim::VK_RIGHT ) || mostRecentHold == shim::VK_RIGHT )
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					Command_Texture_BatchChangeOffset( 1, 0 );
				}
				else
				{
					Command_Texture_ChangeOffset( 1, 0 );
				}
			}
			if( digital.WasActivatedLogical( shim::VK_DELETE ) )
			{
				Command_Meta_DeleteFrame();
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
			RF_DBGFAIL();
			break;
	}
}



void FramePackEditor::Render()
{
	gfx::PPUController* const ppu = app::g_Graphics;
	gfx::FramePackManager const& fpackMan = *ppu->DebugGetFramePackManager();
	gfx::TextureManager const& texMan = *ppu->DebugGetTextureManager();
	input::WndProcDigitalInputComponent const& digital = app::g_WndProcInput->m_Digital;

	gfx::PPUCoord const fontSize( 4, 8 );
	gfx::PPUCoord const textOffset( 0, fontSize.y );

	gfx::PPUCoord const headerOffset( gfx::k_TileSize / 16, gfx::k_TileSize / 16 );
	gfx::PPUCoord const previewHeaderStart = gfx::PPUCoord( 0, 0 ) + headerOffset;

	constexpr size_t k_NumFooterLines = 6;
	gfx::PPUCoord const footerStart( gfx::k_TileSize / 16, gfx::k_TileSize * gfx::k_DesiredDiagonalTiles - textOffset.y * math::integer_cast<gfx::PPUCoordElem>( k_NumFooterLines ) );

	gfx::PPUCoordElem const horizontalPlaneY = math::SnapNearest( footerStart.y, gfx::k_TileSize ) - gfx::k_TileSize;
	gfx::PPUCoordElem const verticalPlaneX = math::SnapNearest<gfx::PPUCoordElem>( ppu->GetWidth() / 2, gfx::k_TileSize );
	gfx::PPUCoordElem const previewOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX / 2, gfx::k_TileSize );
	gfx::PPUCoordElem const editingOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX + verticalPlaneX / 2, gfx::k_TileSize );

	uint8_t animationLength = 0;
	gfx::TimeSlowdownRate preferredSlowdownRate = gfx::k_TimeSlowdownRate_Normal;
	uint8_t numTimeSlots = 0;
	uint8_t slotSustain = 0;
	gfx::PPUCoord texOrigin{ 0, 0 };
	math::Vector2<int64_t> texSize{ 0, 0 };

	//
	// Plane lines
	{
		ppu->DebugDrawLine( gfx::PPUCoord( 0, horizontalPlaneY ), gfx::PPUCoord( ppu->GetWidth(), horizontalPlaneY ), 1 );
		ppu->DebugDrawLine( gfx::PPUCoord( verticalPlaneX, 0 ), gfx::PPUCoord( verticalPlaneX, horizontalPlaneY ), 1 );
	}

	//
	// Origin point
	{
		gfx::PPUCoord const editingOriginPoint( editingOriginX, horizontalPlaneY );
		constexpr gfx::PPUCoordElem pointSize = gfx::k_TileSize / 8;
		ppu->DebugDrawLine( editingOriginPoint - pointSize, editingOriginPoint + pointSize, 1 );
	}

	//
	// FramePacks
	gfx::ManagedTextureID editingTextureID = gfx::k_InvalidDeviceTextureID;
	if( m_FramePackID != gfx::k_InvalidManagedFramePackID )
	{
		gfx::FramePackBase const* const fpack = fpackMan.GetResourceFromManagedResourceID( m_FramePackID );
		RF_ASSERT( fpack != nullptr );

		numTimeSlots = fpack->m_NumTimeSlots;
		if( m_EditingFrame == numTimeSlots )
		{
			m_EditingFrame = 0;
		}
		else if( m_EditingFrame > numTimeSlots )
		{
			m_EditingFrame = numTimeSlots - 1u;
		}

		gfx::FramePackBase::TimeSlot const& timeSlot = fpack->GetTimeSlots()[m_EditingFrame];

		animationLength = fpack->CalculateTimeIndexBoundary();
		preferredSlowdownRate = fpack->m_PreferredSlowdownRate;
		slotSustain = fpack->GetTimeSlotSustains()[m_EditingFrame];
		texOrigin.x = timeSlot.m_TextureOriginX;
		texOrigin.y = timeSlot.m_TextureOriginY;

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

		editingTextureID = timeSlot.m_TextureReference;
		gfx::Texture const* const tex = texMan.GetResourceFromManagedResourceID( editingTextureID );
		if( tex != nullptr )
		{
			texSize.x = tex->DebugGetWidth();
			texSize.y = tex->DebugGetHeight();
		}
	}

	//
	// Preview header
	{
		ppu->DrawText( previewHeaderStart, fontSize, "Preview" );
		gfx::TimeSlowdownRate const previewFPS = 60u / m_PreviewSlowdownRate;
		ppu->DrawText( previewHeaderStart + textOffset, fontSize, "Preview FPS: %i <-/+> to change", previewFPS );
		gfx::TimeSlowdownRate const dataFPS = 60u / preferredSlowdownRate;
		if( m_MasterMode == MasterMode::Meta )
		{
			ppu->DrawText( previewHeaderStart + textOffset * 2, fontSize, "Data FPS: %i <Up/Dn> to change", dataFPS );
		}
		else
		{
			ppu->DrawText( previewHeaderStart + textOffset * 2, fontSize, "Data FPS: %i", dataFPS );
		}
		uint16_t const effectiveFrames = math::integer_cast<uint16_t>( animationLength * m_PreviewSlowdownRate );
		ppu->DrawText( previewHeaderStart + textOffset * 3, fontSize, "Preview frames: %i", effectiveFrames );
	}

	//
	// Editing header
	{
		gfx::PPUCoord const editingHeaderStart = gfx::PPUCoord( verticalPlaneX, 0 ) + headerOffset;
		ppu->DrawText( editingHeaderStart, fontSize, "Editing" );
		ppu->DrawText( editingHeaderStart + textOffset, fontSize, "Frame: %i / [0-%i] <A/D> to change", m_EditingFrame, numTimeSlots - 1 );
		if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
		{
			ppu->DrawText( editingHeaderStart + textOffset * 2, fontSize, "Sustain: %i <Ctrl+/,*> to batch", slotSustain );
		}
		else
		{
			ppu->DrawText( editingHeaderStart + textOffset * 2, fontSize, "Sustain: %i </,*> to change", slotSustain );
		}
		file::VFSPath const texPath = texMan.SearchForFilenameByResourceID( editingTextureID );
		if( texPath.Empty() )
		{
			ppu->DrawText( editingHeaderStart + textOffset * 3, fontSize, "Texture: INVALID" );
		}
		else
		{
			file::VFSPath::Element const lastElement = texPath.GetElement( texPath.NumElements() - 1 );
			ppu->DrawText( editingHeaderStart + textOffset * 3, fontSize, "Texture: %s", lastElement.c_str() );
		}
		switch( m_MasterMode )
		{
			case MasterMode::Meta:
			{
				ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, "Origin: %i, %i", texOrigin.x, texOrigin.y );
				break;
			}
			case MasterMode::Texture:
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, "Origin: %i, %i <Ctrl+Arrows> to batch", texOrigin.x, texOrigin.y );
				}
				else
				{
					ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, "Origin: %i, %i <Arrows> to change", texOrigin.x, texOrigin.y );
				}
				ppu->DrawText( editingHeaderStart + textOffset * 5, fontSize, "Size: %lli, %lli", texSize.x, texSize.y );
				break;
			}
			case MasterMode::Colliders:
			{
				ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, "TODO" );
				break;
			}
			default:
				RF_DBGFAIL();
				break;
		}
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
				ppu->DrawText( footerLine3Start, fontSize, k_Footer3Meta );
				ppu->DrawText( footerLine4Start, fontSize, k_Footer4Meta );
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
				constexpr char k_FooterAlt4Texture[] =
					"[TEXTURE]  "
					"<Ctrl+Arrows>:Batch offset  "
					"<DEL>:Delete frame  ";
				ppu->DrawText( footerLine3Start, fontSize, k_Footer3Texture );
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					ppu->DrawText( footerLine4Start, fontSize, k_FooterAlt4Texture );
				}
				else
				{
					ppu->DrawText( footerLine4Start, fontSize, k_Footer4Texture );
				}
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
				ppu->DrawText( footerLine1Start, fontSize, k_Footer1Colliders );
				ppu->DrawText( footerLine2Start, fontSize, k_Footer2Colliders );
				ppu->DrawText( footerLine3Start, fontSize, k_Footer3Colliders );
				ppu->DrawText( footerLine4Start, fontSize, k_Footer4Colliders );
				break;
			}
			default:
				RF_DBGFAIL();
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
		ppu->DrawText( footerLine5Start, fontSize, k_Footer5 );
		ppu->DrawText( footerLine6Start, fontSize, k_Footer6 );
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



void FramePackEditor::Command_ChangeSustainCount( bool increase )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();
	RF_ASSERT( m_EditingFrame < fpack->m_NumTimeSlots );
	uint8_t& sustain = timeSlotSustains[m_EditingFrame];
	if( increase )
	{
		sustain++;
	}
	else
	{
		sustain--;
	}
	sustain = math::Clamp<uint8_t>( 1, sustain, 240 );
}



void FramePackEditor::Command_BatchChangeSustainCount( bool increase )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();
	for( size_t i = 0; i < fpack->m_NumTimeSlots; i++ )
	{
		uint8_t& sustain = timeSlotSustains[i];
		if( increase )
		{
			sustain++;
		}
		else
		{
			sustain--;
		}
		sustain = math::Clamp<uint8_t>( 1, sustain, 240 );
	}
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



void FramePackEditor::Command_ReloadFramePack()
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackManager const& fpackMan = *ppu->DebugGetFramePackManager();
	file::VFSPath const filename = fpackMan.SearchForFilenameByResourceName( kFramePackName );
	OpenFramePack( filename );
}



void FramePackEditor::Command_Meta_ChangeDataSpeed( bool faster )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t& slowdownRate = fpack->m_PreferredSlowdownRate;
	if( faster )
	{
		slowdownRate--;
	}
	else
	{
		slowdownRate++;
	}
	slowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, slowdownRate, 10 );
}



void FramePackEditor::Command_Meta_CreateFramePack()
{
	gfx::PPUController* const ppu = app::g_Graphics;

	gfx::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	gfx::TextureManager& texMan = *ppu->DebugGetTextureManager();
	if( m_FramePackID != gfx::k_InvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	UniquePtr<gfx::FramePackBase> newFPack = DefaultCreator<gfx::FramePack_256>::Create();
	file::VFSPath const defaultFrame = file::VFS::k_Root.GetChild( "assets", "textures", "common", "max_delta_32.png" );
	newFPack->m_NumTimeSlots = 1;
	newFPack->GetMutableTimeSlots()[0].m_TextureReference = texMan.LoadNewResourceGetID( kInitialTextureName, defaultFrame );
	m_FramePackID = fpackMan.LoadNewResourceGetID( kFramePackName, rftl::move( newFPack ) );
}



void FramePackEditor::Command_Meta_OpenFramePack()
{
	rftl::string const rawPath = platform::dialogs::OpenFileDialog();
	if( rawPath.empty() )
	{
		// User cancelled?
		return;
	}
	OpenFramePack( rawPath );
}



void FramePackEditor::Command_Meta_DeleteFrame()
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->GetResourceFromManagedResourceID( m_FramePackID );
	RF_ASSERT( fpack != nullptr );
	size_t const& numSlots = fpack->m_NumTimeSlots;
	if( numSlots <= 1 )
	{
		return;
	}

	RemoveTimeSlotAt( m_EditingFrame );
}



void FramePackEditor::Command_Texture_InsertBefore()
{
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	InsertTimeSlotBefore( m_EditingFrame );
	ChangeTexture( m_EditingFrame );
}



void FramePackEditor::Command_Texture_InsertAfter()
{
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	InsertTimeSlotBefore( m_EditingFrame + 1u );
	m_EditingFrame = m_EditingFrame + 1u;
	ChangeTexture( m_EditingFrame );
}



void FramePackEditor::Command_Texture_ChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( m_EditingFrame <= fpack->m_NumTimeSlots );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::FramePackBase::TimeSlot& timeSlot = timeSlots[m_EditingFrame];

	timeSlot.m_TextureOriginX = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginX - x );
	timeSlot.m_TextureOriginY = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginY - y );
}



void FramePackEditor::Command_Texture_BatchChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	for( size_t i = 0; i < fpack->m_NumTimeSlots; i++ )
	{
		gfx::FramePackBase::TimeSlot& timeSlot = timeSlots[i];

		timeSlot.m_TextureOriginX = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginX - x );
		timeSlot.m_TextureOriginY = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginY - y );
	}
}

///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::OpenFramePack( rftl::string const& rawPath )
{
	file::VFS const& vfs = *m_Vfs;
	file::VFSPath const filePath = vfs.AttemptMapToVFS( rawPath, file::VFSMount::Permissions::ReadOnly );
	OpenFramePack( filePath );
}



void FramePackEditor::OpenFramePack( file::VFSPath const& path )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	gfx::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	if( m_FramePackID != gfx::k_InvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	if( path.Empty() )
	{
		RFLOG_ERROR( path, RFCAT_FRAMEPACKEDITOR, "Invalid frame pack filename" );
		m_FramePackID = gfx::k_InvalidManagedFramePackID;
		return;
	}
	m_FramePackID = fpackMan.LoadNewResourceGetID( kFramePackName, path );

	if( m_FramePackID != gfx::k_InvalidManagedFramePackID )
	{
		gfx::FramePackBase const* fpack = fpackMan.GetResourceFromManagedResourceID( m_FramePackID );
		RF_ASSERT( fpack != nullptr );
		m_PreviewSlowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, fpack->m_PreferredSlowdownRate, 10 );
	}
}



void FramePackEditor::InsertTimeSlotBefore( size_t slotIndex )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	RF_ASSERT( m_FramePackID != gfx::k_InvalidManagedFramePackID );
	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->m_NumTimeSlots;
	RF_ASSERT( numSlots + 1 <= fpack->m_MaxTimeSlots );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();

	// Push everything over
	RF_ASSERT( slotIndex <= numSlots );
	for( size_t i = numSlots; i > slotIndex; i-- )
	{
		timeSlots[i] = timeSlots[i - 1];
		timeSlotSustains[i] = timeSlotSustains[i - 1];
	}

	if( slotIndex != 0 )
	{
		timeSlots[slotIndex] = timeSlots[slotIndex - 1];
		timeSlots[slotIndex].m_TextureReference = gfx::k_InvalidManagedTextureID;
		// TODO: Typedef
		timeSlots[slotIndex].m_ColliderReference = static_cast<uint64_t>( 0 );
		timeSlotSustains[slotIndex] = timeSlotSustains[slotIndex - 1];
	}
	else
	{
		timeSlots[slotIndex] = {};
		timeSlotSustains[slotIndex] = 1;
	}

	fpack->m_NumTimeSlots++;
}



void FramePackEditor::RemoveTimeSlotAt( size_t slotIndex )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->m_NumTimeSlots;
	RF_ASSERT( numSlots - 1 > 0 );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();

	// Push everything over
	RF_ASSERT( slotIndex <= numSlots );
	for( size_t i = slotIndex; i < numSlots - 1; i++ )
	{
		timeSlots[i] = timeSlots[i + 1];
		timeSlotSustains[i] = timeSlotSustains[i + 1];
	}

	timeSlots[numSlots - 1] = {};
	timeSlotSustains[numSlots - 1] = 1;

	fpack->m_NumTimeSlots--;
}



void FramePackEditor::ChangeTexture( size_t slotIndex )
{
	gfx::PPUController* const ppu = app::g_Graphics;

	RF_ASSERT( m_FramePackID != gfx::k_InvalidManagedFramePackID );
	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( slotIndex < fpack->m_NumTimeSlots );
	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::ManagedTextureID& textureID = timeSlots[slotIndex].m_TextureReference;

	// User needs to select texture
	rftl::string const filepath = platform::dialogs::OpenFileDialog();
	if( filepath.empty() )
	{
		// User probably cancelled
		return;
	}

	// Need to map user choice into VFS
	file::VFS* const vfs = m_Vfs;
	file::VFSPath mappedPath = vfs->AttemptMapToVFS( filepath, file::VFSMount::Permissions::ReadOnly );
	if( mappedPath.Empty() )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEPACKEDITOR, "Unable to map to VFS" );
		return;
	}

	gfx::TextureManager* const texMan = ppu->DebugGetTextureManager();
	textureID = texMan->LoadNewResourceGetID( filepath, mappedPath );
}

///////////////////////////////////////////////////////////////////////////////
}

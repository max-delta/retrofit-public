#include "stdafx.h"
#include "FramePackEditor.h"

#include "TEMP_AssetLoadSave.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformUtils_win32/dialogs.h"
#include "PlatformFilesystem/VFS.h"
#include "Logging/Logging.h"

#include "core_platform/winuser_shim.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/static_array.h"


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

	if( digital.WasActivatedLogical( shim::VK_ADD ) )
	{
		Command_ChangePreviewSpeed( true );
	}
	if( digital.WasActivatedLogical( shim::VK_SUBTRACT ) )
	{
		Command_ChangePreviewSpeed( false );
	}
	if( digital.WasActivatedLogical( shim::VK_MULTIPLY ) )
	{
		Command_ChangeSustainCount( true );
	}
	if( digital.WasActivatedLogical( shim::VK_DIVIDE ) )
	{
		Command_ChangeSustainCount( false );
	}

	if( digital.WasActivatedLogical( 'R' ) )
	{
		Command_ReloadFramePack();
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
			if( digital.WasActivatedLogical( shim::VK_UP ) )
			{
				Command_Meta_ChangeDataSpeed( true );
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) )
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
				Command_Texture_ChangeOffset( 0, -1 );
			}
			if( digital.WasActivatedLogical( shim::VK_DOWN ) || mostRecentHold == shim::VK_DOWN )
			{
				Command_Texture_ChangeOffset( 0, 1 );
			}
			if( digital.WasActivatedLogical( shim::VK_LEFT ) || mostRecentHold == shim::VK_LEFT )
			{
				Command_Texture_ChangeOffset( -1, 0 );
			}
			if( digital.WasActivatedLogical( shim::VK_RIGHT ) || mostRecentHold == shim::VK_RIGHT )
			{
				Command_Texture_ChangeOffset( 1, 0 );
			}
			if( digital.WasActivatedLogical( shim::VK_DELETE ) )
			{
				Command_Meta_DeleteFrame();
			}
			break;
		}
		case MasterMode::Colliders:
		{
			RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "TODO: Colliders" );
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
	gfx::PPUController* const ppu = g_Graphics;
	gfx::FramePackManager const& fpackMan = *ppu->DebugGetFramePackManager();
	gfx::TextureManager const& texMan = *ppu->DebugGetTextureManager();

	gfx::PPUCoord const textOffset( 0, gfx::k_TileSize / 3 );

	gfx::PPUCoord const headerOffset( gfx::k_TileSize / 6, gfx::k_TileSize / 3 );
	gfx::PPUCoord const previewHeaderStart = gfx::PPUCoord( 0, 0 ) + headerOffset;

	constexpr size_t k_NumFooterLines = 6;
	gfx::PPUCoord const footerStart( gfx::k_TileSize / 4u, gfx::k_TileSize * gfx::k_DesiredDiagonalTiles - textOffset.y * math::integer_cast<gfx::PPUCoordElem>( k_NumFooterLines ) );

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

		editingTextureID = fpack->GetTimeSlots()[m_EditingFrame].m_TextureReference;
	}

	//
	// Preview header
	{
		ppu->DebugDrawText( previewHeaderStart, "Preview" );
		gfx::TimeSlowdownRate const previewFPS = 60u / m_PreviewSlowdownRate;
		ppu->DebugDrawText( previewHeaderStart + textOffset, "Preview FPS: %i <-/+> to change", previewFPS );
		gfx::TimeSlowdownRate const dataFPS = 60u / preferredSlowdownRate;
		ppu->DebugDrawText( previewHeaderStart + textOffset * 2, "Data FPS: %i", dataFPS );
		uint16_t const effectiveFrames = math::integer_cast<uint16_t>( animationLength * m_PreviewSlowdownRate );
		ppu->DebugDrawText( previewHeaderStart + textOffset * 3, "Preview frames: %i", effectiveFrames );
	}

	//
	// Editing header
	{
		gfx::PPUCoord const editingHeaderStart = gfx::PPUCoord( verticalPlaneX, 0 ) + headerOffset;
		ppu->DebugDrawText( editingHeaderStart, "Editing" );
		ppu->DebugDrawText( editingHeaderStart + textOffset, "Frame: %i / [0-%i]", m_EditingFrame, numTimeSlots - 1 );
		ppu->DebugDrawText( editingHeaderStart + textOffset * 2, "Sustain: %i </,*> to change", slotSustain );
		file::VFSPath const texPath = texMan.SearchForFilenameByResourceID( editingTextureID );
		if( texPath.Empty() )
		{
			ppu->DebugDrawText( editingHeaderStart + textOffset * 3, "Texture: INVALID" );
		}
		else
		{
			file::VFSPath::Element const lastElement = texPath.GetElement( texPath.NumElements() - 1 );
			ppu->DebugDrawText( editingHeaderStart + textOffset * 3, "Texture: %s", lastElement.c_str() );
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



void FramePackEditor::Command_ChangeSustainCount( bool increase )
{
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
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
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackManager const& fpackMan = *g_Graphics->DebugGetFramePackManager();
	file::VFSPath const filename = fpackMan.SearchForFilenameByResourceName( kFramePackName );
	OpenFramePack( filename );
}



void FramePackEditor::Command_Meta_ChangeDataSpeed( bool faster )
{
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
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
	gfx::FramePackManager& fpackMan = *g_Graphics->DebugGetFramePackManager();
	gfx::TextureManager& texMan = *g_Graphics->DebugGetTextureManager();
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
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->GetResourceFromManagedResourceID( m_FramePackID );
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
	if( m_FramePackID == gfx::k_InvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( m_EditingFrame <= fpack->m_NumTimeSlots );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::FramePackBase::TimeSlot& timeSlot = timeSlots[m_EditingFrame];

	timeSlot.m_TextureOriginX = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginX - x );
	timeSlot.m_TextureOriginY = math::integer_truncast<uint8_t>( timeSlot.m_TextureOriginY - y );
}

///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::OpenFramePack( rftl::string const & rawPath )
{
	file::VFS const& vfs = *file::VFS::HACK_GetInstance();
	file::VFSPath const filePath = vfs.AttemptMapToVFS( rawPath, file::VFSMount::Permissions::ReadOnly );
	OpenFramePack( filePath );
}



void FramePackEditor::OpenFramePack( file::VFSPath const & path )
{
	gfx::FramePackManager& fpackMan = *g_Graphics->DebugGetFramePackManager();
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
}



void FramePackEditor::InsertTimeSlotBefore( size_t slotIndex )
{
	RF_ASSERT( m_FramePackID != gfx::k_InvalidManagedFramePackID );
	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
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
		timeSlots[slotIndex].m_ColliderReference = static_cast<uint64_t>(0);
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

	gfx::FramePackBase* const fpack = g_Graphics->DebugGetFramePackManager()->DebugLockResourceForDirectModification( m_FramePackID );
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
	gfx::PPUController* const ppu = g_Graphics;

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
	file::VFS* const vfs = file::VFS::HACK_GetInstance();
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

#include "stdafx.h"
#include "FramePackEditor.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/FramePackSerDes.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"
#include "PPU/FontManager.h"

#include "PlatformInput_win32/WndProcInputDevice.h"
#include "PlatformUtils_win32/dialogs.h"
#include "PlatformFilesystem/VFS.h"
#include "Logging/Logging.h"

#include "core_platform/shim/winuser_shim.h"
#include "core_vfs/FileBuffer.h"
#include "core_vfs/SeekHandle.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static void OnFramePackOperationToInvalidFramePack()
{
	RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "No framepack set" );
}

}
///////////////////////////////////////////////////////////////////////////////

FramePackEditor::FramePackEditor( WeakPtr<file::VFS> const& vfs )
	: mVfs( vfs )
{
	//
}



void FramePackEditor::Init()
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	mMasterMode = MasterMode::Meta;
	mFramePackID = gfx::ppu::kInvalidManagedFramePackID;
	mEditingFrame = 0;
	mPreviewSlowdownRate = gfx::kTimeSlowdownRate_Normal;
	mPreviewObject = {};

	gfx::FontManager& fontMan = *ppu->DebugGetFontManager();
	file::VFSPath const fonts = file::VFS::kRoot.GetChild( "assets", "fonts", "common" );
	mDefaultFontID = fontMan.LoadNewResource( fonts.GetChild( "font_narrow_1x_mono.fnt.txt" ) );
}



void FramePackEditor::Process()
{
	input::WndProcDigitalInputComponent const& digital = app::gWndProcInput->mDigital;

	using LogicalEvent = input::DigitalInputComponent::LogicalEvent;
	using LogicEvents = rftl::static_vector<LogicalEvent, 1>;
	using LogicEventParser = rftl::virtual_back_inserter_iterator<LogicalEvent, LogicEvents>;
	LogicEvents logicEvents;
	LogicEventParser logicEventParser( logicEvents );
	digital.GetLogicalEventStream( logicEventParser, logicEvents.max_size() );
	uint8_t mostRecentHold = 0;
	if( logicEvents.empty() == false )
	{
		LogicalEvent mostRecentEvent( 0, input::DigitalPinState::Inactive );
		mostRecentEvent = *logicEvents.rbegin();
		if( mostRecentEvent.mNewState == input::DigitalPinState::Active )
		{
			RF_ASSERT( mostRecentEvent.mTime > time::CommonClock::time_point() );
			RF_ASSERT( time::FrameClock::now() >= mostRecentEvent.mTime );
			time::CommonClock::duration const timePassed = time::FrameClock::now() - mostRecentEvent.mTime;
			if( timePassed > rftl::chrono::milliseconds( 200 ) )
			{
				mostRecentHold = mostRecentEvent.mCode;
			}
		}
	}

	if( digital.WasActivatedLogical( 'Z' ) )
	{
		mMasterMode = MasterMode::Meta;
	}
	if( digital.WasActivatedLogical( 'X' ) )
	{
		mMasterMode = MasterMode::Texture;
	}
	if( digital.WasActivatedLogical( 'C' ) )
	{
		mMasterMode = MasterMode::Colliders;
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

	switch( mMasterMode )
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
			if( digital.WasActivatedLogical( 'S' ) )
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					Command_Meta_SaveFramePack();
				}
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
			if( digital.WasActivatedLogical( 'J' ) )
			{
				Command_Texture_ChangeTexture();
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
			//footerText.at( 0 ) =
			//	"[BOX]  "
			//	"<Arrows>:Move box  "
			//	"<CTRL+Arrows>:Resize box  "
			//	"<1-8>:Change layer";
			//footerText.at( 1 ) =
			//	"[SET]  "
			//	"<U>:New set  "
			//	"<O>:Ref old set  "
			//	"<K>:Clone current set  "
			//	"<L>:Ref latest clone";
			//footerText.at( 2 ) =
			//	"[COLLIDERS]  "
			//	"<SHIFT+Arrows>:Change offset  "
			//	"<PGUP/PGDN>:Select box";
			//footerText.at( 3 ) =
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
	gfx::ppu::PPUController* const ppu = app::gGraphics;
	gfx::ppu::FramePackManager const& fpackMan = *ppu->GetFramePackManager();
	gfx::TextureManager const& texMan = *ppu->GetTextureManager();
	input::WndProcDigitalInputComponent const& digital = app::gWndProcInput->mDigital;

	static constexpr uint8_t kFontSize = 8;
	static constexpr gfx::ppu::Coord const kTextOffset( 0, kFontSize );

	static constexpr gfx::ppu::Coord const kHeaderOffset( gfx::ppu::kTileSize / 16, gfx::ppu::kTileSize / 16 );
	static constexpr gfx::ppu::Coord const kPreviewHeaderStart = gfx::ppu::Coord( 0, 0 ) + kHeaderOffset;

	static constexpr size_t kNumFooterLines = 6;
	static constexpr gfx::ppu::Coord const kFooterStart( gfx::ppu::kTileSize / 16, gfx::ppu::kTileSize * gfx::ppu::kDesiredDiagonalTiles - kTextOffset.y * static_cast<gfx::ppu::CoordElem>( kNumFooterLines ) );

	gfx::ppu::CoordElem const horizontalPlaneY = math::SnapNearest<gfx::ppu::CoordElem>( kFooterStart.y, gfx::ppu::kTileSize ) - gfx::ppu::kTileSize;
	gfx::ppu::CoordElem const verticalPlaneX = math::SnapNearest<gfx::ppu::CoordElem>( ppu->GetWidth() / 2, gfx::ppu::kTileSize );
	gfx::ppu::CoordElem const previewOriginX = math::SnapNearest<gfx::ppu::CoordElem>( verticalPlaneX / 2, gfx::ppu::kTileSize );
	gfx::ppu::CoordElem const editingOriginX = math::SnapNearest<gfx::ppu::CoordElem>( verticalPlaneX + verticalPlaneX / 2, gfx::ppu::kTileSize );

	gfx::ppu::Coord const warningStart( previewOriginX - gfx::ppu::kTileSize / 2, horizontalPlaneY - gfx::ppu::kTileSize / 2 );
	rftl::string_view warningText;

	uint8_t animationLength = 0;
	gfx::TimeSlowdownRate preferredSlowdownRate = gfx::kTimeSlowdownRate_Normal;
	uint8_t numTimeSlots = 0;
	uint8_t slotSustain = 0;
	gfx::ppu::Coord texOrigin{ 0, 0 };
	math::Vector2<int64_t> texSize{ 0, 0 };

	//
	// Plane lines
	{
		ppu->DebugDrawLine( gfx::ppu::Coord( 0, horizontalPlaneY ), gfx::ppu::Coord( ppu->GetWidth(), horizontalPlaneY ), 1 );
		ppu->DebugDrawLine( gfx::ppu::Coord( verticalPlaneX, 0 ), gfx::ppu::Coord( verticalPlaneX, horizontalPlaneY ), 1 );
	}

	//
	// Origin point
	{
		gfx::ppu::Coord const editingOriginPoint( editingOriginX, horizontalPlaneY );
		static constexpr gfx::ppu::CoordElem kPointSize = gfx::ppu::kTileSize / 8;
		ppu->DebugDrawLine( editingOriginPoint - kPointSize, editingOriginPoint + kPointSize, 1 );
	}

	//
	// FramePacks
	gfx::ManagedTextureID editingTextureID = gfx::kInvalidDeviceTextureID;
	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		warningText = "No valid frame pack";
	}
	else
	{
		gfx::ppu::FramePackBase const* const fpack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
		RF_ASSERT( fpack != nullptr );

		numTimeSlots = fpack->mNumTimeSlots;
		if( mEditingFrame == numTimeSlots )
		{
			mEditingFrame = 0;
		}
		else if( mEditingFrame > numTimeSlots )
		{
			mEditingFrame = numTimeSlots - 1u;
		}

		gfx::ppu::FramePackBase::TimeSlot const& timeSlot = fpack->GetTimeSlots()[mEditingFrame];

		animationLength = fpack->CalculateTimeIndexBoundary();
		preferredSlowdownRate = fpack->mPreferredSlowdownRate;
		slotSustain = fpack->GetTimeSlotSustains()[mEditingFrame];
		texOrigin.x = timeSlot.mTextureOriginX;
		texOrigin.y = timeSlot.mTextureOriginY;

		mPreviewObject.mFramePackID = mFramePackID;
		mPreviewObject.mTimer.mMaxTimeIndex = animationLength;
		mPreviewObject.mTimer.mTimeSlowdown = mPreviewSlowdownRate;
		mPreviewObject.mLooping = true;
		mPreviewObject.mXCoord = previewOriginX;
		mPreviewObject.mYCoord = horizontalPlaneY;
		mPreviewObject.mZLayer = gfx::ppu::kFarthestLayer;

		ppu->DrawObject( mPreviewObject );
		mPreviewObject.Animate();

		gfx::ppu::Object editingObject = mPreviewObject;
		editingObject.mPaused = true;
		editingObject.mTimer.mTimeIndex = fpack->CalculateFirstTimeIndexOfTimeSlot( mEditingFrame );
		editingObject.mXCoord = editingOriginX;
		editingObject.mYCoord = horizontalPlaneY;
		editingObject.mZLayer = gfx::ppu::kFarthestLayer - 1;

		ppu->DrawObject( editingObject );

		editingTextureID = timeSlot.mTextureReference;
		ppu->ForceImmediateLoadAllRequests();
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
		ppu->DrawText( kPreviewHeaderStart, kFontSize, mDefaultFontID, "Preview" );
		gfx::TimeSlowdownRate const previewFPS = 60u / mPreviewSlowdownRate;
		ppu->DrawText( kPreviewHeaderStart + kTextOffset, kFontSize, mDefaultFontID, "Preview FPS: {} <-/+> to change", previewFPS );
		gfx::TimeSlowdownRate const dataFPS = 60u / preferredSlowdownRate;
		if( mMasterMode == MasterMode::Meta )
		{
			ppu->DrawText( kPreviewHeaderStart + kTextOffset * 2, kFontSize, mDefaultFontID, "Data FPS: {} <Up/Dn> to change", dataFPS );
		}
		else
		{
			ppu->DrawText( kPreviewHeaderStart + kTextOffset * 2, kFontSize, mDefaultFontID, "Data FPS: {}", dataFPS );
		}
		uint16_t const effectivePreviewFrames = math::integer_cast<uint16_t>( animationLength * mPreviewSlowdownRate );
		ppu->DrawText( kPreviewHeaderStart + kTextOffset * 3, kFontSize, mDefaultFontID, "Preview frames: {}", effectivePreviewFrames );
		uint16_t const effectiveDataFrames = math::integer_cast<uint16_t>( animationLength * preferredSlowdownRate );
		ppu->DrawText( kPreviewHeaderStart + kTextOffset * 4, kFontSize, mDefaultFontID, "Data frames: {}", effectiveDataFrames );
	}

	//
	// Editing header
	{
		gfx::ppu::Coord const editingHeaderStart = gfx::ppu::Coord( verticalPlaneX, 0 ) + kHeaderOffset;
		ppu->DrawText( editingHeaderStart, kFontSize, mDefaultFontID, "Editing" );
		if( numTimeSlots == 0 )
		{
			ppu->DrawText( editingHeaderStart + kTextOffset, kFontSize, mDefaultFontID, "Frame: {} / [INVALID] <A/D> to change", mEditingFrame );
		}
		else
		{
			ppu->DrawText( editingHeaderStart + kTextOffset, kFontSize, mDefaultFontID, "Frame: {} / [0-{}] <A/D> to change", mEditingFrame, numTimeSlots - 1 );
		}
		if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
		{
			ppu->DrawText( editingHeaderStart + kTextOffset * 2, kFontSize, mDefaultFontID, "Sustain: {} <Ctrl+/,*> to batch", slotSustain );
		}
		else
		{
			ppu->DrawText( editingHeaderStart + kTextOffset * 2, kFontSize, mDefaultFontID, "Sustain: {} </,*> to change", slotSustain );
		}
		if( editingTextureID == gfx::kInvalidDeviceTextureID )
		{
			ppu->DrawText( editingHeaderStart + kTextOffset * 3, kFontSize, mDefaultFontID, "Texture: UNSET" );
		}
		else
		{
			file::VFSPath const texPath = texMan.SearchForFilenameByResourceID( editingTextureID );
			if( texPath.Empty() )
			{
				ppu->DrawText( editingHeaderStart + kTextOffset * 3, kFontSize, mDefaultFontID, "Texture: INVALID" );
			}
			else
			{
				file::VFSPath::Element const lastElement = texPath.GetElement( texPath.NumElements() - 1 );
				ppu->DrawText( editingHeaderStart + kTextOffset * 3, kFontSize, mDefaultFontID, "Texture: {}", lastElement );
			}
		}
		switch( mMasterMode )
		{
			case MasterMode::Meta:
			{
				ppu->DrawText( editingHeaderStart + kTextOffset * 4, kFontSize, mDefaultFontID, "Origin: {}, {}", texOrigin.x, texOrigin.y );
				break;
			}
			case MasterMode::Texture:
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					ppu->DrawText( editingHeaderStart + kTextOffset * 4, kFontSize, mDefaultFontID, "Origin: {}, {} <Ctrl+Arrows> to batch", texOrigin.x, texOrigin.y );
				}
				else
				{
					ppu->DrawText( editingHeaderStart + kTextOffset * 4, kFontSize, mDefaultFontID, "Origin: {}, {} <Arrows> to change", texOrigin.x, texOrigin.y );
				}
				ppu->DrawText( editingHeaderStart + kTextOffset * 5, kFontSize, mDefaultFontID, "Size: {}, {}", texSize.x, texSize.y );
				break;
			}
			case MasterMode::Colliders:
			{
				ppu->DrawText( editingHeaderStart + kTextOffset * 4, kFontSize, mDefaultFontID, "TODO" );
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
		rftl::array<rftl::string_view, kNumFooterLines> footerText = {};
		switch( mMasterMode )
		{
			case MasterMode::Meta:
			{
				footerText.at( 2 ) =
					"[META]  "
					"<U>:New FPack  "
					"<O>:Open FPack  "
					"<Ctrl+S>:Save FPack";
				footerText.at( 3 ) =
					"[META]  "
					"<Up/Dwn>:Change preferred framerate  "
					"<DEL>:Delete frame";
				break;
			}
			case MasterMode::Texture:
			{
				footerText.at( 2 ) =
					"[TEXTURE]  "
					"<B>:Insert before  "
					"<J>:Change current  "
					"<M>:Insert after";
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					footerText.at( 3 ) =
						"[TEXTURE]  "
						"<Arrows>:Change offset  "
						"<DEL>:Delete frame  ";
				}
				else
				{
					footerText.at( 3 ) =
						"[TEXTURE]  "
						"<Ctrl+Arrows>:Batch offset  "
						"<DEL>:Delete frame  ";
				}
				break;
			}
			case MasterMode::Colliders:
			{
				footerText.at( 0 ) =
					"[BOX]  "
					"<Arrows>:Move box  "
					"<CTRL+Arrows>:Resize box  "
					"<1-8>:Change layer";
				footerText.at( 1 ) =
					"[SET]  "
					"<U>:New set  "
					"<O>:Ref old set  "
					"<K>:Clone current set  "
					"<L>:Ref latest clone";
				footerText.at( 2 ) =
					"[COLLIDERS]  "
					"<SHIFT+Arrows>:Change offset  "
					"<PGUP/PGDN>:Select box";
				footerText.at( 3 ) =
					"[COLLIDERS]  "
					"<N>:New box  "
					"<BACK>:Delete box  "
					"<DEL>:Delete frame";
				break;
			}
			default:
				RF_DBGFAIL();
				break;
		}
		footerText.at( 4 ) =
			"<R>:Reload fpack  "
			"<A/D>:Change frame  "
			"<SPACE>:Snap to preview";
		footerText.at( 5 ) =
			"<Z>:Meta mode  "
			"<X>:Texture mode  "
			"<C>:Collider mode";
		for( size_t i = 0; i < kNumFooterLines; i++ )
		{
			gfx::ppu::Coord const pos =
				kFooterStart + kTextOffset * math::integer_cast<gfx::ppu::CoordElem>( i );
			rftl::string_view const text = footerText.at( i );
			if( text.empty() == false )
			{
				ppu->DrawText( pos, kFontSize, mDefaultFontID, "{}", text );
			}
		}
	}

	//
	// Warnings
	if( warningText.empty() == false )
	{
		ppu->DrawText(
			warningStart,
			gfx::ppu::kNearestLayer,
			kFontSize,
			mDefaultFontID,
			true,
			math::Color3u8::kYellow,
			"{}",
			warningText );
	}
}



void FramePackEditor::Command_ChangePreviewSpeed( bool faster )
{
	if( faster )
	{
		mPreviewSlowdownRate--;
	}
	else
	{
		mPreviewSlowdownRate++;
	}
	mPreviewSlowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, mPreviewSlowdownRate, 60 );
}



void FramePackEditor::Command_ChangeSustainCount( bool increase )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();
	RF_ASSERT( mEditingFrame < fpack->mNumTimeSlots );
	uint8_t& sustain = timeSlotSustains[mEditingFrame];
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
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t* const timeSlotSustains = fpack->GetMutableTimeSlotSustains();
	for( size_t i = 0; i < fpack->mNumTimeSlots; i++ )
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
		mEditingFrame++;
	}
	else
	{
		mEditingFrame--;
	}
}



void FramePackEditor::Command_ReloadFramePack()
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackManager const& fpackMan = *ppu->GetFramePackManager();
	file::VFSPath const filename = fpackMan.SearchForFilenameByResourceName( kFramePackName );
	OpenFramePack( filename );
}



void FramePackEditor::Command_Meta_ChangeDataSpeed( bool faster )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	gfx::TimeSlowdownRate& slowdownRate = fpack->mPreferredSlowdownRate;
	if( faster )
	{
		slowdownRate--;
	}
	else
	{
		slowdownRate++;
	}
	slowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, slowdownRate, 60 );
}



void FramePackEditor::Command_Meta_CreateFramePack()
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	gfx::ppu::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	gfx::TextureManager& texMan = *ppu->DebugGetTextureManager();
	if( mFramePackID != gfx::ppu::kInvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	UniquePtr<gfx::ppu::FramePackBase> newFPack = DefaultCreator<gfx::ppu::FramePack_256>::Create();
	file::VFSPath const defaultFrame = file::VFS::kRoot.GetChild( "assets", "textures", "common", "max_delta_32.png" );
	newFPack->mNumTimeSlots = 1;
	newFPack->GetMutableTimeSlots()[0].mTextureReference = texMan.LoadNewResourceGetID( kInitialTextureName, defaultFrame );
	mFramePackID = fpackMan.LoadNewResourceGetID( kFramePackName, rftl::move( newFPack ) );
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



void FramePackEditor::Command_Meta_SaveFramePack()
{
	rftl::string rawPath = platform::dialogs::SaveFileDialog( { { "FramePack (*.fpack)", "*.fpack" } } );
	if( rawPath.empty() )
	{
		// User cancelled?
		return;
	}

	if( rawPath.ends_with( ".fpack" ) == false )
	{
		rawPath.append( ".fpack" );
	}

	SaveFramePack( rawPath );
}



void FramePackEditor::Command_Meta_DeleteFrame()
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->GetResourceFromManagedResourceID( mFramePackID );
	RF_ASSERT( fpack != nullptr );
	size_t const& numSlots = fpack->mNumTimeSlots;
	if( numSlots <= 1 )
	{
		return;
	}

	RemoveTimeSlotAt( mEditingFrame );
}



void FramePackEditor::Command_Texture_InsertBefore()
{
	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	InsertTimeSlotBefore( mEditingFrame );
	bool const success = ChangeTexture( mEditingFrame );
	if( success == false )
	{
		// Something failed, undo the insert
		RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "Failed to insert texture, undoing insert" );
		RemoveTimeSlotAt( mEditingFrame );
	}
}



void FramePackEditor::Command_Texture_InsertAfter()
{
	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	InsertTimeSlotBefore( mEditingFrame + 1u );
	mEditingFrame = mEditingFrame + 1u;
	bool const success = ChangeTexture( mEditingFrame );
	if( success == false )
	{
		// Something failed, undo the insert
		RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "Failed to insert texture, undoing insert" );
		RemoveTimeSlotAt( mEditingFrame );
		RF_ASSERT( mEditingFrame > 0u );
		mEditingFrame = mEditingFrame - 1u;
	}
}



void FramePackEditor::Command_Texture_ChangeTexture()
{
	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	bool const success = ChangeTexture( mEditingFrame );
	if( success == false )
	{
		// Something failed, hopefully it's left unchanged
		RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "Failed to insert texture" );
	}
}



void FramePackEditor::Command_Texture_ChangeOffset( gfx::ppu::CoordElem x, gfx::ppu::CoordElem y )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( mEditingFrame <= fpack->mNumTimeSlots );

	gfx::ppu::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::ppu::FramePackBase::TimeSlot& timeSlot = timeSlots[mEditingFrame];

	timeSlot.mTextureOriginX = math::integer_truncast<int8_t>( timeSlot.mTextureOriginX - x );
	timeSlot.mTextureOriginY = math::integer_truncast<int8_t>( timeSlot.mTextureOriginY - y );
}



void FramePackEditor::Command_Texture_BatchChangeOffset( gfx::ppu::CoordElem x, gfx::ppu::CoordElem y )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::ppu::kInvalidManagedFramePackID )
	{
		details::OnFramePackOperationToInvalidFramePack();
		return;
	}

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	gfx::ppu::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	for( size_t i = 0; i < fpack->mNumTimeSlots; i++ )
	{
		gfx::ppu::FramePackBase::TimeSlot& timeSlot = timeSlots[i];

		timeSlot.mTextureOriginX = math::integer_truncast<int8_t>( timeSlot.mTextureOriginX - x );
		timeSlot.mTextureOriginY = math::integer_truncast<int8_t>( timeSlot.mTextureOriginY - y );
	}
}

///////////////////////////////////////////////////////////////////////////////

void FramePackEditor::OpenFramePack( rftl::string const& rawPath )
{
	file::VFS const& vfs = *mVfs;
	file::VFSPath const filePath = vfs.AttemptMapToVFS( rawPath, file::VFSMount::Permissions::ReadOnly );
	OpenFramePack( filePath );
}



void FramePackEditor::OpenFramePack( file::VFSPath const& path )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	gfx::ppu::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	if( mFramePackID != gfx::ppu::kInvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	if( path.Empty() )
	{
		RFLOG_ERROR( path, RFCAT_FRAMEPACKEDITOR, "Invalid frame pack filename" );
		mFramePackID = gfx::ppu::kInvalidManagedFramePackID;
		return;
	}
	mFramePackID = fpackMan.LoadNewResourceGetID( kFramePackName, path );

	if( mFramePackID != gfx::ppu::kInvalidManagedFramePackID )
	{
		gfx::ppu::FramePackBase const* fpack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
		RF_ASSERT( fpack != nullptr );
		mPreviewSlowdownRate = math::Clamp<gfx::TimeSlowdownRate>( 1, fpack->mPreferredSlowdownRate, 60 );
	}
}



void FramePackEditor::SaveFramePack( rftl::string const& rawPath )
{
	file::VFS const& vfs = *mVfs;
	file::VFSPath const filePath = vfs.AttemptMapToVFS( rawPath, file::VFSMount::Permissions::ReadWrite );
	if( filePath.Empty() )
	{
		// NOTE: Won't error out, but not the expected use case
		RFLOG_NOTIFY( filePath, RFCAT_FRAMEPACKEDITOR, "Unable to map to VFS" );
	}

	gfx::ppu::PPUController const& ppu = *app::gGraphics;
	gfx::ppu::FramePackManager const& fpackMan = *ppu.DebugGetFramePackManager();
	gfx::TextureManager const& texMan = *ppu.DebugGetTextureManager();

	WeakPtr<gfx::ppu::FramePackBase> const currentFPack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
	if( currentFPack == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEPACKEDITOR, "Unable to fetch current framepack" );
		return;
	}

	// Serialize
	rftl::vector<uint8_t> buffer;
	bool const writeSuccess = gfx::ppu::FramePackSerDes::SerializeToBuffer( texMan, buffer, *currentFPack );
	RF_ASSERT( writeSuccess );

	// Create file
	{
		file::SeekHandlePtr const fileHandle = vfs.GetRawFileForWrite( rawPath );
		if( fileHandle == nullptr )
		{
			RFLOG_NOTIFY( rawPath, RFCAT_FRAMEPACKEDITOR, "Failed to create FPack file" );
			return;
		}

		// Write file
		size_t const bytesWritten = fileHandle->WriteBytes( buffer.data(), buffer.size() );
		RF_ASSERT( bytesWritten == buffer.size() );
	}
}



void FramePackEditor::InsertTimeSlotBefore( size_t slotIndex )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	RF_ASSERT( mFramePackID != gfx::ppu::kInvalidManagedFramePackID );
	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->mNumTimeSlots;
	RF_ASSERT( numSlots + 1 <= fpack->mMaxTimeSlots );

	gfx::ppu::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
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
		timeSlots[slotIndex].mTextureReference = gfx::kInvalidManagedTextureID;
		// TODO: Typedef
		timeSlots[slotIndex].mColliderReference = 0;
		timeSlotSustains[slotIndex] = timeSlotSustains[slotIndex - 1];
	}
	else
	{
		timeSlots[slotIndex] = {};
		timeSlotSustains[slotIndex] = 1;
	}

	fpack->mNumTimeSlots++;
}



void FramePackEditor::RemoveTimeSlotAt( size_t slotIndex )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->mNumTimeSlots;
	RF_ASSERT( numSlots - 1 > 0 );

	gfx::ppu::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
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

	fpack->mNumTimeSlots--;
}



bool FramePackEditor::ChangeTexture( size_t slotIndex )
{
	gfx::ppu::PPUController* const ppu = app::gGraphics;

	RF_ASSERT( mFramePackID != gfx::ppu::kInvalidManagedFramePackID );
	gfx::ppu::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( slotIndex < fpack->mNumTimeSlots );
	gfx::ppu::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::ManagedTextureID& textureID = timeSlots[slotIndex].mTextureReference;

	// User needs to select texture
	rftl::string const filepath = platform::dialogs::OpenFileDialog();
	if( filepath.empty() )
	{
		// User probably cancelled
		RFLOG_WARNING( nullptr, RFCAT_FRAMEPACKEDITOR, "No file selected for texture" );
		return false;
	}

	// Need to map user choice into VFS
	file::VFS* const vfs = mVfs;
	file::VFSPath mappedPath = vfs->AttemptMapToVFS( filepath, file::VFSMount::Permissions::ReadOnly );
	if( mappedPath.Empty() )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEPACKEDITOR, "Unable to map to VFS" );
		return false;
	}

	gfx::TextureManager* const texMan = ppu->DebugGetTextureManager();
	textureID = texMan->LoadNewResourceGetID( mappedPath );

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

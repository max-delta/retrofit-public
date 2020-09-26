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
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"
#include "Logging/Logging.h"

#include "core_platform/shim/winuser_shim.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/static_vector.h"
#include "rftl/extension/string_compare.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

FramePackEditor::FramePackEditor( WeakPtr<file::VFS> const& vfs )
	: mVfs( vfs )
{
	//
}



void FramePackEditor::Init()
{
	gfx::PPUController* const ppu = app::gGraphics;

	mMasterMode = MasterMode::Meta;
	mFramePackID = gfx::kInvalidManagedFramePackID;
	mEditingFrame = 0;
	mPreviewSlowdownRate = gfx::kTimeSlowdownRate_Normal;
	mPreviewObject = {};

	gfx::FontManager& fontMan = *ppu->DebugGetFontManager();
	file::VFSPath const fonts = file::VFS::kRoot.GetChild( "assets", "fonts", "common" );
	mDefaultFontID = fontMan.LoadNewResource( fonts.GetChild( "font_narrow_1x.fnt.txt" ) );
}



void FramePackEditor::Process()
{
	input::WndProcDigitalInputComponent const& digital = app::gWndProcInput->mDigital;

	typedef input::DigitalInputComponent::LogicalEvent LogicalEvent;
	typedef rftl::static_vector<LogicalEvent, 1> LogicEvents;
	typedef rftl::virtual_back_inserter_iterator<LogicalEvent, LogicEvents> LogicEventParser;
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
	gfx::PPUController* const ppu = app::gGraphics;
	gfx::FramePackManager const& fpackMan = *ppu->GetFramePackManager();
	gfx::TextureManager const& texMan = *ppu->GetTextureManager();
	input::WndProcDigitalInputComponent const& digital = app::gWndProcInput->mDigital;

	constexpr uint8_t fontSize = 8;
	gfx::PPUCoord const textOffset( 0, fontSize );

	gfx::PPUCoord const headerOffset( gfx::kTileSize / 16, gfx::kTileSize / 16 );
	gfx::PPUCoord const previewHeaderStart = gfx::PPUCoord( 0, 0 ) + headerOffset;

	constexpr size_t k_NumFooterLines = 6;
	gfx::PPUCoord const footerStart( gfx::kTileSize / 16, gfx::kTileSize * gfx::kDesiredDiagonalTiles - textOffset.y * math::integer_cast<gfx::PPUCoordElem>( k_NumFooterLines ) );

	gfx::PPUCoordElem const horizontalPlaneY = math::SnapNearest( footerStart.y, gfx::kTileSize ) - gfx::kTileSize;
	gfx::PPUCoordElem const verticalPlaneX = math::SnapNearest<gfx::PPUCoordElem>( ppu->GetWidth() / 2, gfx::kTileSize );
	gfx::PPUCoordElem const previewOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX / 2, gfx::kTileSize );
	gfx::PPUCoordElem const editingOriginX = math::SnapNearest<gfx::PPUCoordElem>( verticalPlaneX + verticalPlaneX / 2, gfx::kTileSize );

	uint8_t animationLength = 0;
	gfx::TimeSlowdownRate preferredSlowdownRate = gfx::kTimeSlowdownRate_Normal;
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
		constexpr gfx::PPUCoordElem pointSize = gfx::kTileSize / 8;
		ppu->DebugDrawLine( editingOriginPoint - pointSize, editingOriginPoint + pointSize, 1 );
	}

	//
	// FramePacks
	gfx::ManagedTextureID editingTextureID = gfx::kInvalidDeviceTextureID;
	if( mFramePackID != gfx::kInvalidManagedFramePackID )
	{
		gfx::FramePackBase const* const fpack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
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

		gfx::FramePackBase::TimeSlot const& timeSlot = fpack->GetTimeSlots()[mEditingFrame];

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
		mPreviewObject.mZLayer = gfx::kFarthestLayer;

		ppu->DrawObject( mPreviewObject );
		mPreviewObject.Animate();

		gfx::Object editingObject = mPreviewObject;
		editingObject.mPaused = true;
		editingObject.mTimer.mTimeIndex = fpack->CalculateFirstTimeIndexOfTimeSlot( mEditingFrame );
		editingObject.mXCoord = editingOriginX;
		editingObject.mYCoord = horizontalPlaneY;
		editingObject.mZLayer = gfx::kFarthestLayer - 1;

		ppu->DrawObject( editingObject );

		editingTextureID = timeSlot.mTextureReference;
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
		ppu->DrawText( previewHeaderStart, fontSize, mDefaultFontID, "Preview" );
		gfx::TimeSlowdownRate const previewFPS = 60u / mPreviewSlowdownRate;
		ppu->DrawText( previewHeaderStart + textOffset, fontSize, mDefaultFontID, "Preview FPS: %i <-/+> to change", previewFPS );
		gfx::TimeSlowdownRate const dataFPS = 60u / preferredSlowdownRate;
		if( mMasterMode == MasterMode::Meta )
		{
			ppu->DrawText( previewHeaderStart + textOffset * 2, fontSize, mDefaultFontID, "Data FPS: %i <Up/Dn> to change", dataFPS );
		}
		else
		{
			ppu->DrawText( previewHeaderStart + textOffset * 2, fontSize, mDefaultFontID, "Data FPS: %i", dataFPS );
		}
		uint16_t const effectivePreviewFrames = math::integer_cast<uint16_t>( animationLength * mPreviewSlowdownRate );
		ppu->DrawText( previewHeaderStart + textOffset * 3, fontSize, mDefaultFontID, "Preview frames: %i", effectivePreviewFrames );
		uint16_t const effectiveDataFrames = math::integer_cast<uint16_t>( animationLength * preferredSlowdownRate );
		ppu->DrawText( previewHeaderStart + textOffset * 4, fontSize, mDefaultFontID, "Data frames: %i", effectiveDataFrames );
	}

	//
	// Editing header
	{
		gfx::PPUCoord const editingHeaderStart = gfx::PPUCoord( verticalPlaneX, 0 ) + headerOffset;
		ppu->DrawText( editingHeaderStart, fontSize, mDefaultFontID, "Editing" );
		ppu->DrawText( editingHeaderStart + textOffset, fontSize, mDefaultFontID, "Frame: %i / [0-%i] <A/D> to change", mEditingFrame, numTimeSlots - 1 );
		if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
		{
			ppu->DrawText( editingHeaderStart + textOffset * 2, fontSize, mDefaultFontID, "Sustain: %i <Ctrl+/,*> to batch", slotSustain );
		}
		else
		{
			ppu->DrawText( editingHeaderStart + textOffset * 2, fontSize, mDefaultFontID, "Sustain: %i </,*> to change", slotSustain );
		}
		file::VFSPath const texPath = texMan.SearchForFilenameByResourceID( editingTextureID );
		if( texPath.Empty() )
		{
			ppu->DrawText( editingHeaderStart + textOffset * 3, fontSize, mDefaultFontID, "Texture: INVALID" );
		}
		else
		{
			file::VFSPath::Element const lastElement = texPath.GetElement( texPath.NumElements() - 1 );
			ppu->DrawText( editingHeaderStart + textOffset * 3, fontSize, mDefaultFontID, "Texture: %s", lastElement.c_str() );
		}
		switch( mMasterMode )
		{
			case MasterMode::Meta:
			{
				ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, mDefaultFontID, "Origin: %i, %i", texOrigin.x, texOrigin.y );
				break;
			}
			case MasterMode::Texture:
			{
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, mDefaultFontID, "Origin: %i, %i <Ctrl+Arrows> to batch", texOrigin.x, texOrigin.y );
				}
				else
				{
					ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, mDefaultFontID, "Origin: %i, %i <Arrows> to change", texOrigin.x, texOrigin.y );
				}
				ppu->DrawText( editingHeaderStart + textOffset * 5, fontSize, mDefaultFontID, "Size: %lli, %lli", texSize.x, texSize.y );
				break;
			}
			case MasterMode::Colliders:
			{
				ppu->DrawText( editingHeaderStart + textOffset * 4, fontSize, mDefaultFontID, "TODO" );
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
		switch( mMasterMode )
		{
			case MasterMode::Meta:
			{
				constexpr char k_Footer3Meta[] =
					"[META]  "
					"<U>:New FPack  "
					"<O>:Open FPack "
					"<Ctrl+S>:Save FPack ";
				constexpr char k_Footer4Meta[] =
					"[META]  "
					"<Up/Dwn>:Change preferred framerate  "
					"<DEL>:Delete frame";
				ppu->DrawText( footerLine3Start, fontSize, mDefaultFontID, k_Footer3Meta );
				ppu->DrawText( footerLine4Start, fontSize, mDefaultFontID, k_Footer4Meta );
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
				ppu->DrawText( footerLine3Start, fontSize, mDefaultFontID, k_Footer3Texture );
				if( digital.GetCurrentLogicalState( shim::VK_CONTROL ) )
				{
					ppu->DrawText( footerLine4Start, fontSize, mDefaultFontID, k_FooterAlt4Texture );
				}
				else
				{
					ppu->DrawText( footerLine4Start, fontSize, mDefaultFontID, k_Footer4Texture );
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
				ppu->DrawText( footerLine1Start, fontSize, mDefaultFontID, k_Footer1Colliders );
				ppu->DrawText( footerLine2Start, fontSize, mDefaultFontID, k_Footer2Colliders );
				ppu->DrawText( footerLine3Start, fontSize, mDefaultFontID, k_Footer3Colliders );
				ppu->DrawText( footerLine4Start, fontSize, mDefaultFontID, k_Footer4Colliders );
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
		ppu->DrawText( footerLine5Start, fontSize, mDefaultFontID, k_Footer5 );
		ppu->DrawText( footerLine6Start, fontSize, mDefaultFontID, k_Footer6 );
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
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
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
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
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
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackManager const& fpackMan = *ppu->GetFramePackManager();
	file::VFSPath const filename = fpackMan.SearchForFilenameByResourceName( kFramePackName );
	OpenFramePack( filename );
}



void FramePackEditor::Command_Meta_ChangeDataSpeed( bool faster )
{
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	uint8_t& slowdownRate = fpack->mPreferredSlowdownRate;
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
	gfx::PPUController* const ppu = app::gGraphics;

	gfx::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	gfx::TextureManager& texMan = *ppu->DebugGetTextureManager();
	if( mFramePackID != gfx::kInvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	UniquePtr<gfx::FramePackBase> newFPack = DefaultCreator<gfx::FramePack_256>::Create();
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

	if( rftl::ends_with( rawPath, ".fpack" ) == false )
	{
		rawPath.append( ".fpack" );
	}

	SaveFramePack( rawPath );
}



void FramePackEditor::Command_Meta_DeleteFrame()
{
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->GetResourceFromManagedResourceID( mFramePackID );
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
	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	InsertTimeSlotBefore( mEditingFrame );
	ChangeTexture( mEditingFrame );
}



void FramePackEditor::Command_Texture_InsertAfter()
{
	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	InsertTimeSlotBefore( mEditingFrame + 1u );
	mEditingFrame = mEditingFrame + 1u;
	ChangeTexture( mEditingFrame );
}



void FramePackEditor::Command_Texture_ChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y )
{
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( mEditingFrame <= fpack->mNumTimeSlots );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::FramePackBase::TimeSlot& timeSlot = timeSlots[mEditingFrame];

	timeSlot.mTextureOriginX = math::integer_truncast<uint8_t>( timeSlot.mTextureOriginX - x );
	timeSlot.mTextureOriginY = math::integer_truncast<uint8_t>( timeSlot.mTextureOriginY - y );
}



void FramePackEditor::Command_Texture_BatchChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y )
{
	gfx::PPUController* const ppu = app::gGraphics;

	if( mFramePackID == gfx::kInvalidManagedFramePackID )
	{
		return;
	}

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	for( size_t i = 0; i < fpack->mNumTimeSlots; i++ )
	{
		gfx::FramePackBase::TimeSlot& timeSlot = timeSlots[i];

		timeSlot.mTextureOriginX = math::integer_truncast<uint8_t>( timeSlot.mTextureOriginX - x );
		timeSlot.mTextureOriginY = math::integer_truncast<uint8_t>( timeSlot.mTextureOriginY - y );
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
	gfx::PPUController* const ppu = app::gGraphics;

	gfx::FramePackManager& fpackMan = *ppu->DebugGetFramePackManager();
	if( mFramePackID != gfx::kInvalidManagedFramePackID )
	{
		fpackMan.DestroyResource( kFramePackName );
	}
	if( path.Empty() )
	{
		RFLOG_ERROR( path, RFCAT_FRAMEPACKEDITOR, "Invalid frame pack filename" );
		mFramePackID = gfx::kInvalidManagedFramePackID;
		return;
	}
	mFramePackID = fpackMan.LoadNewResourceGetID( kFramePackName, path );

	if( mFramePackID != gfx::kInvalidManagedFramePackID )
	{
		gfx::FramePackBase const* fpack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
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

	gfx::PPUController const& ppu = *app::gGraphics;
	gfx::FramePackManager const& fpackMan = *ppu.DebugGetFramePackManager();
	gfx::TextureManager const& texMan = *ppu.DebugGetTextureManager();

	WeakPtr<gfx::FramePackBase> const currentFPack = fpackMan.GetResourceFromManagedResourceID( mFramePackID );
	if( currentFPack == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEPACKEDITOR, "Unable to fetch current framepack" );
		return;
	}

	// Serialize
	rftl::vector<uint8_t> buffer;
	bool const writeSuccess = gfx::FramePackSerDes::SerializeToBuffer( texMan, buffer, *currentFPack );
	RF_ASSERT( writeSuccess );

	// Create file
	{
		file::FileHandlePtr const fileHandle = vfs.GetRawFileForWrite( rawPath.c_str() );
		if( fileHandle == nullptr )
		{
			RFLOG_NOTIFY( rawPath.c_str(), RFCAT_FRAMEPACKEDITOR, "Failed to create FPack file" );
			return;
		}

		// Write file
		FILE* const file = fileHandle->GetFile();
		RF_ASSERT( file != nullptr );
		size_t const bytesWritten = fwrite( buffer.data(), sizeof( decltype( buffer )::value_type ), buffer.size(), file );
		RF_ASSERT( bytesWritten == buffer.size() );
	}
}



void FramePackEditor::InsertTimeSlotBefore( size_t slotIndex )
{
	gfx::PPUController* const ppu = app::gGraphics;

	RF_ASSERT( mFramePackID != gfx::kInvalidManagedFramePackID );
	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->mNumTimeSlots;
	RF_ASSERT( numSlots + 1 <= fpack->mMaxTimeSlots );

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
		timeSlots[slotIndex].mTextureReference = gfx::kInvalidManagedTextureID;
		// TODO: Typedef
		timeSlots[slotIndex].mColliderReference = static_cast<uint64_t>( 0 );
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
	gfx::PPUController* const ppu = app::gGraphics;

	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	size_t const& numSlots = fpack->mNumTimeSlots;
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

	fpack->mNumTimeSlots--;
}



void FramePackEditor::ChangeTexture( size_t slotIndex )
{
	gfx::PPUController* const ppu = app::gGraphics;

	RF_ASSERT( mFramePackID != gfx::kInvalidManagedFramePackID );
	gfx::FramePackBase* const fpack = ppu->DebugGetFramePackManager()->DebugLockResourceForDirectModification( mFramePackID );
	RF_ASSERT( fpack != nullptr );

	RF_ASSERT( slotIndex < fpack->mNumTimeSlots );
	gfx::FramePackBase::TimeSlot* const timeSlots = fpack->GetMutableTimeSlots();
	gfx::ManagedTextureID& textureID = timeSlots[slotIndex].mTextureReference;

	// User needs to select texture
	rftl::string const filepath = platform::dialogs::OpenFileDialog();
	if( filepath.empty() )
	{
		// User probably cancelled
		return;
	}

	// Need to map user choice into VFS
	file::VFS* const vfs = mVfs;
	file::VFSPath mappedPath = vfs->AttemptMapToVFS( filepath, file::VFSMount::Permissions::ReadOnly );
	if( mappedPath.Empty() )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_FRAMEPACKEDITOR, "Unable to map to VFS" );
		return;
	}

	gfx::TextureManager* const texMan = ppu->DebugGetTextureManager();
	textureID = texMan->LoadNewResourceGetID( mappedPath );
}

///////////////////////////////////////////////////////////////////////////////
}

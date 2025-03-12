#include "stdafx.h"
#include "PPUController.h"

#include "PPU/DeviceInterface.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/TilesetManager.h"
#include "PPU/Tileset.h"
#include "PPU/Font.h"
#include "PPU/FontManager.h"
#include "PPU/Viewport.h"

#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"
#include "core_math/math_bits.h"
#include "core_math/Lerp.h"
#include "core_math/Rand.h"

#include "core/ptr/default_creator.h"
#include "core/rf_onceper.h"

#include "rftl/extension/variadic_print.h"
#include "rftl/cstdarg"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr DepthLayer kDebugStringLayer = kNearestLayer;
static constexpr DepthLayer kDebugGridLayer = 0;

static constexpr DepthLayer kDefaultTextLayer = 0;
static constexpr DepthLayer kDefaultDebugLineLayer = 0;

}
///////////////////////////////////////////////////////////////////////////////

PPUController::PPUController( UniquePtr<DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs )
	: mDeviceInterface( rftl::move( deviceInterface ) )
	, mTextureManager( nullptr )
	, mFramePackManager( nullptr )
	, mTilesetManager( nullptr )
	, mFontManager( nullptr )
	, mVfs( vfs )
{
	//
}



PPUController::~PPUController()
{
	// TODO: Proper, safe cleanup

	mFramePackManager = nullptr;
	mTextureManager = nullptr;
	mTilesetManager = nullptr;
	mFontManager = nullptr;

	mDeviceInterface->DetachFromWindow();
	mDeviceInterface = nullptr;
}



bool PPUController::Initialize( uint16_t width, uint16_t height )
{
	bool success = true;

	mWidth = width;
	mHeight = height;

	// Create texture manager
	RF_ASSERT( mTextureManager == nullptr );
	mTextureManager = alloc::DefaultAllocCreator<TextureManager>::Create( *alloc::GetAllocator<RFTAG_PPU>(), mVfs );
	success = mTextureManager->AttachToDevice( mDeviceInterface );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Create frame pack manager
	{
		RF_ASSERT( mFramePackManager == nullptr );
		FramePackManager::TextureLoadRefFunc texLoad = //
			[this]( file::VFSPath const& path ) -> ManagedTextureID //
		{
			QueueDeferredLoadRequest( AssetType::Texture, path );
			return mTextureManager->GetManagedResourceIDFromResourceName( path );
		};
		mFramePackManager = alloc::DefaultAllocCreator<FramePackManager>::Create( *alloc::GetAllocator<RFTAG_PPU>(), mVfs, rftl::move( texLoad ) );
	}

	// Create tileset manager
	{
		RF_ASSERT( mTilesetManager == nullptr );
		TilesetManager::TextureLoadRefFunc texLoad = //
			[this]( file::VFSPath const& path ) -> ManagedTextureID //
		{
			QueueDeferredLoadRequest( AssetType::Texture, path );
			return mTextureManager->GetManagedResourceIDFromResourceName( path );
		};
		mTilesetManager = alloc::DefaultAllocCreator<TilesetManager>::Create( *alloc::GetAllocator<RFTAG_PPU>(), mVfs, rftl::move( texLoad ) );
	}

	// Create font manager
	RF_ASSERT( mFontManager == nullptr );
	mFontManager = alloc::DefaultAllocCreator<FontManager>::Create( *alloc::GetAllocator<RFTAG_PPU>(), mVfs );
	success = mFontManager->AttachToDevice( mDeviceInterface );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Prepare device
	success = mDeviceInterface->Initialize2DGraphics();
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}
	success = mDeviceInterface->SetBackgroundColor( math::Color3f::kMagenta );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Set initial size
	success = ResizeSurface( width, height );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Setup buffers
	for( size_t i = 0; i < kNumStateBuffers; i++ )
	{
		mPPUState[i].Clear();
		mPPUDebugState[i].Clear();
	}
	mWriteState = kInvalidStateBufferID;
	mQueueToRenderState = kInvalidStateBufferID;
	mRenderState = kInvalidStateBufferID;

	return true;
}



bool PPUController::ResizeSurface( uint16_t width, uint16_t height )
{
	mWidth = width;
	mHeight = height;
	bool const surfaceSuccess = mDeviceInterface->SetSurfaceSize( width, height );
	RF_ASSERT( surfaceSuccess );
	if( surfaceSuccess == false )
	{
		return false;
	}
	bool const fontSuccess = mDeviceInterface->SetFontScale( GetZoomFactor() );
	RF_ASSERT( fontSuccess );
	if( fontSuccess == false )
	{
		return false;
	}
	return true;
}



CoordElem PPUController::GetWidth() const
{
	return math::integer_cast<CoordElem>( mWidth / GetZoomFactor() );
}



CoordElem PPUController::GetHeight() const
{
	return math::integer_cast<CoordElem>( mHeight / GetZoomFactor() );
}



bool PPUController::BeginFrame()
{
	RF_ASSERT_MSG( mWriteState == kInvalidStateBufferID, "Write buffer wasn't closed" );

	// Find an open buffer for writing
	for( StateBufferID i = 0; i < kNumStateBuffers; i++ )
	{
		if( i == mRenderState )
		{
			// Can't use render state
			continue;
		}
		if( i == mQueueToRenderState )
		{
			// Can't use queued state
			continue;
		}

		// Found an open state, claiming it for write
		mWriteState = i;
		break;
	}
	if( mWriteState == kInvalidStateBufferID )
	{
		RFLOG_FATAL( nullptr, RFCAT_PPU, "Failed to find an open write state to begin render frame" );
	}
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	RF_ASSERT( mWriteState < kNumStateBuffers );
	mPPUState[mWriteState].Clear();
	mPPUDebugState[mWriteState].Clear();

	// TODO: Update timers

	mDeviceInterface->BeginFrame();
	return true;
}



bool PPUController::SubmitToRender()
{
	RF_ASSERT_MSG( mWriteState != kInvalidStateBufferID, "Write buffer wasn't opened" );

	// Close write buffer, and queue it
	StateBufferID const bufferToQueue = mWriteState;
	mWriteState = kInvalidStateBufferID;
	SignalRender( bufferToQueue );

	return true;
}



bool PPUController::WaitForRender()
{
	// TODO: Wait for rendering->complete
	return true;
}



bool PPUController::EndFrame()
{
	// TODO: Set frame as closed
	// TODO: Update timers

	mDeviceInterface->EndFrame();
	return true;
}



void PPUController::SuppressDrawRequests( bool suppress )
{
	mDrawRequestsSuppressed = suppress;
}



void PPUController::UpdateViewportExtents( Viewport& viewport ) const
{
	viewport.mSurfaceExtents = {
		math::integer_cast<CoordElem>( mWidth / GetZoomFactor() ),
		math::integer_cast<CoordElem>( mHeight / GetZoomFactor() ) };
}



void PPUController::ApplyViewport( Viewport const& viewport )
{
	mDrawOffset = -viewport.mOffset;
}



void PPUController::ResetViewport()
{
	mDrawOffset = {};
}



bool PPUController::DrawObject( Object const& object )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	Object& targetObject =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerAssert>(
			targetState.mObjects, targetState.mNumObjects, "Object" );

	targetObject = object;
	targetObject.mXCoord += mDrawOffset.x;
	targetObject.mYCoord += mDrawOffset.y;

	return true;
}



bool PPUController::DrawTileLayer( TileLayer const& tileLayer )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	TileLayer& targetTileLayer =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerAssert>(
			targetState.mTileLayers, targetState.mNumTileLayers, "TileLayer" );

	targetTileLayer = tileLayer;
	targetTileLayer.mXCoord += mDrawOffset.x;
	targetTileLayer.mYCoord += mDrawOffset.y;

	return true;
}



bool PPUController::DrawText( Coord pos, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, details::kDefaultTextLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, math::Color3f color, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, false, color, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, border, color, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	// Make sure we have room for some text before we try to claim a string
	TextStorage<kMaxTextStorage>& textStorage = targetState.mTextStorage;
	rftl::byte_span const targetText = textStorage.GetMutableRemainingTextBuffer();
	if( targetText.empty() )
	{
		RF_DBGFAIL_MSG( "Text storage overflow" );
		return false;
	}

	PPUState::String& targetString =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerAssert>(
			targetState.mStrings, targetState.mNumStrings, "String" );

	targetString.mXCoord = pos.x + mDrawOffset.x;
	targetString.mYCoord = pos.y + mDrawOffset.y;
	targetString.mZLayer = zLayer;
	ConvertColor( targetString.mColor, color );
	targetString.mDesiredHeight = desiredHeight;
	targetString.mBorder = border;
	targetString.mFontReference = font;
	targetString.mTextOffset = textStorage.mTextStorageUsed;
	rftl::string_view const charsWritten = rftl::var_snprintf( &targetText.front<char>(), targetText.size(), fmt, args );
	RF_ASSERT( charsWritten.size() > 0 );
	RF_ASSERT( charsWritten.size() <= kMaxStringLen );
	RF_ASSERT( charsWritten.back() == '\0' );
	textStorage.ConsumeTextBuffer( charsWritten.size() );

	return true;
}



void PPUController::HideZoomFactor( bool hide )
{
	mHideZoomFactor = hide;
}



ZoomFactor PPUController::GetCurrentZoomFactor() const
{
	// TODO: Thread-safe?
	return mHideZoomFactor ? 1u : GetZoomFactor();
}



CoordElem PPUController::CalculateStringLengthFormatted( uint8_t desiredHeight, ManagedFontID fontID, char const* fmt, ... ) const
{
	rftl::array<char, kMaxStringLen> string = {};
	{
		string.front() = '\0';
		va_list args;
		va_start( args, fmt );
		vsnprintf( string.data(), string.size(), fmt, args );
		va_end( args );
		string.back() = '\0';
	}

	char const* const text = string.data();

	return CalculateStringLength( desiredHeight, fontID, text );
}



CoordElem PPUController::CalculateStringLength( uint8_t desiredHeight, ManagedFontID fontID, rftl::string_view text ) const
{
	// !!!WARNING!!! This must be kept logically equivalent to RenderString(...)

	CoordElem retVal = 0;

	Font const* font = mFontManager->GetResourceFromManagedResourceID( fontID );
	RF_ASSERT_MSG( font != nullptr, "Failed to fetch font" );

	uint8_t tileWidth = font->mTileWidth;
	uint8_t tileHeight = font->mTileHeight;
	uint8_t zoomDesired = 0;
	uint8_t shrinkDesired = 0;
	CalculateDesiredFontZoomShrink( *font, desiredHeight, zoomDesired, shrinkDesired );
	RF_ASSERT( zoomDesired >= 1 );
	RF_ASSERT( shrinkDesired >= 1 );
	tileWidth *= zoomDesired;
	tileHeight *= zoomDesired;
	tileWidth /= shrinkDesired;
	tileHeight /= shrinkDesired;
	RF_ASSERT( tileWidth > 0 );
	RF_ASSERT( tileHeight > 0 );

	for( char const& character : text )
	{
		if( character == '\0' )
		{
			break;
		}

		uint8_t charWidth = tileWidth;
		uint8_t charMargin = 0;

		if( font->mSpacingMode == Font::SpacingMode::Variable )
		{
			uint8_t whitespaceWidth = 0;
			CalculateFontVariableWhitespaceWidth( *font, whitespaceWidth );
			CalculateFontVariableCharWidth( *font, character, whitespaceWidth, zoomDesired, shrinkDesired, charWidth );
			charMargin = 1;
		}

		RF_ASSERT( charWidth > 0 );

		retVal += charWidth;
		retVal += charMargin;
	}

	return retVal;
}



Coord PPUController::CalculateTileLayerSize( TileLayer const& tileLayer ) const
{
	Tileset const* tileset = mTilesetManager->GetResourceFromManagedResourceID( tileLayer.mTilesetReference );
	RF_ASSERT_MSG( tileset != nullptr, "Failed to fetch tileset" );

	CoordElem xStep;
	CoordElem yStep;
	CalculateTileSize( tileLayer, *tileset, xStep, yStep );
	CoordElem const xLayerStep = xStep * math::integer_cast<CoordElem>( tileLayer.NumColumns() );
	CoordElem const yLayerStep = yStep * math::integer_cast<CoordElem>( tileLayer.NumRows() );
	return Coord{ xLayerStep, yLayerStep };
}



bool PPUController::HasOutstandingLoadRequests() const
{
	return mDeferredLoadRequests.empty() == false;
}



bool PPUController::QueueDeferredLoadRequest( AssetType type, Filename const& filename )
{
	return QueueDeferredLoadRequest( type, ResourceName{}, filename );
}



bool PPUController::QueueDeferredLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename )
{
	FullfillLoadRequest( LoadRequest{ LoadType::Reserve, type, resourceName, filename } );
	mDeferredLoadRequests.emplace_back( LoadRequest{ LoadType::Modify, type, resourceName, filename } );
	return true;
}



bool PPUController::QueueDeferredReloadRequest( AssetType type, ResourceName const& resourceName )
{
	mDeferredLoadRequests.emplace_back( LoadRequest{ LoadType::Reload, type, resourceName, Filename{} } );
	return true;
}



bool PPUController::ForceImmediateLoadRequest( AssetType type, Filename const& filename )
{
	return ForceImmediateLoadRequest( type, ResourceName{}, filename );
}



bool PPUController::ForceImmediateLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename )
{
	return FullfillLoadRequest( LoadRequest{ LoadType::New, type, resourceName, filename } );
}



bool PPUController::ForceImmediateLoadAllRequests()
{
	FullfillAllDeferredLoadRequests();
	return true;
}




WeakPtr<TextureManager const> PPUController::GetTextureManager() const
{
	return mTextureManager;
}



WeakPtr<FramePackManager const> PPUController::GetFramePackManager() const
{
	return mFramePackManager;
}



WeakPtr<TilesetManager const> PPUController::GetTilesetManager() const
{
	return mTilesetManager;
}



WeakPtr<FontManager const> PPUController::GetFontManager() const
{
	return mFontManager;
}



void PPUController::DebugSetGridEnabled( bool enabled )
{
	mDebugDrawGrid = enabled;
}



void PPUController::DebugSetBackgroundColor( math::Color3f color )
{
	mDeviceInterface->SetBackgroundColor( color );
}



bool PPUController::DebugDrawText( Coord pos, const char* fmt, ... )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	PPUDebugState::DebugString& targetString =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerSilent>(
			targetState.mStrings, targetState.mNumStrings, "DebugString" );

	targetString.mXCoord = pos.x + mDrawOffset.x;
	targetString.mYCoord = pos.y + mDrawOffset.y;
	targetString.mText[0] = '\0';
	{
		va_list args;
		va_start( args, fmt );
		vsnprintf( &targetString.mText[0], PPUDebugState::DebugString::k_MaxLen, fmt, args );
		va_end( args );
	}
	targetString.mText[PPUDebugState::DebugString::k_MaxLen] = '\0';

	return true;
}



bool PPUController::DebugDrawAuxText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DebugDrawAuxText( pos, zLayer, desiredHeight, font, border, color, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DebugDrawAuxText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	// Make sure we have room for some text before we try to claim a string
	TextStorage<kMaxDebugTextStorage>& textStorage = targetState.mTextStorage;
	rftl::byte_span const targetText = textStorage.GetMutableRemainingTextBuffer();
	if( targetText.empty() )
	{
		RF_DBGFAIL_MSG( "Text storage overflow" );
		return false;
	}

	PPUDebugState::DebugAuxString& targetString =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerSilent>(
			targetState.mAuxStrings, targetState.mNumAuxStrings, "DebugAuxString" );

	targetString.mXCoord = pos.x + mDrawOffset.x;
	targetString.mYCoord = pos.y + mDrawOffset.y;
	targetString.mZLayer = zLayer;
	ConvertColor( targetString.mColor, color );
	targetString.mDesiredHeight = desiredHeight;
	targetString.mBorder = border;
	targetString.mFontReference = font;
	targetString.mTextOffset = textStorage.mTextStorageUsed;
	rftl::string_view const charsWritten = rftl::var_snprintf( &targetText.front<char>(), targetText.size(), fmt, args );
	RF_ASSERT( charsWritten.size() > 0 );
	RF_ASSERT( charsWritten.size() <= kMaxStringLen );
	RF_ASSERT( charsWritten.back() == '\0' );
	textStorage.ConsumeTextBuffer( charsWritten.size() );

	return true;
}



bool PPUController::DebugDrawLine( Coord p0, Coord p1 )
{
	return DebugDrawLine( p0, p1, math::Color3f::kBlack );
}



bool PPUController::DebugDrawLine( Coord p0, Coord p1, CoordElem width )
{
	return DebugDrawLine( p0, p1, width, details::kDefaultDebugLineLayer, math::Color3f::kBlack );
}



bool PPUController::DebugDrawLine( Coord p0, Coord p1, math::Color3f color )
{
	return DebugDrawLine( p0, p1, 0, details::kDefaultDebugLineLayer, color );
}



bool PPUController::DebugDrawLine( Coord p0, Coord p1, CoordElem width, DepthLayer zLayer, math::Color3f color )
{
	if( mDrawRequestsSuppressed )
	{
		return true;
	}

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	PPUDebugState::DebugLine& targetLine =
		RenderStateListItemSelect<DrawOverflowBehavior::FlickerSilent>(
			targetState.mLines, targetState.mNumLines, "DebugLine" );

	targetLine.mXCoord0 = p0.x + mDrawOffset.x;
	targetLine.mYCoord0 = p0.y + mDrawOffset.y;
	targetLine.mXCoord1 = p1.x + mDrawOffset.x;
	targetLine.mYCoord1 = p1.y + mDrawOffset.y;
	targetLine.mWidth = width;
	targetLine.mZLayer = zLayer;
	targetLine.mColor = color;

	return true;
}



bool PPUController::DebugDrawAABB( AABB aabb )
{
	return DebugDrawAABB( aabb, 0, details::kDefaultDebugLineLayer, math::Color3f::kBlack );
}



bool PPUController::DebugDrawAABB( AABB aabb, CoordElem width )
{
	return DebugDrawAABB( aabb, width, details::kDefaultDebugLineLayer, math::Color3f::kBlack );
}



bool PPUController::DebugDrawAABB( AABB aabb, math::Color3f color )
{
	return DebugDrawAABB( aabb, 0, details::kDefaultDebugLineLayer, color );
}



bool PPUController::DebugDrawAABB( AABB aabb, CoordElem width, DepthLayer zLayer, math::Color3f color )
{
	DebugDrawLine( aabb.mTopLeft, Coord( aabb.mTopLeft.x, aabb.mBottomRight.y ), width, zLayer, color );
	DebugDrawLine( aabb.mTopLeft, Coord( aabb.mBottomRight.x, aabb.mTopLeft.y ), width, zLayer, color );
	DebugDrawLine( aabb.mBottomRight, Coord( aabb.mTopLeft.x, aabb.mBottomRight.y ), width, zLayer, color );
	DebugDrawLine( aabb.mBottomRight, Coord( aabb.mBottomRight.x, aabb.mTopLeft.y ), width, zLayer, color );

	return true;
}



WeakPtr<DeviceInterface> PPUController::DebugGetDeviceInterface() const
{
	return mDeviceInterface;
}



WeakPtr<TextureManager> PPUController::DebugGetTextureManager() const
{
	return mTextureManager;
}



WeakPtr<FramePackManager> PPUController::DebugGetFramePackManager() const
{
	return mFramePackManager;
}



WeakPtr<TilesetManager> PPUController::DebugGetTilesetManager() const
{
	return mTilesetManager;
}



WeakPtr<FontManager> PPUController::DebugGetFontManager() const
{
	return mFontManager;
}

///////////////////////////////////////////////////////////////////////////////

void PPUController::SignalRender( StateBufferID readyBuffer )
{
	// TODO: If renderer open - push work, if closed - queue work
	RF_ASSERT_MSG( mRenderState == kInvalidStateBufferID, "TODO: Asynchronous rendering" );
	RF_ASSERT_MSG( mQueueToRenderState == kInvalidStateBufferID, "TODO: Asynchronous rendering" );

	// Set buffer as rendering
	mRenderState = readyBuffer;

	// HACK: Block on all deferred loads
	FullfillAllDeferredLoadRequests();

	// HACK: Render now
	Render();
	mRenderState = kInvalidStateBufferID;

	return;
}



void PPUController::Render() const
{
	RF_ASSERT( mRenderState != kInvalidStateBufferID );
	PPUDebugState const& targetDebugState = mPPUDebugState[mRenderState];
	PPUState const& targetState = mPPUState[mRenderState];
	TextStorage<kMaxDebugTextStorage> const& textDebugStorage = targetDebugState.mTextStorage;
	TextStorage<kMaxTextStorage> const& textStorage = targetState.mTextStorage;

	// Transparency and whatnot
	DepthOrder depthOrder = {};
	CalculateDepthOrder( depthOrder );

	bool hasDrawnGrid = false;

	// Render in depth order
	for( DepthElement const& element : depthOrder )
	{
		// Have we reached or crossed over the grid?
		if( mDebugDrawGrid && hasDrawnGrid == false && element.mDepth < details::kDebugGridLayer )
		{
			// Draw grid
			// NOTE: Will be drawn below things that share the same layer, due
			//  to this being immediately after all elements on that layer that
			//  have already written to the depth buffer
			// NOTE: Transparent objects on the same layer will get to write to
			//  the depth buffer first, and so may block the grid from
			//  rendering in those pixels
			RenderDebugGrid();
			hasDrawnGrid = true;
		}

		size_t const i = element.mId;

		bool terminate = false;
		switch( element.mType )
		{
			default:
				RF_DBGFAIL();
				terminate = true;
				break;

			case ElementType::Invalid:
				// End of list
				terminate = true;
				break;

			case ElementType::Object:
				RenderObject( targetState.mObjects[i] );
				break;
			case ElementType::TileLayer:
				RenderTileLayer( targetState.mTileLayers[i] );
				break;
			case ElementType::String:
				RenderString( targetState.mStrings[i],
					textStorage.GetTextBufferForStringOffset( targetState.mStrings[i].mTextOffset ) );
				break;
			case ElementType::DebugLine:
				RenderDebugLine( targetDebugState.mLines[i] );
				break;
			case ElementType::DebugString:
				RenderDebugString( targetDebugState.mStrings[i] );
				break;
			case ElementType::DebugAuxString:
				RenderString( targetDebugState.mAuxStrings[i],
					textDebugStorage.GetTextBufferForStringOffset( targetDebugState.mAuxStrings[i].mTextOffset ) );
				break;
		}
		if( terminate )
		{
			break;
		}
	}

	// Have we drawn the grid yet?
	if( mDebugDrawGrid && hasDrawnGrid == false )
	{
		// Draw grid
		RenderDebugGrid();
		hasDrawnGrid = true;
	}

	mDeviceInterface->RenderFrame();
}



void PPUController::CalculateDepthOrder( DepthOrder& depthOrder ) const
{
	RF_ASSERT( mRenderState != kInvalidStateBufferID );
	PPUDebugState const& targetDebugState = mPPUDebugState[mRenderState];
	PPUState const& targetState = mPPUState[mRenderState];

	size_t i_depthOrder = 0;

	// Objects
	for( size_t i = 0; i < targetState.mNumObjects; i++ )
	{
		Object const& object = targetState.mObjects[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::Object;
		element.mDepth = object.mZLayer;
	}

	// Tile layers
	for( size_t i = 0; i < targetState.mNumTileLayers; i++ )
	{
		TileLayer const& tileLayer = targetState.mTileLayers[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::TileLayer;
		element.mDepth = tileLayer.mZLayer;
	}

	// Text
	for( size_t i = 0; i < targetState.mNumStrings; i++ )
	{
		PPUState::String const& string = targetState.mStrings[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::String;
		element.mDepth = string.mZLayer;
	}

	// Debug lines
	for( size_t i = 0; i < targetDebugState.mNumLines; i++ )
	{
		PPUDebugState::DebugLine const& line = targetDebugState.mLines[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::DebugLine;
		element.mDepth = line.mZLayer;
	}

	// Debug text
	for( size_t i = 0; i < targetDebugState.mNumStrings; i++ )
	{
		//PPUDebugState::DebugString const& string = targetDebugState.mStrings[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::DebugString;
		element.mDepth = details::kDebugStringLayer;
	}

	// Debug aux text
	for( size_t i = 0; i < targetDebugState.mNumAuxStrings; i++ )
	{
		PPUDebugState::DebugAuxString const& string = targetDebugState.mAuxStrings[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::DebugAuxString;
		element.mDepth = string.mZLayer;
	}

	// Sort
	struct Sorter
	{
		static int Sort( void const* lhs, void const* rhs )
		{
			DepthElement const& lhse = *reinterpret_cast<DepthElement const*>( lhs );
			DepthElement const& rhse = *reinterpret_cast<DepthElement const*>( rhs );

			// Invalid elements go last
			if( lhse.mType == ElementType::Invalid && rhse.mType != ElementType::Invalid )
			{
				return 1;
			}
			else if( lhse.mType != ElementType::Invalid && rhse.mType == ElementType::Invalid )
			{
				return -1;
			}
			else if( lhse.mType == ElementType::Invalid && rhse.mType == ElementType::Invalid )
			{
				return 0;
			}

			// Higher depth values are farther from camera, want to draw them
			//  first for transparency purposes
			if( lhse.mDepth < rhse.mDepth )
			{
				return 1;
			}
			else if( lhse.mDepth > rhse.mDepth )
			{
				return -1;
			}

			// Prefer to clump together like types
			if( math::enum_bitcast( lhse.mType ) < math::integer_unsigned_bitcast( rhse.mDepth ) )
			{
				return -1;
			}
			else if( math::enum_bitcast( lhse.mType ) > math::integer_unsigned_bitcast( rhse.mDepth ) )
			{
				return 1;
			}

			// Prefer to iterate in order
			if( lhse.mId < rhse.mId )
			{
				return -1;
			}
			else if( lhse.mId > rhse.mId )
			{
				return 1;
			}

			// Equal? Unclear if this should happen
			RF_DBGFAIL_MSG( "Unclear if this is valid" );
			return 0;
		}
	};
	constexpr size_t kMaxCount = rftl::extent<rftl::remove_reference<decltype( depthOrder )>::type>::value;
	constexpr size_t kSize = sizeof( depthOrder[0] );
	if( i_depthOrder == 0 )
	{
		// No sort needed
		RF_ASSERT( depthOrder[0].mType == ElementType::Invalid );
	}
	else if( i_depthOrder == kMaxCount )
	{
		// Sort everything
		RF_ASSERT( depthOrder[kMaxCount - 1].mType != ElementType::Invalid );
		rftl::qsort( &depthOrder, kMaxCount, kSize, Sorter::Sort );
	}
	else
	{
		// Sort only filled entries
		size_t const count = i_depthOrder;
		RF_ASSERT( depthOrder[count - 1].mType != ElementType::Invalid );
		RF_ASSERT( depthOrder[count].mType == ElementType::Invalid );
		rftl::qsort( &depthOrder, count, kSize, Sorter::Sort );
	}
}



void PPUController::CalculateDesiredFontZoomShrink( Font const& font, uint8_t desiredHeight, uint8_t& zoomDesired, uint8_t& shrinkDesired ) const
{
	uint8_t const& tileHeight = font.mTileHeight;
	zoomDesired = math::integer_cast<uint8_t>( desiredHeight / tileHeight );
	shrinkDesired = 1;
	if( zoomDesired < 1 )
	{
		// Shrink
		zoomDesired = 1;
		shrinkDesired = math::integer_cast<uint8_t>( tileHeight / desiredHeight );
		ZoomFactor const zoomFactor = GetZoomFactor();
		if( ( zoomFactor - shrinkDesired ) < 0 )
		{
			// Font bigger than desired height, can't downscale enough, will
			//  look wonky when rendered even at reduced zoom
			// TODO: Warning?
			shrinkDesired = zoomFactor;
			RF_ASSERT_MSG( shrinkDesired == 1, "Check this assumption?" );
		}

		bool const effectiveZoomIsMisalignedScale = ( zoomFactor % shrinkDesired ) != 0;
		if( effectiveZoomIsMisalignedScale )
		{
			// Won't result in an aligned scale, will look wonky
			// TODO: Warning?
		}
	}
	RF_ASSERT( zoomDesired >= 1 );
	RF_ASSERT( shrinkDesired >= 1 );
}



void PPUController::CalculateFontVariableWhitespaceWidth( Font const& font, uint8_t& whitespaceWidth ) const
{
	whitespaceWidth = font.mTileWidth / 2u;
}



void PPUController::CalculateFontVariableCharWidth( Font const& font, char character, uint8_t whitespaceWidth, uint8_t zoomDesired, uint8_t shrinkDesired, uint8_t& varCharWidth ) const
{
	// NOTE: 'True width' is just for debugging ease
	uint8_t const trueVarCharWidth = math::integer_cast<uint8_t>( font.mVariableWidth.at( static_cast<unsigned char>( character ) ) );
	varCharWidth = trueVarCharWidth;
	if( varCharWidth == 0 )
	{
		// Whitespace?
		varCharWidth = whitespaceWidth;
	}
	RF_ASSERT( varCharWidth > 0 );
	varCharWidth *= zoomDesired;
	RF_ASSERT( varCharWidth > 0 );
	uint8_t const shrinkLoss = math::integer_cast<uint8_t>( varCharWidth % shrinkDesired );
	varCharWidth /= shrinkDesired;
	varCharWidth += shrinkLoss;
	if( varCharWidth < 1 )
	{
		// Cannot be shrunk as much as desired
		varCharWidth = 1;
	}
}



void PPUController::CalculateFontVariableCharHeight( Font const& font, char character, uint8_t zoomDesired, uint8_t shrinkDesired, uint8_t& varCharHeight ) const
{
	// NOTE: 'True height' is just for debugging ease
	uint8_t const trueVarCharHeight = math::integer_cast<uint8_t>( font.mVariableHeight.at( static_cast<unsigned char>( character ) ) );
	varCharHeight = trueVarCharHeight;
	if( varCharHeight == 0 )
	{
		// Whitespace?
		return;
	}
	RF_ASSERT( varCharHeight > 0 );
	varCharHeight *= zoomDesired;
	RF_ASSERT( varCharHeight > 0 );
	uint8_t const shrinkLoss = math::integer_cast<uint8_t>( varCharHeight % shrinkDesired );
	varCharHeight /= shrinkDesired;
	varCharHeight += shrinkLoss;
	if( varCharHeight < 1 )
	{
		// Cannot be shrunk as much as desired
		varCharHeight = 1;
	}
}



void PPUController::CalculateTileSize( TileLayer const& tileLayer, Tileset const& tileset, CoordElem& tileWidth, CoordElem& tileHeight ) const
{
	uint8_t scaleUp;
	uint8_t scaleDown;
	tileLayer.GetTileZoomFactor( scaleUp, scaleDown );
	tileWidth = ( tileset.mTileWidth * scaleUp ) / scaleDown;
	tileHeight = ( tileset.mTileHeight * scaleUp ) / scaleDown;
}



void PPUController::ConvertColor( uint8_t ( &dest )[3], math::Color3f const& src )
{
	dest[0] = math::integer_cast<uint8_t>( src.r * math::float_cast<float>( rftl::numeric_limits<uint8_t>::max() ) );
	dest[1] = math::integer_cast<uint8_t>( src.g * math::float_cast<float>( rftl::numeric_limits<uint8_t>::max() ) );
	dest[2] = math::integer_cast<uint8_t>( src.b * math::float_cast<float>( rftl::numeric_limits<uint8_t>::max() ) );
}



math::Color3f PPUController::ConvertColor( uint8_t const ( &src )[3] )
{
	return //
		math::Color3f{
			math::float_cast<math::Color3f::ElementType>( src[0] ),
			math::float_cast<math::Color3f::ElementType>( src[1] ),
			math::float_cast<math::Color3f::ElementType>( src[2] ) } *
		( 1.f / math::float_cast<float>( rftl::numeric_limits<uint8_t>::max() ) );
}



math::Color3f PPUController::CalculateBorderColor( math::Color3f contentsColor )
{
	// TODO: This is all sketchy, re-evaluate with better test cases
	contentsColor.ClampValues();
	math::Color3f::ElementType const total = contentsColor.r + contentsColor.g + contentsColor.b;
	if( total > .5f * 3 )
	{
		return math::Color3f::kBlack;
	}
	else if( total < .2f * 3 )
	{
		contentsColor.ClampValues( .7f, 1.f );
		return contentsColor;
	}
	else
	{
		contentsColor.ClampLuminance( .2f, .25f );
		return contentsColor;
	}
}



void PPUController::RenderObject( Object const& object ) const
{
	FramePackBase const* const framePack = mFramePackManager->GetResourceFromManagedResourceID( object.mFramePackID );
	RF_ASSERT_MSG( framePack != nullptr, "Invalid frame pack ID" );
	uint8_t const slotIndex = framePack->CalculateTimeSlotFromTimeIndex( object.mTimer.mTimeIndex );
	FramePackBase::TimeSlot const& timeSlot = framePack->GetTimeSlots()[slotIndex];

	if( timeSlot.mTextureReference == kInvalidManagedTextureID )
	{
		// Invisible frame
		return;
	}

	Texture const* texture = mTextureManager->GetResourceFromManagedResourceID( timeSlot.mTextureReference );
	RF_ASSERT_MSG( texture != nullptr, "Failed to fetch texture" );
	DeviceTextureID const deviceTextureID = texture->GetDeviceRepresentation();

	// TODO: Transforms
	CoordElem const x = object.mXCoord - timeSlot.mTextureOriginX;
	CoordElem const y = object.mYCoord - timeSlot.mTextureOriginY;
	math::Vector2f topLeft = CoordToDevice( x, y );
	math::Vector2f bottomRight = CoordToDevice(
		x + math::integer_cast<CoordElem>( texture->mWidthPostLoad ),
		y + math::integer_cast<CoordElem>( texture->mHeightPostLoad ) );

	// Perform flips
	// NOTE: Assuming the device billboards are implemented similarly to a
	//  double-sided quad in a 3D pipeline
	if( object.mHorizontalFlip )
	{
		rftl::swap( topLeft.x, bottomRight.x );
	}
	if( object.mVerticalFlip )
	{
		rftl::swap( topLeft.y, bottomRight.y );
	}

	mDeviceInterface->DrawBillboard( deviceTextureID, math::AABB4f{ topLeft, bottomRight }, LayerToDevice( object.mZLayer ) );
}



void PPUController::RenderTileLayer( TileLayer const& tileLayer ) const
{
	Tileset const* tileset = mTilesetManager->GetResourceFromManagedResourceID( tileLayer.mTilesetReference );
	RF_ASSERT_MSG( tileset != nullptr, "Failed to fetch tileset" );
	Texture const* texture = mTextureManager->GetResourceFromManagedResourceID( tileset->mTextureReference );
	RF_ASSERT_MSG( texture != nullptr, "Failed to fetch texture" );
	DeviceTextureID const deviceTextureID = texture->GetDeviceRepresentation();

	uint16_t const texTilesPerRow = math::integer_cast<uint16_t>( texture->mWidthPostLoad / tileset->mTileWidth );
	float const texXStep = math::float_cast<float>( tileset->mTileWidth ) / math::float_cast<float>( texture->mWidthPostLoad );
	float const texYStep = math::float_cast<float>( tileset->mTileHeight ) / math::float_cast<float>( texture->mHeightPostLoad );

	CoordElem xStep;
	CoordElem yStep;
	CalculateTileSize( tileLayer, *tileset, xStep, yStep );
	CoordElem const xLayerStep = xStep * math::integer_cast<CoordElem>( tileLayer.NumColumns() );
	CoordElem const yLayerStep = yStep * math::integer_cast<CoordElem>( tileLayer.NumRows() );

	float const z = LayerToDevice( tileLayer.mZLayer );

	CoordElem const rootX = tileLayer.mXCoord;
	CoordElem const rootY = tileLayer.mYCoord;

	// TODO: Animations? Probably done via shaders and passing in the timer

	// TODO: Transforms

	// TODO: Flips

	int16_t posColStart = 0;
	int16_t posColEnd = math::integer_cast<int16_t>( tileLayer.NumColumns() );
	int16_t posRowStart = 0;
	int16_t posRowEnd = math::integer_cast<int16_t>( tileLayer.NumRows() );
	CoordElem drawRootX = rootX;
	CoordElem drawRootY = rootY;
	int16_t tileColOffset = 0;
	int16_t tileRowOffset = 0;

	// Wrapping
	if( tileLayer.mWrapping )
	{
		// How far off screen do we need to start rendering?
		CoordElem wrapRootX;
		CoordElem wrapRootY;
		{
			CoordElem const xStepDifferenceFromZero = rootX % xLayerStep;
			CoordElem const yStepDifferenceFromZero = rootY % yLayerStep;
			wrapRootX = xStepDifferenceFromZero;
			wrapRootY = yStepDifferenceFromZero;
			if( wrapRootX > 0 )
			{
				wrapRootX -= xLayerStep;
			}
			if( wrapRootY > 0 )
			{
				wrapRootY -= yLayerStep;
			}
		}
		RF_ASSERT( wrapRootX <= 0 );
		RF_ASSERT( wrapRootY <= 0 );
		drawRootX = wrapRootX;
		drawRootY = wrapRootY;

		// How many cycles of wrapping before we reach the root of the layer?
		int16_t xCyclesBeforeRoot;
		int16_t yCyclesBeforeRoot;
		{
			CoordElem const xWrapDifferenceFromRoot = rootX - wrapRootX;
			CoordElem const yWrapDifferenceFromRoot = rootY - wrapRootY;
			RF_ASSERT( xWrapDifferenceFromRoot % xStep == 0 );
			RF_ASSERT( yWrapDifferenceFromRoot % yStep == 0 );
			xCyclesBeforeRoot = xWrapDifferenceFromRoot / xStep;
			yCyclesBeforeRoot = yWrapDifferenceFromRoot / yStep;
		}
		RF_ASSERT( xCyclesBeforeRoot >= 0 );
		RF_ASSERT( yCyclesBeforeRoot >= 0 );
		tileColOffset = xCyclesBeforeRoot;
		tileRowOffset = yCyclesBeforeRoot;

		// How many cycles of wrapping before we reach the end of the screen?
		int16_t xCyclesBeforeScreenEscape;
		int16_t yCyclesBeforeScreenEscape;
		{
			CoordElem const xWrapDifferenceFromEscape = GetWidth() - wrapRootX;
			CoordElem const yWrapDifferenceFromEscape = GetHeight() - wrapRootY;
			xCyclesBeforeScreenEscape = xWrapDifferenceFromEscape / xStep;
			yCyclesBeforeScreenEscape = yWrapDifferenceFromEscape / yStep;
		}
		RF_ASSERT( xCyclesBeforeScreenEscape >= 0 );
		RF_ASSERT( yCyclesBeforeScreenEscape >= 0 );
		posColEnd = xCyclesBeforeScreenEscape + 1;
		posRowEnd = yCyclesBeforeScreenEscape + 1;
	}

	constexpr auto renderTile =
		[]( DeviceInterface* deviceInterface,
			TileLayer::Tile const& tile,
			DeviceTextureID deviceTextureID,
			uint16_t texTilesPerRow,
			float texXStep,
			float texYStep,
			math::AABB4f pos,
			float z ) -> void //
	{
		RF_ASSERT( tile.mIndex != TileLayer::kEmptyTileIndex );

		math::Vector2i16 const texTile = math::Vector2i16(
			math::integer_cast<int16_t>( tile.mIndex % texTilesPerRow ),
			math::integer_cast<int16_t>( tile.mIndex / texTilesPerRow ) );
		math::Vector2f const texTopLeft = math::Vector2f(
			math::float_cast<float>( texTile.x ) * texXStep,
			math::float_cast<float>( texTile.y ) * texYStep );
		math::Vector2f const texBottomRight = texTopLeft + math::Vector2f( texXStep, texYStep );

		deviceInterface->DrawBillboard( deviceTextureID, pos, z, math::AABB4f{ texTopLeft, texBottomRight } );
	};

	for( int16_t i_col = posColStart; i_col < posColEnd; i_col++ )
	{
		for( int16_t i_row = posRowStart; i_row < posRowEnd; i_row++ )
		{
			size_t const tileCol = math::integer_cast<size_t>( i_col + tileColOffset ) % tileLayer.NumColumns();
			size_t const tileRow = math::integer_cast<size_t>( i_row + tileRowOffset ) % tileLayer.NumRows();
			TileLayer::Tile const& tile = tileLayer.GetTile( tileCol, tileRow );
			if( tile.mIndex == TileLayer::kEmptyTileIndex )
			{
				// Empty
				continue;
			}

			CoordElem const x = drawRootX + xStep * math::integer_cast<CoordElem>( i_col );
			CoordElem const y = drawRootY + yStep * math::integer_cast<CoordElem>( i_row );
			math::Vector2f const topLeft = CoordToDevice( x, y );
			math::Vector2f const bottomRight = CoordToDevice(
				math::integer_cast<CoordElem>( x + xStep ),
				math::integer_cast<CoordElem>( y + yStep ) );
			math::AABB4f const pos = math::AABB4f{ topLeft, bottomRight };

			renderTile( mDeviceInterface, tile, deviceTextureID, texTilesPerRow, texXStep, texYStep, pos, z );
		}
	}
}



void PPUController::RenderString( PPUState::String const& string, rftl::string_view const& text ) const
{
	// !!!WARNING!!! This must be kept logically equivalent to CalculateStringLength(...)

	Font const* font = mFontManager->GetResourceFromManagedResourceID( string.mFontReference );
	RF_ASSERT_MSG( font != nullptr, "Failed to fetch font" );
	DeviceFontID const deviceFontID = font->GetDeviceRepresentation();

	uint8_t tileWidth = font->mTileWidth;
	uint8_t tileHeight = font->mTileHeight;
	uint8_t zoomDesired = 0;
	uint8_t shrinkDesired = 0;
	CalculateDesiredFontZoomShrink( *font, string.mDesiredHeight, zoomDesired, shrinkDesired );
	RF_ASSERT( zoomDesired >= 1 );
	RF_ASSERT( shrinkDesired >= 1 );
	tileWidth *= zoomDesired;
	tileHeight *= zoomDesired;
	tileWidth /= shrinkDesired;
	tileHeight /= shrinkDesired;
	RF_ASSERT( tileWidth > 0 );
	RF_ASSERT( tileHeight > 0 );

	CoordElem lastCharX = string.mXCoord;
	for( char const& character : text )
	{
		if( character == '\0' )
		{
			// End of string
			break;
		}

		uint8_t charWidth = tileWidth;
		uint8_t charHeight = tileHeight;
		uint8_t charMargin = 0;

		// Variable fonts use variable width
		if( font->mSpacingMode == Font::SpacingMode::Variable )
		{
			uint8_t whitespaceWidth = 0;
			CalculateFontVariableWhitespaceWidth( *font, whitespaceWidth );
			CalculateFontVariableCharWidth( *font, character, whitespaceWidth, zoomDesired, shrinkDesired, charWidth );
			charMargin = 1;
		}

		// Most fonts (even monospace) have varying heights, which we can use to
		//  reduce some overlap issues
		static constexpr bool kTrimHeight = true;
		if constexpr( kTrimHeight )
		{
			CalculateFontVariableCharHeight( *font, character, zoomDesired, shrinkDesired, charHeight );
		}

		RF_ASSERT( charWidth > 0 );
		RF_ASSERT( charHeight >= 0 );

		// Calculate the PPU position
		CoordElem const x1 = lastCharX;
		CoordElem const y1 = string.mYCoord;
		CoordElem const x2 = x1 + charWidth;
		CoordElem const y2 = y1 + charHeight;
		lastCharX = x2 + charMargin;

		if( charHeight <= 0 )
		{
			// Whitespace, doesn't need to be rendered
			continue;
		}

		RF_ASSERT( charWidth > 0 );
		RF_ASSERT( charHeight > 0 );

		// Calculate render coordinates
		math::Vector2f const topLeft = CoordToDevice( x1, y1 );
		math::Vector2f const bottomRight = CoordToDevice( x2, y2 );
		float const deviceWidth = bottomRight.x - topLeft.x;
		float const deviceHeight = bottomRight.y - topLeft.y;
		math::Color3f const color = ConvertColor( string.mColor );
		float const uvWidth = deviceWidth / ( ( deviceWidth * math::float_cast<float>( tileWidth ) ) / math::float_cast<float>( charWidth ) );
		float const uvHeight = deviceHeight / ( ( deviceHeight * math::float_cast<float>( tileHeight ) ) / math::float_cast<float>( charHeight ) );
		math::AABB4f const pos = math::AABB4f{ topLeft, bottomRight };
		float const z = LayerToDevice( string.mZLayer );
		math::AABB4f const uv = math::AABB4f{ 0.f, 0.f, uvWidth, uvHeight };

		// Render
		if( string.mBorder )
		{
			float const subZStep = math::Lerp( z, LayerToDevice( string.mZLayer + 1 ), 0.1f ) - z;
			math::Color3f const borderColor = CalculateBorderColor( color );
			math::Vector2f const pixelStep = GetDevicePixelStep( true );
			// HACK: Lots of draw calls here, lazy Scaleform-style
			mDeviceInterface->DrawBitmapFont(
				deviceFontID, character, pos + math::Vector2f{ pixelStep.x, 0 }, z + ( subZStep * 4 ), borderColor, uv );
			mDeviceInterface->DrawBitmapFont(
				deviceFontID, character, pos + math::Vector2f{ -pixelStep.x, 0 }, z + ( subZStep * 3 ), borderColor, uv );
			mDeviceInterface->DrawBitmapFont(
				deviceFontID, character, pos + math::Vector2f{ 0, pixelStep.y }, z + ( subZStep * 2 ), borderColor, uv );
			mDeviceInterface->DrawBitmapFont(
				deviceFontID, character, pos + math::Vector2f{ 0, -pixelStep.y }, z + ( subZStep * 1 ), borderColor, uv );
		}
		mDeviceInterface->DrawBitmapFont(
			deviceFontID,
			character,
			pos,
			z,
			color,
			uv );
	}
}



void PPUController::RenderDebugLine( PPUDebugState::DebugLine const& line ) const
{
	math::Vector2f const p0 = CoordToDevice( line.mXCoord0, line.mYCoord0 );
	math::Vector2f const p1 = CoordToDevice( line.mXCoord1, line.mYCoord1 );
	float const lineWidth = math::float_cast<float>( math::Max( 1, line.mWidth * GetZoomFactor() ) );
	mDeviceInterface->DebugDrawLine( p0, p1, LayerToDevice( line.mZLayer ), lineWidth, line.mColor );
}



void PPUController::RenderDebugString( PPUDebugState::DebugString const& string ) const
{
	math::Vector2f const pos = CoordToDevice( string.mXCoord, string.mYCoord );
	mDeviceInterface->DebugRenderText( pos, LayerToDevice( details::kDebugStringLayer ), "%s", string.mText );
}



void PPUController::RenderDebugGrid() const
{
	math::Color3f const& color = math::Color3f::kBlack;
	for( CoordElem horizontal = 0; horizontal <= mWidth; horizontal += kTileSize )
	{
		math::Vector2f const posA = CoordToDevice( horizontal, 0 );
		math::Vector2f const posB = CoordToDevice( horizontal - 1, 0 );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( posA.x, 0 ),
			math::Vector2f( posA.x, 1 ),
			LayerToDevice( details::kDebugGridLayer ),
			1.f,
			color );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( posB.x, 0 ),
			math::Vector2f( posB.x, 1 ),
			LayerToDevice( details::kDebugGridLayer ),
			1.f,
			color );
	}
	for( CoordElem vertical = 0; vertical <= mHeight; vertical += kTileSize )
	{
		math::Vector2f const posA = CoordToDevice( 0, vertical );
		math::Vector2f const posB = CoordToDevice( 0, vertical - 1 );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( 0, posA.y ),
			math::Vector2f( 1, posA.y ),
			LayerToDevice( details::kDebugGridLayer ),
			1.f,
			color );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( 0, posB.y ),
			math::Vector2f( 1, posB.y ),
			LayerToDevice( details::kDebugGridLayer ),
			1.f,
			color );
	}
}



ZoomFactor PPUController::GetZoomFactor() const
{
	uint16_t const smallestDimension = math::Min( mWidth, mHeight );
	uint16_t const approximateDiagonalTiles = math::integer_cast<uint16_t>( smallestDimension / kTileSize );
	ZoomFactor const zoomFactor = math::integer_cast<ZoomFactor>( math::Max( 1, approximateDiagonalTiles / kDesiredDiagonalTiles ) );
	return zoomFactor;
}



math::Vector2f PPUController::GetDevicePixelStep( bool allowZoomHiding ) const
{
	math::Vector2f const zero = CoordToDevice( 0, 0 );
	math::Vector2f const one = CoordToDevice( 1, 1 );
	math::Vector2f const delta = one - zero;
	math::Vector2f const atScale = delta * ( 1.f / ( allowZoomHiding && mHideZoomFactor ? 1.f : GetZoomFactor() ) );
	return atScale;
}



math::Vector2f PPUController::CoordToDevice( CoordElem xCoord, CoordElem yCoord ) const
{
	return CoordToDevice( Coord( xCoord, yCoord ) );
}



math::Vector2f PPUController::CoordToDevice( Coord const& coord ) const
{
	// TODO: Windowing
	uint16_t const smallestDimenssion = math::Min( mWidth, mHeight );
	ZoomFactor const zoomFactor = GetZoomFactor();
	float const diagonalTiles = ( math::float_cast<float>( smallestDimenssion ) ) / math::float_cast<float>( kTileSize * zoomFactor );

	// Baseline
	// [0-64]
	float x = coord.x;
	float y = coord.y;

	// Tiles
	// [0-2]
	float const coordToTiles = 1.f / kTileSize;
	x *= coordToTiles;
	y *= coordToTiles;

	// NDC, mHeight only
	// [0-2/15.3f]
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting mWidth
	float const heightToWidthNDC = math::float_cast<float>( mHeight ) / math::float_cast<float>( mWidth );
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
}



float PPUController::LayerToDevice( DepthLayer zDepth ) const
{
	static_assert( rftl::is_integral<DepthLayer>::value, "Unexpected depth type" );
	static_assert( rftl::is_signed<DepthLayer>::value, "Unexpected depth type" );
	float const scaledDepth = math::float_cast<float>( zDepth ) / math::float_cast<float>( rftl::numeric_limits<DepthLayer>::max() );
	return scaledDepth;
}



math::Vector2f PPUController::TileToDevice( TileElem xTile, TileElem yTile ) const
{
	// TODO: Windowing, scaling, etc
	//constexpr uint8_t zoomFactor = 1;
	constexpr uint8_t diagonalTiles = 15;

	// Baseline
	float x = xTile;
	float y = yTile;

	// NDC, mHeight only
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting mWidth
	float const heightToWidthNDC = math::float_cast<float>( mHeight ) / math::float_cast<float>( mWidth );
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
}



template<PPUController::DrawOverflowBehavior BehaviorT, typename TypeT, size_t MaxCountT, typename SizeT>
auto PPUController::RenderStateListItemSelect( TypeT ( &list )[MaxCountT], SizeT& count, char const* name ) -> TypeT&
{
	static_assert(
		0 < MaxCountT,
		"List is somehow of max size zero" );
	static_assert(
		MaxCountT <= rftl::numeric_limits<SizeT>::max(),
		"Count variable type is not large enough to reference all items in list" );

	// TODO: Thread-safe

	if( count >= MaxCountT )
	{
		// Draw command would overflow

		static constexpr bool kAssert =
			BehaviorT == DrawOverflowBehavior::OverwriteAssert ||
			BehaviorT == DrawOverflowBehavior::FlickerAssert;
		static constexpr bool kFlicker =
			BehaviorT == DrawOverflowBehavior::FlickerSilent ||
			BehaviorT == DrawOverflowBehavior::FlickerAssert;

		if constexpr( kAssert )
		{
			// Some types are important enough we want to complain louder
			RF_DBGFAIL_MSG( "Draw command overflow" );
		}
		RF_ONCEPER_MINUTE( RFLOG_WARNING( nullptr, RFCAT_PPU, "Draw command overflow of type '%s', some will be ignored", name ) );

		if constexpr( kFlicker )
		{
			// Make the screen flicker so the commands are atleast somewhat
			//  visible, even if they look terrible

			static uint32_t randVal = 0;
			randVal = math::StableRandLCG( randVal );

			size_t const randomIndex = randVal % MaxCountT;
			TypeT& target = list[randomIndex];
			return target;
		}
		else
		{
			// Just overwrite the last entry, which prevents flickering but
			//  essentially truncates the list, and may cause something to never
			//  be seen if conditions are unlucky enough

			static constexpr size_t kLastIndex = MaxCountT - 1;
			TypeT& target = list[kLastIndex];
			return target;
		}
	}
	else
	{
		RF_ASSERT( count < MaxCountT );
		TypeT& target = list[count];
		count++;
		return target;
	}
}



void PPUController::FullfillAllDeferredLoadRequests()
{
	LoadRequests const inFlight = rftl::move( mDeferredLoadRequests );
	for( LoadRequest const& loadRequest : inFlight )
	{
		FullfillLoadRequest( loadRequest );
	}
}



bool PPUController::FullfillLoadRequest( LoadRequest const& request )
{
	ResourceName const& resourceName = request.mResourceName;
	Filename const& filename = request.mFilename;
	switch( request.mLoadType )
	{
		case LoadType::Reserve:
		{
			auto const load = [&resourceName, &filename]( auto& manager ) -> bool //
			{
				if( resourceName.empty() )
				{
					return manager->ReserveNullResource( filename );
				}
				else
				{
					return manager->ReserveNullResource( resourceName );
				}
			};
			switch( request.mAssetType )
			{
				case AssetType::Texture:
					return load( mTextureManager );
				case AssetType::FramePack:
					return load( mFramePackManager );
				case AssetType::Tileset:
					return load( mTilesetManager );
				case AssetType::Font:
					return load( mFontManager );
				case AssetType::Invalid:
				default:
					RF_DBGFAIL();
					return false;
			}
		}
		case LoadType::New:
		{
			auto const load = [&resourceName, &filename]( auto& manager ) -> bool //
			{
				if( resourceName.empty() )
				{
					return manager->LoadNewResource( filename );
				}
				else
				{
					return manager->LoadNewResource( resourceName, filename );
				}
			};
			switch( request.mAssetType )
			{
				case AssetType::Texture:
					return load( mTextureManager );
				case AssetType::FramePack:
					return load( mFramePackManager );
				case AssetType::Tileset:
					return load( mTilesetManager );
				case AssetType::Font:
					return load( mFontManager );
				case AssetType::Invalid:
				default:
					RF_DBGFAIL();
					return false;
			}
		}
		case LoadType::Modify:
		{
			auto const load = [&resourceName, &filename]( auto& manager ) -> bool //
			{
				if( resourceName.empty() )
				{
					return manager->UpdateExistingResource( filename );
				}
				else
				{
					return manager->UpdateExistingResource( resourceName, filename );
				}
			};
			switch( request.mAssetType )
			{
				case AssetType::Texture:
					return load( mTextureManager );
				case AssetType::FramePack:
					return load( mFramePackManager );
				case AssetType::Tileset:
					return load( mTilesetManager );
				case AssetType::Font:
					return load( mFontManager );
				case AssetType::Invalid:
				default:
					RF_DBGFAIL();
					return false;
			}
		}
		case LoadType::Reload:
		{
			auto const load = [&resourceName, &filename]( auto& manager ) -> bool //
			{
				if( resourceName.empty() )
				{
					return manager->ReloadExistingResource( filename );
				}
				else
				{
					return manager->ReloadExistingResource( resourceName );
				}
			};
			switch( request.mAssetType )
			{
				case AssetType::Texture:
					return load( mTextureManager );
				case AssetType::FramePack:
					return load( mFramePackManager );
				case AssetType::Tileset:
					return load( mTilesetManager );
				case AssetType::Font:
					return load( mFontManager );
				case AssetType::Invalid:
				default:
					RF_DBGFAIL();
					return false;
			}
		}
		case LoadType::Invalid:
		default:
			RF_DBGFAIL();
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

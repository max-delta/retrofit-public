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

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"
#include "core_math/math_bits.h"
#include "core_math/Lerp.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstdarg"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr PPUDepthLayer kDebugLineLayer = 0;
static constexpr PPUDepthLayer kDebugStringLayer = kNearestLayer;
static constexpr PPUDepthLayer kDebugGridLayer = 0;

static constexpr PPUDepthLayer kDefaultTextLayer = 0;

}
///////////////////////////////////////////////////////////////////////////////

PPUController::PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs )
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
	mTextureManager = DefaultCreator<gfx::TextureManager>::Create( mVfs );
	success = mTextureManager->AttachToDevice( mDeviceInterface );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Create frame pack manager
	RF_ASSERT( mFramePackManager == nullptr );
	mFramePackManager = DefaultCreator<gfx::FramePackManager>::Create( mTextureManager, mVfs );

	// Create tileset manager
	RF_ASSERT( mTilesetManager == nullptr );
	mTilesetManager = DefaultCreator<gfx::TilesetManager>::Create( mTextureManager, mVfs );

	// Create font manager
	RF_ASSERT( mFontManager == nullptr );
	mFontManager = DefaultCreator<gfx::FontManager>::Create( mVfs );
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



PPUCoordElem PPUController::GetWidth() const
{
	return mWidth / GetZoomFactor();
}



PPUCoordElem PPUController::GetHeight() const
{
	return mHeight / GetZoomFactor();
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



bool PPUController::DrawObject( Object const& object )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	// TODO: Thread-safe

	if( targetState.mNumObjects >= PPUState::kMaxObjects )
	{
		RF_DBGFAIL_MSG( "Too many objects drawn this frame" );
		return false;
	}

	Object& targetObject = targetState.mObjects[targetState.mNumObjects];
	targetState.mNumObjects++;

	targetObject = object;

	return true;
}



bool PPUController::DrawTileLayer( TileLayer const& tileLayer )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	// TODO: Thread-safe

	if( targetState.mNumTileLayers >= PPUState::kMaxTileLayers )
	{
		RF_DBGFAIL_MSG( "Too many tile layers drawn this frame" );
		return false;
	}

	TileLayer& targetTileLayer = targetState.mTileLayers[targetState.mNumTileLayers];
	targetState.mNumTileLayers++;

	targetTileLayer = tileLayer;

	return true;
}



bool PPUController::DrawText( PPUCoord pos, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, details::kDefaultTextLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, math::Color3f color, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, false, color, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, desiredHeight, font, border, color, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.mNumStrings < PPUState::kMaxStrings );
	PPUState::String& targetString = targetState.mStrings[targetState.mNumStrings];
	targetState.mNumStrings++;

	targetString.mXCoord = math::integer_cast<PPUCoordElem>( pos.x );
	targetString.mYCoord = math::integer_cast<PPUCoordElem>( pos.y );
	targetString.mZLayer = zLayer;
	targetString.mColor[0] = static_cast<uint8_t>( color.r * rftl::numeric_limits<uint8_t>::max() );
	targetString.mColor[1] = static_cast<uint8_t>( color.g * rftl::numeric_limits<uint8_t>::max() );
	targetString.mColor[2] = static_cast<uint8_t>( color.b * rftl::numeric_limits<uint8_t>::max() );
	targetString.mDesiredHeight = desiredHeight;
	targetString.mBorder = border;
	targetString.mFontReference = font;
	targetString.mText[0] = '\0';
	vsnprintf( &targetString.mText[0], PPUState::String::k_MaxLen, fmt, args );
	targetString.mText[PPUState::String::k_MaxLen] = '\0';

	return true;
}



PPUZoomFactor PPUController::GetCurrentZoomFactor() const
{
	// TODO: Thread-safe?
	return GetZoomFactor();
}



PPUCoordElem PPUController::CalculateStringLengthFormatted( uint8_t desiredHeight, ManagedFontID fontID, char const* fmt, ... )
{
	decltype( PPUState::String::mText ) string = {};
	static_assert( rftl::is_same<rftl::remove_all_extents<decltype( string )>::type, char>::value, "Unexpected storage" );
	{
		string[0] = '\0';
		va_list args;
		va_start( args, fmt );
		vsnprintf( &string[0], PPUState::String::k_MaxLen, fmt, args );
		va_end( args );
		string[PPUState::String::k_MaxLen] = '\0';
	}

	char const* text = string;

	return CalculateStringLength( desiredHeight, fontID, text );
}



PPUCoordElem PPUController::CalculateStringLength( uint8_t desiredHeight, ManagedFontID fontID, char const* text )
{
	// !!!WARNING!!! This must be kept logically equivalent to RenderString(...)

	PPUCoordElem retVal = 0;

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

	for( size_t i_char = 0; i_char < PPUState::String::k_MaxLen; i_char++ )
	{
		char const character = text[i_char];
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



PPUCoord PPUController::CalculateTileLayerSize( TileLayer const& tileLayer ) const
{
	Tileset const* tileset = mTilesetManager->GetResourceFromManagedResourceID( tileLayer.mTilesetReference );
	RF_ASSERT_MSG( tileset != nullptr, "Failed to fetch tileset" );

	PPUCoordElem xStep;
	PPUCoordElem yStep;
	CalculateTileSize( tileLayer, *tileset, xStep, yStep );
	PPUCoordElem const xLayerStep = math::integer_cast<PPUCoordElem>( xStep * tileLayer.NumColumns() );
	PPUCoordElem const yLayerStep = math::integer_cast<PPUCoordElem>( yStep * tileLayer.NumRows() );
	return PPUCoord{ xLayerStep, yLayerStep };
}



bool PPUController::QueueDeferredLoadRequest( AssetType type, Filename const& filename )
{
	return QueueDeferredLoadRequest( type, ResourceName{}, filename );
}



bool PPUController::QueueDeferredLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename )
{
	mDeferredLoadRequests.emplace_back( LoadRequest{ type, resourceName, filename } );
	return true;
}



bool PPUController::ForceImmediateLoadRequest( AssetType type, Filename const& filename )
{
	return ForceImmediateLoadRequest( type, ResourceName{}, filename );
}



bool PPUController::ForceImmediateLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename )
{
	return FullfillLoadRequest( LoadRequest{ type, resourceName, filename } );
}




WeakPtr<gfx::TextureManager const> PPUController::GetTextureManager() const
{
	return mTextureManager;
}



WeakPtr<gfx::FramePackManager const> PPUController::GetFramePackManager() const
{
	return mFramePackManager;
}



WeakPtr<gfx::TilesetManager const> PPUController::GetTilesetManager() const
{
	return mTilesetManager;
}



WeakPtr<gfx::FontManager const> PPUController::GetFontManager() const
{
	return mFontManager;
}



void PPUController::DebugSetGridEnabled( bool enabled )
{
	mDebugDrawGrid = enabled;
}



bool PPUController::DebugDrawText( PPUCoord pos, const char* fmt, ... )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.mNumStrings < PPUDebugState::kMaxStrings );
	PPUDebugState::DebugString& targetString = targetState.mStrings[targetState.mNumStrings];
	targetState.mNumStrings++;

	targetString.mXCoord = math::integer_cast<PPUCoordElem>( pos.x );
	targetString.mYCoord = math::integer_cast<PPUCoordElem>( pos.y );
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



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1 )
{
	return DebugDrawLine( p0, p1, math::Color3f::kBlack );
}



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width )
{
	return DebugDrawLine( p0, p1, width, math::Color3f::kBlack );
}



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1, math::Color3f color )
{
	return DebugDrawLine( p0, p1, 0, color );
}



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width, math::Color3f color )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.mNumLines < PPUDebugState::kMaxLines );
	PPUDebugState::DebugLine& targetLine = targetState.mLines[targetState.mNumLines];
	targetState.mNumLines++;

	targetLine.mXCoord0 = math::integer_cast<PPUCoordElem>( p0.x );
	targetLine.mYCoord0 = math::integer_cast<PPUCoordElem>( p0.y );
	targetLine.mXCoord1 = math::integer_cast<PPUCoordElem>( p1.x );
	targetLine.mYCoord1 = math::integer_cast<PPUCoordElem>( p1.y );
	targetLine.mWidth = width;
	targetLine.mColor = color;

	return true;
}



bool PPUController::DebugDrawAABB( math::AABB4<PPUCoordElem> aabb )
{
	return DebugDrawAABB( aabb, math::Color3f::kBlack );
}



bool PPUController::DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, PPUCoordElem width )
{
	return DebugDrawAABB( aabb, width, math::Color3f::kBlack );
}



bool PPUController::DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, math::Color3f color )
{
	return DebugDrawAABB( aabb, 0, color );
}



bool PPUController::DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, PPUCoordElem width, math::Color3f color )
{
	DebugDrawLine( aabb.mTopLeft, PPUCoord( aabb.mTopLeft.x, aabb.mBottomRight.y ), width, color );
	DebugDrawLine( aabb.mTopLeft, PPUCoord( aabb.mBottomRight.x, aabb.mTopLeft.y ), width, color );
	DebugDrawLine( aabb.mBottomRight, PPUCoord( aabb.mTopLeft.x, aabb.mBottomRight.y ), width, color );
	DebugDrawLine( aabb.mBottomRight, PPUCoord( aabb.mBottomRight.x, aabb.mTopLeft.y ), width, color );

	return true;
}



WeakPtr<gfx::DeviceInterface> PPUController::DebugGetDeviceInterface() const
{
	return mDeviceInterface;
}



WeakPtr<gfx::TextureManager> PPUController::DebugGetTextureManager() const
{
	return mTextureManager;
}



WeakPtr<gfx::FramePackManager> PPUController::DebugGetFramePackManager() const
{
	return mFramePackManager;
}



WeakPtr<gfx::TilesetManager> PPUController::DebugGetTilesetManager() const
{
	return mTilesetManager;
}



WeakPtr<gfx::FontManager> PPUController::DebugGetFontManager() const
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
			case RF::gfx::PPUController::ElementType::Invalid:
				// End of list
				terminate = true;
				break;

			case RF::gfx::PPUController::ElementType::Object:
				RenderObject( targetState.mObjects[i] );
				break;
			case RF::gfx::PPUController::ElementType::TileLayer:
				RenderTileLayer( targetState.mTileLayers[i] );
				break;
			case RF::gfx::PPUController::ElementType::String:
				RenderString( targetState.mStrings[i] );
				break;
			case RF::gfx::PPUController::ElementType::DebugLine:
				RenderDebugLine( targetDebugState.mLines[i] );
				break;
			case RF::gfx::PPUController::ElementType::DebugString:
				RenderDebugString( targetDebugState.mStrings[i] );
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
		//PPUDebugState::DebugLine const& line = targetDebugState.mLines[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::DebugLine;
		element.mDepth = details::kDebugLineLayer;
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
			if( static_cast<uint8_t>( lhse.mType ) < static_cast<uint8_t>( rhse.mDepth ) )
			{
				return -1;
			}
			else if( static_cast<uint8_t>( lhse.mType ) > static_cast<uint8_t>( rhse.mDepth ) )
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
		PPUZoomFactor const zoomFactor = GetZoomFactor();
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
	whitespaceWidth = font.mTileWidth;
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

void PPUController::CalculateTileSize( TileLayer const& tileLayer, Tileset const& tileset, PPUCoordElem& tileWidth, PPUCoordElem& tileHeight ) const
{
	uint8_t scaleUp;
	uint8_t scaleDown;
	tileLayer.GetTileZoomFactor( scaleUp, scaleDown );
	tileWidth = ( tileset.mTileWidth * scaleUp ) / scaleDown;
	tileHeight = ( tileset.mTileHeight * scaleUp ) / scaleDown;
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
	PPUCoordElem const x = object.mXCoord - timeSlot.mTextureOriginX;
	PPUCoordElem const y = object.mYCoord - timeSlot.mTextureOriginY;
	math::Vector2f topLeft = CoordToDevice( x, y );
	math::Vector2f bottomRight = CoordToDevice(
		math::integer_cast<PPUCoordElem>( x + texture->mWidthPostLoad ),
		math::integer_cast<PPUCoordElem>( y + texture->mHeightPostLoad ) );

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
	float const texXStep = static_cast<float>( tileset->mTileWidth ) / texture->mWidthPostLoad;
	float const texYStep = static_cast<float>( tileset->mTileHeight ) / texture->mHeightPostLoad;

	PPUCoordElem xStep;
	PPUCoordElem yStep;
	CalculateTileSize( tileLayer, *tileset, xStep, yStep );
	PPUCoordElem const xLayerStep = math::integer_cast<PPUCoordElem>( xStep * tileLayer.NumColumns() );
	PPUCoordElem const yLayerStep = math::integer_cast<PPUCoordElem>( yStep * tileLayer.NumRows() );

	float const z = LayerToDevice( tileLayer.mZLayer );

	PPUCoordElem const rootX = tileLayer.mXCoord;
	PPUCoordElem const rootY = tileLayer.mYCoord;

	// TODO: Animations? Probably done via shaders and passing in the timer

	// TODO: Transforms

	// TODO: Flips

	int16_t posColStart = 0;
	int16_t posColEnd = math::integer_cast<int16_t>( tileLayer.NumColumns() );
	int16_t posRowStart = 0;
	int16_t posRowEnd = math::integer_cast<int16_t>( tileLayer.NumRows() );
	PPUCoordElem drawRootX = rootX;
	PPUCoordElem drawRootY = rootY;
	int16_t tileColOffset = 0;
	int16_t tileRowOffset = 0;

	// Wrapping
	if( tileLayer.mWrapping )
	{
		// How far off screen do we need to start rendering?
		PPUCoordElem wrapRootX;
		PPUCoordElem wrapRootY;
		{
			RF_ASSERT_MSG( rootX >= 0, "Untested math!" );
			RF_ASSERT_MSG( rootY >= 0, "Untested math!" );
			PPUCoordElem const xStepDifferenceFromZero = rootX % xLayerStep;
			PPUCoordElem const yStepDifferenceFromZero = rootY % yLayerStep;
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

		// How many cycles of wrapping before we reach the root of the layer?
		int16_t xCyclesBeforeRoot;
		int16_t yCyclesBeforeRoot;
		{
			PPUCoordElem const xWrapDifferenceFromRoot = rootX - wrapRootX;
			PPUCoordElem const yWrapDifferenceFromRoot = rootY - wrapRootY;
			RF_ASSERT( xWrapDifferenceFromRoot % xStep == 0 );
			RF_ASSERT( yWrapDifferenceFromRoot % yStep == 0 );
			xCyclesBeforeRoot = xWrapDifferenceFromRoot / xStep;
			yCyclesBeforeRoot = yWrapDifferenceFromRoot / yStep;
		}
		RF_ASSERT( xCyclesBeforeRoot >= 0 );
		RF_ASSERT( yCyclesBeforeRoot >= 0 );

		// How many cycles of wrapping before we reach the end of the screen?
		int16_t xCyclesBeforeScreenEscape;
		int16_t yCyclesBeforeScreenEscape;
		{
			PPUCoordElem const xWrapDifferenceFromEscape = GetWidth() - wrapRootX;
			PPUCoordElem const yWrapDifferenceFromEscape = GetHeight() - wrapRootY;
			xCyclesBeforeScreenEscape = xWrapDifferenceFromEscape / xStep;
			yCyclesBeforeScreenEscape = yWrapDifferenceFromEscape / yStep;
		}
		RF_ASSERT( xCyclesBeforeScreenEscape >= 0 );
		RF_ASSERT( yCyclesBeforeScreenEscape >= 0 );

		drawRootX = wrapRootX;
		drawRootY = wrapRootY;
		tileColOffset = xCyclesBeforeRoot;
		tileRowOffset = yCyclesBeforeRoot;
		posColEnd = xCyclesBeforeScreenEscape + 1;
		posRowEnd = yCyclesBeforeScreenEscape + 1;
		if( drawRootX < 0 )
		{
			posColStart = -1;
		}
		if( drawRootY < 0 )
		{
			posColStart = -1;
		}
	}

	constexpr auto renderTile = [](
		DeviceInterface* deviceInterface,
		TileLayer::Tile const& tile,
		DeviceTextureID deviceTextureID,
		uint16_t texTilesPerRow,
		float texXStep,
		float texYStep,
		math::AABB4f pos,
		float z) -> void
	{
		RF_ASSERT( tile.mIndex != TileLayer::kEmptyTileIndex );

		math::Vector2i16 const texTile = math::Vector2i16(
			math::integer_cast<int16_t>( tile.mIndex % texTilesPerRow ),
			math::integer_cast<int16_t>( tile.mIndex / texTilesPerRow ) );
		math::Vector2f const texTopLeft = math::Vector2f(
			texTile.x * texXStep,
			texTile.y * texYStep );
		math::Vector2f const texBottomRight = texTopLeft + math::Vector2f( texXStep, texYStep );

		deviceInterface->DrawBillboard( deviceTextureID, pos, z, math::AABB4f{ texTopLeft, texBottomRight } );
	};

	for( int16_t i_col = posColStart; i_col < posColEnd; i_col++ )
	{
		for( int16_t i_row = posRowStart; i_row < posRowEnd; i_row++ )
		{
			size_t const tileCol = ( i_col + tileColOffset ) % tileLayer.NumColumns();
			size_t const tileRow = ( i_row + tileRowOffset ) % tileLayer.NumRows();
			TileLayer::Tile const& tile = tileLayer.GetTile( tileCol, tileRow );
			if( tile.mIndex == TileLayer::kEmptyTileIndex )
			{
				// Empty
				continue;
			}

			PPUCoordElem const x = drawRootX + xStep * math::integer_cast<PPUCoordElem>( i_col );
			PPUCoordElem const y = drawRootY + yStep * math::integer_cast<PPUCoordElem>( i_row );
			math::Vector2f const topLeft = CoordToDevice( x, y );
			math::Vector2f const bottomRight = CoordToDevice(
				math::integer_cast<PPUCoordElem>( x + xStep ),
				math::integer_cast<PPUCoordElem>( y + yStep ) );
			math::AABB4f const pos = math::AABB4f{ topLeft, bottomRight };

			renderTile( mDeviceInterface, tile, deviceTextureID, texTilesPerRow, texXStep, texYStep, pos, z );
		}
	}
}



void PPUController::RenderString( PPUState::String const& string ) const
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

	char const* text = string.mText;
	PPUCoordElem lastCharX = string.mXCoord;
	for( size_t i_char = 0; i_char < PPUState::String::k_MaxLen; i_char++ )
	{
		char const character = text[i_char];
		if( character == '\0' )
		{
			break;
		}

		uint8_t charWidth = tileWidth;
		uint8_t const& charHeight = tileHeight;
		uint8_t charMargin = 0;

		if( font->mSpacingMode == Font::SpacingMode::Variable )
		{
			uint8_t whitespaceWidth = 0;
			CalculateFontVariableWhitespaceWidth( *font, whitespaceWidth );
			CalculateFontVariableCharWidth( *font, character, whitespaceWidth, zoomDesired, shrinkDesired, charWidth );
			charMargin = 1;
		}

		RF_ASSERT( charWidth > 0 );
		RF_ASSERT( charHeight > 0 );

		PPUCoordElem const x1 = lastCharX;
		PPUCoordElem const y1 = string.mYCoord;
		PPUCoordElem const x2 = x1 + charWidth;
		PPUCoordElem const y2 = y1 + charHeight;
		lastCharX = x2 + charMargin;

		math::Vector2f const topLeft = CoordToDevice( x1, y1 );
		math::Vector2f const bottomRight = CoordToDevice( x2, y2 );
		float const deviceWidth = bottomRight.x - topLeft.x;
		math::Color3f const color = math::Color3f{
			static_cast<math::Color3f::ElementType>( string.mColor[0] ),
			static_cast<math::Color3f::ElementType>( string.mColor[1] ),
			static_cast<math::Color3f::ElementType>( string.mColor[2] )
		} * ( 1.f / rftl::numeric_limits<uint8_t>::max() );
		float const uvWidth = deviceWidth / ( ( deviceWidth * tileWidth ) / charWidth );
		math::AABB4f const pos = math::AABB4f{ topLeft, bottomRight };
		float const z = LayerToDevice( string.mZLayer );
		math::AABB4f const uv = math::AABB4f{ 0.f, 0.f, uvWidth, 1.f };
		if( string.mBorder )
		{
			float const subZStep = math::Lerp( z, LayerToDevice( string.mZLayer + 1 ), 0.1f ) - z;
			math::Color3f const borderColor = CalculateBorderColor( color );
			math::Vector2f const pixelStep = GetDevicePixelStep();
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
	mDeviceInterface->DebugDrawLine( p0, p1, LayerToDevice( details::kDebugLineLayer ), static_cast<float>( line.mWidth * GetZoomFactor() ), line.mColor );
}



void PPUController::RenderDebugString( PPUDebugState::DebugString const& string ) const
{
	math::Vector2f const pos = CoordToDevice( string.mXCoord, string.mYCoord );
	mDeviceInterface->DebugRenderText( pos, LayerToDevice( details::kDebugStringLayer ), "%s", string.mText );
}



void PPUController::RenderDebugGrid() const
{
	math::Color3f const& color = math::Color3f::kBlack;
	for( PPUCoordElem horizontal = 0; horizontal <= mWidth; horizontal += kTileSize )
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
	for( PPUCoordElem vertical = 0; vertical <= mHeight; vertical += kTileSize )
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



PPUZoomFactor PPUController::GetZoomFactor() const
{
	uint16_t const smallestDimension = math::Min( mWidth, mHeight );
	uint16_t const approximateDiagonalTiles = math::integer_cast<uint16_t>( smallestDimension / kTileSize );
	PPUZoomFactor const zoomFactor = math::integer_cast<PPUZoomFactor>( math::Max( 1, approximateDiagonalTiles / kDesiredDiagonalTiles ) );
	return zoomFactor;
}



math::Vector2f PPUController::GetDevicePixelStep() const
{
	math::Vector2f const zero = CoordToDevice( 0, 0 );
	math::Vector2f const one = CoordToDevice( 1, 1 );
	math::Vector2f const delta = one - zero;
	math::Vector2f const atScale = delta * ( 1.f / GetCurrentZoomFactor() );
	return atScale;
}



math::Vector2f PPUController::CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const
{
	return CoordToDevice( PPUCoord( xCoord, yCoord ) );
}



math::Vector2f PPUController::CoordToDevice( PPUCoord const& coord ) const
{
	// TODO: Windowing
	uint16_t const smallestDimenssion = math::Min( mWidth, mHeight );
	PPUZoomFactor const zoomFactor = GetZoomFactor();
	float const diagonalTiles = ( static_cast<float>( smallestDimenssion ) ) / ( kTileSize * zoomFactor );

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
	float const heightToWidthNDC = float( mHeight ) / mWidth;
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
}



float PPUController::LayerToDevice( PPUDepthLayer zDepth ) const
{
	static_assert( rftl::is_integral<PPUDepthLayer>::value, "Unexpected depth type" );
	static_assert( rftl::is_signed<PPUDepthLayer>::value, "Unexpected depth type" );
	float const scaledDepth = math::real_cast<float>( zDepth ) / rftl::numeric_limits<PPUDepthLayer>::max();
	return scaledDepth;
}



math::Vector2f PPUController::TileToDevice( PPUTileElem xTile, PPUTileElem yTile ) const
{
	// TODO: Windowing, scaling, etc
	constexpr uint8_t zoomFactor = 1;
	constexpr uint8_t diagonalTiles = 15;

	// Baseline
	float x = xTile;
	float y = yTile;

	// NDC, mHeight only
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting mWidth
	float const heightToWidthNDC = float( mHeight ) / mWidth;
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
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
	if( resourceName.empty() )
	{
		switch( request.mType )
		{
			case AssetType::Texture:
				return mTextureManager->LoadNewResource( filename );
			case AssetType::FramePack:
				return mFramePackManager->LoadNewResource( filename );
			case AssetType::Tileset:
				return mTilesetManager->LoadNewResource( filename );
			case AssetType::Font:
				return mFontManager->LoadNewResource( filename );
			case AssetType::Invalid:
				RF_DBGFAIL();
				return false;
		}
	}
	else
	{
		switch( request.mType )
		{
			case AssetType::Texture:
				return mTextureManager->LoadNewResource( resourceName, filename );
			case AssetType::FramePack:
				return mFramePackManager->LoadNewResource( resourceName, filename );
			case AssetType::Tileset:
				return mTilesetManager->LoadNewResource( resourceName, filename );
			case AssetType::Font:
				return mFontManager->LoadNewResource( resourceName, filename );
			case AssetType::Invalid:
				RF_DBGFAIL();
				return false;
		}
	}

	RF_DBGFAIL();
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}

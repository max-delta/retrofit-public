#include "stdafx.h"
#include "PPUController.h"

#include "PPU/DeviceInterface.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"
#include "PPU/TilesetManager.h"
#include "PPU/Tileset.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstdarg"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

PPUController::PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs )
	: mDeviceInterface( rftl::move( deviceInterface ) )
	, mTextureManager( nullptr )
	, mFramePackManager( nullptr )
	, mTilesetManager( nullptr )
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

	// Prepare device
	success = mDeviceInterface->Initialize2DGraphics();
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}
	success = mDeviceInterface->SetBackgroundColor( 1, 0, 1, 1 );
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



bool PPUController::LoadFont( void const* buffer, size_t len )
{
	// TODO: Revise the font API
	uint32_t unused;
	return mDeviceInterface->CreateBitmapFont( buffer, len, 7, unused, unused );
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



bool PPUController::DrawText( PPUCoord pos, PPUCoord charSize, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, 0, charSize, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, PPUCoord charSize, const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	bool const retVal = DrawText( pos, zLayer, charSize, fmt, args );
	va_end( args );
	return retVal;
}



bool PPUController::DrawText( PPUCoord pos, PPUDepthLayer zLayer, PPUCoord charSize, const char* fmt, va_list args )
{
	RF_ASSERT_MSG( charSize.x == 4, "TODO: Support other sizes" );
	RF_ASSERT_MSG( charSize.y == 8, "TODO: Support other sizes" );

	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUState& targetState = mPPUState[mWriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.mNumStrings < PPUState::kMaxStrings );
	PPUState::String& targetString = targetState.mStrings[targetState.mNumStrings];
	targetState.mNumStrings++;

	targetString.mXCoord = math::integer_cast<PPUCoordElem>( pos.x );
	targetString.mYCoord = math::integer_cast<PPUCoordElem>( pos.y );
	targetString.mZLayer = zLayer;
	targetString.mWidth = charSize.x;
	targetString.mHeight = charSize.y;
	targetString.mText[0] = '\0';
	vsnprintf( &targetString.mText[0], PPUState::String::k_MaxLen, fmt, args );
	targetString.mText[PPUState::String::k_MaxLen] = '\0';

	return true;
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



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width )
{
	RF_ASSERT( mWriteState != kInvalidStateBufferID );
	PPUDebugState& targetState = mPPUDebugState[mWriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.mNumLines < PPUDebugState::kMaxLines );
	PPUDebugState::DebugLine& targetLine = targetState.mLines[targetState.mNumLines];
	targetState.mNumLines++;

	targetLine.m_XCoord0 = math::integer_cast<PPUCoordElem>( p0.x );
	targetLine.m_YCoord0 = math::integer_cast<PPUCoordElem>( p0.y );
	targetLine.m_XCoord1 = math::integer_cast<PPUCoordElem>( p1.x );
	targetLine.m_YCoord1 = math::integer_cast<PPUCoordElem>( p1.y );
	targetLine.mWidth = width;

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

///////////////////////////////////////////////////////////////////////////////

void PPUController::SignalRender( StateBufferID readyBuffer )
{
	// TODO: If renderer open - push work, if closed - queue work
	RF_ASSERT_MSG( mRenderState == kInvalidStateBufferID, "TODO: Asynchronous rendering" );
	RF_ASSERT_MSG( mQueueToRenderState == kInvalidStateBufferID, "TODO: Asynchronous rendering" );

	// Set buffer as rendering
	mRenderState = readyBuffer;

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

	// Render in depth order
	for( DepthElement const& element : depthOrder )
	{
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

	// HACK: Draw grid
	// TODO: Configurable
	constexpr bool drawGrid = true;
	if( drawGrid )
	{
		RenderDebugGrid();
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
		element.mDepth = kNearestLayer;
	}

	// Debug text
	for( size_t i = 0; i < targetDebugState.mNumStrings; i++ )
	{
		//PPUDebugState::DebugString const& string = targetDebugState.mStrings[i];
		DepthElement& element = depthOrder[i_depthOrder];
		i_depthOrder++;
		element.mId = math::integer_cast<uint8_t>( i );
		element.mType = ElementType::DebugString;
		element.mDepth = kNearestLayer;
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



void PPUController::RenderObject( Object const& object ) const
{
	FramePackBase const* const framePack = mFramePackManager->GetResourceFromManagedResourceID( object.mFramePackID );
	RF_ASSERT_MSG( framePack != nullptr, "Invalid frame pack ID" );
	uint8_t const slotIndex = framePack->CalculateTimeSlotFromTimeIndex( object.mTimer.mTimeIndex );
	FramePackBase::TimeSlot const& timeSlot = framePack->GetTimeSlots()[slotIndex];

	if( timeSlot.m_TextureReference == kInvalidManagedTextureID )
	{
		// Invisible frame
		return;
	}

	Texture const* texture = mTextureManager->GetResourceFromManagedResourceID( timeSlot.m_TextureReference );
	RF_ASSERT_MSG( texture != nullptr, "Failed to fetch texture" );
	DeviceTextureID const deviceTextureID = texture->GetDeviceRepresentation();

	// TODO: Transforms
	PPUCoordElem const x = object.mXCoord - timeSlot.m_TextureOriginX;
	PPUCoordElem const y = object.mYCoord - timeSlot.m_TextureOriginY;
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

	// TODO: Animations? Probably done via shaders and passing in the timer

	// TODO: Wrapping

	// TODO: Transforms

	// TODO: Flips

	uint8_t scaleUp;
	uint8_t scaleDown;
	tileLayer.GetTileZoomFactor( scaleUp, scaleDown );
	PPUCoordElem const rootX = tileLayer.mXCoord;
	PPUCoordElem const rootY = tileLayer.mYCoord;
	PPUCoordElem const xStep = ( tileset->mTileWidth * scaleUp ) / scaleDown;
	PPUCoordElem const yStep = ( tileset->mTileHeight * scaleUp ) / scaleDown;
	for( size_t i_col = 0; i_col < tileLayer.NumColumns(); i_col++ )
	{
		for( size_t i_row = 0; i_row < tileLayer.NumRows(); i_row++ )
		{
			TileLayer::Tile const& tile = tileLayer.GetTile( i_col, i_row );
			if( tile.mIndex == TileLayer::kEmptyTileIndex )
			{
				// Empty
				continue;
			}

			math::Vector2i16 const texTile = math::Vector2i16{ tile.mIndex % texTilesPerRow, tile.mIndex / texTilesPerRow };
			math::Vector2f const texTopLeft = math::Vector2f(
				texTile.x * texXStep,
				texTile.y * texYStep );
			math::Vector2f const texBottomRight = texTopLeft + math::Vector2f( texXStep, texYStep );

			PPUCoordElem const x = rootX + xStep * math::integer_cast<PPUCoordElem>( i_col );
			PPUCoordElem const y = rootY + yStep * math::integer_cast<PPUCoordElem>( i_row );
			math::Vector2f const topLeft = CoordToDevice( x, y );
			math::Vector2f const bottomRight = CoordToDevice(
				math::integer_cast<PPUCoordElem>( x + xStep ),
				math::integer_cast<PPUCoordElem>( y + yStep ) );
			mDeviceInterface->DrawBillboard( deviceTextureID, math::AABB4f{ topLeft, bottomRight }, LayerToDevice( tileLayer.mZLayer ), math::AABB4f{ texTopLeft, texBottomRight } );
		}
	}
}



void PPUController::RenderString( PPUState::String const& string ) const
{
	char const* text = string.mText;
	for( size_t i_char = 0; i_char < PPUState::String::k_MaxLen; i_char++ )
	{
		char const character = text[i_char];
		if( character == '\0' )
		{
			break;
		}
		PPUCoordElem const x1 = string.mXCoord + math::integer_cast<PPUCoordElem>( i_char * string.mWidth );
		PPUCoordElem const y1 = string.mYCoord;
		PPUCoordElem const x2 = x1 + string.mWidth;
		PPUCoordElem const y2 = y1 + string.mHeight;

		math::Vector2f const topLeft = CoordToDevice( x1, y1 );
		math::Vector2f const bottomRight = CoordToDevice( x2, y2 );
		mDeviceInterface->DrawBitmapFont( 7, character, math::AABB4f{ topLeft, bottomRight }, LayerToDevice( string.mZLayer ) );
	}
}



void PPUController::RenderDebugLine( PPUDebugState::DebugLine const& line ) const
{
	math::Vector2f const p0 = CoordToDevice( line.m_XCoord0, line.m_YCoord0 );
	math::Vector2f const p1 = CoordToDevice( line.m_XCoord1, line.m_YCoord1 );
	mDeviceInterface->DebugDrawLine( p0, p1, static_cast<float>( line.mWidth * GetZoomFactor() ) );
}



void PPUController::RenderDebugString( PPUDebugState::DebugString const& string ) const
{
	math::Vector2f const pos = CoordToDevice( string.mXCoord, string.mYCoord );
	mDeviceInterface->DebugRenderText( pos, "%s", string.mText );
}



void PPUController::RenderDebugGrid() const
{
	for( PPUCoordElem horizontal = 0; horizontal <= mWidth; horizontal += kTileSize )
	{
		math::Vector2f const posA = CoordToDevice( horizontal, 0 );
		math::Vector2f const posB = CoordToDevice( horizontal - 1, 0 );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( posA.x, 0 ),
			math::Vector2f( posA.x, 1 ),
			LayerToDevice( 0 ) );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( posB.x, 0 ),
			math::Vector2f( posB.x, 1 ),
			LayerToDevice( 0 ) );
	}
	for( PPUCoordElem vertical = 0; vertical <= mHeight; vertical += kTileSize )
	{
		math::Vector2f const posA = CoordToDevice( 0, vertical );
		math::Vector2f const posB = CoordToDevice( 0, vertical - 1 );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( 0, posA.y ),
			math::Vector2f( 1, posA.y ),
			LayerToDevice( 0 ) );
		mDeviceInterface->DebugDrawLine(
			math::Vector2f( 0, posB.y ),
			math::Vector2f( 1, posB.y ),
			LayerToDevice( 0 ) );
	}
}



uint8_t PPUController::GetZoomFactor() const
{
	uint16_t const smallestDimenssion = math::Min( mWidth, mHeight );
	uint16_t const approximateDiagonalTiles = math::integer_cast<uint16_t>( smallestDimenssion / kTileSize );
	uint8_t const zoomFactor = math::integer_cast<uint8_t>( math::Max( 1, approximateDiagonalTiles / kDesiredDiagonalTiles ) );
	return zoomFactor;
}



math::Vector2f PPUController::CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const
{
	return CoordToDevice( PPUCoord( xCoord, yCoord ) );
}



math::Vector2f PPUController::CoordToDevice( PPUCoord const& coord ) const
{
	// TODO: Windowing
	uint16_t const smallestDimenssion = math::Min( mWidth, mHeight );
	uint8_t const zoomFactor = GetZoomFactor();
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

///////////////////////////////////////////////////////////////////////////////
}}

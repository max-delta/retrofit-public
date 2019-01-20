#include "stdafx.h"
#include "PPUController.h"

#include "PPU/DeviceInterface.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"
#include "PPU/FramePackManager.h"
#include "PPU/FramePack.h"

#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstdarg"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

PPUController::PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs )
	: m_DeviceInterface( rftl::move( deviceInterface ) )
	, m_TextureManager( nullptr )
	, m_FramePackManager( nullptr )
	, m_Vfs( vfs )
{
	//
}



PPUController::~PPUController()
{
	// TODO: Proper, safe cleanup

	m_FramePackManager = nullptr;
	m_TextureManager = nullptr;

	m_DeviceInterface->DetachFromWindow();
	m_DeviceInterface = nullptr;
}



bool PPUController::Initialize( uint16_t width, uint16_t height )
{
	bool success = true;

	m_Width = width;
	m_Height = height;

	// Create texture manager
	RF_ASSERT( m_TextureManager == nullptr );
	m_TextureManager = DefaultCreator<gfx::TextureManager>::Create( m_Vfs );
	success = m_TextureManager->AttachToDevice( m_DeviceInterface );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Create frame pack manager
	RF_ASSERT( m_FramePackManager == nullptr );
	m_FramePackManager = DefaultCreator<gfx::FramePackManager>::Create( m_TextureManager, m_Vfs );

	// Prepare device
	success = m_DeviceInterface->Initialize2DGraphics();
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}
	success = m_DeviceInterface->SetBackgroundColor( 1, 0, 1, 1 );
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
	for( size_t i = 0; i < k_NumStateBuffers; i++ )
	{
		m_PPUState[i].Clear();
		m_PPUDebugState[i].Clear();
	}
	m_WriteState = k_InvalidStateBufferID;
	m_QueueToRenderState = k_InvalidStateBufferID;
	m_RenderState = k_InvalidStateBufferID;

	return true;
}



bool PPUController::ResizeSurface( uint16_t width, uint16_t height )
{
	m_Width = width;
	m_Height = height;
	bool const surfaceSuccess = m_DeviceInterface->SetSurfaceSize( width, height );
	RF_ASSERT( surfaceSuccess );
	if( surfaceSuccess == false )
	{
		return false;
	}
	bool const fontSuccess = m_DeviceInterface->SetFontScale( GetZoomFactor() );
	RF_ASSERT( fontSuccess );
	if( fontSuccess == false )
	{
		return false;
	}
	return true;
}



bool PPUController::LoadFont( FILE* file )
{
	// TODO: Revise the font API
	uint32_t unused;
	return m_DeviceInterface->CreateBitmapFont( file, 7, unused, unused );
}



PPUCoordElem PPUController::GetWidth() const
{
	return m_Width / GetZoomFactor();
}



PPUCoordElem PPUController::GetHeight() const
{
	return m_Height / GetZoomFactor();
}



bool PPUController::BeginFrame()
{
	RF_ASSERT_MSG( m_WriteState == k_InvalidStateBufferID, "Write buffer wasn't closed" );

	// Find an open buffer for writing
	for( StateBufferID i = 0; i < k_NumStateBuffers; i++ )
	{
		if( i == m_RenderState )
		{
			// Can't use render state
			continue;
		}
		if( i == m_QueueToRenderState )
		{
			// Can't use queued state
			continue;
		}

		// Found an open state, claiming it for write
		m_WriteState = i;
		break;
	}
	if( m_WriteState == k_InvalidStateBufferID )
	{
		RFLOG_FATAL( nullptr, RFCAT_PPU, "Failed to find an open write state to begin render frame" );
	}
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	RF_ASSERT( m_WriteState < k_NumStateBuffers );
	m_PPUState[m_WriteState].Clear();
	m_PPUDebugState[m_WriteState].Clear();

	// TODO: Update timers

	m_DeviceInterface->BeginFrame();
	return true;
}



bool PPUController::SubmitToRender()
{
	RF_ASSERT_MSG( m_WriteState != k_InvalidStateBufferID, "Write buffer wasn't opened" );

	// Close write buffer, and queue it
	StateBufferID const bufferToQueue = m_WriteState;
	m_WriteState = k_InvalidStateBufferID;
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

	m_DeviceInterface->EndFrame();
	return true;
}



bool PPUController::DrawObject( Object const& object )
{
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUState& targetState = m_PPUState[m_WriteState];

	// TODO: Thread-safe

	if( targetState.m_NumObjects >= PPUState::k_MaxObjects )
	{
		RF_DBGFAIL_MSG( "Too many objects drawn this frame" );
		return false;
	}

	Object& targetObject = targetState.m_Objects[targetState.m_NumObjects];
	targetState.m_NumObjects++;

	targetObject = object;

	return true;
}



bool PPUController::DrawText( PPUCoord pos, PPUCoord charSize, const char* fmt, ... )
{
	RF_ASSERT_MSG( charSize.x == 4, "TODO: Support other sizes" );
	RF_ASSERT_MSG( charSize.y == 8, "TODO: Support other sizes" );

	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUState& targetState = m_PPUState[m_WriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.m_NumStrings < PPUState::k_MaxStrings );
	PPUState::String& targetString = targetState.m_Strings[targetState.m_NumStrings];
	targetState.m_NumStrings++;

	targetString.m_XCoord = math::integer_cast<PPUCoordElem>( pos.x );
	targetString.m_YCoord = math::integer_cast<PPUCoordElem>( pos.y );
	targetString.m_Width = charSize.x;
	targetString.m_Height = charSize.y;
	targetString.m_Text[0] = '\0';
	{
		va_list args;
		va_start( args, fmt );
		vsnprintf( &targetString.m_Text[0], PPUState::String::k_MaxLen, fmt, args );
		va_end( args );
	}
	targetString.m_Text[PPUState::String::k_MaxLen] = '\0';

	return true;
}



bool PPUController::DebugDrawText( PPUCoord pos, const char* fmt, ... )
{
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUDebugState& targetState = m_PPUDebugState[m_WriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.m_NumStrings < PPUDebugState::k_MaxStrings );
	PPUDebugState::DebugString& targetString = targetState.m_Strings[targetState.m_NumStrings];
	targetState.m_NumStrings++;

	targetString.m_XCoord = math::integer_cast<PPUCoordElem>( pos.x );
	targetString.m_YCoord = math::integer_cast<PPUCoordElem>( pos.y );
	targetString.m_Text[0] = '\0';
	{
		va_list args;
		va_start( args, fmt );
		vsnprintf( &targetString.m_Text[0], PPUDebugState::DebugString::k_MaxLen, fmt, args );
		va_end( args );
	}
	targetString.m_Text[PPUDebugState::DebugString::k_MaxLen] = '\0';

	return true;
}



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width )
{
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUDebugState& targetState = m_PPUDebugState[m_WriteState];

	// TODO: Thread-safe
	RF_ASSERT( targetState.m_NumLines < PPUDebugState::k_MaxLines );
	PPUDebugState::DebugLine& targetLine = targetState.m_Lines[targetState.m_NumLines];
	targetState.m_NumLines++;

	targetLine.m_XCoord0 = math::integer_cast<PPUCoordElem>( p0.x );
	targetLine.m_YCoord0 = math::integer_cast<PPUCoordElem>( p0.y );
	targetLine.m_XCoord1 = math::integer_cast<PPUCoordElem>( p1.x );
	targetLine.m_YCoord1 = math::integer_cast<PPUCoordElem>( p1.y );
	targetLine.m_Width = width;

	return true;
}



WeakPtr<gfx::DeviceInterface> PPUController::DebugGetDeviceInterface() const
{
	return m_DeviceInterface;
}



WeakPtr<gfx::TextureManager> PPUController::DebugGetTextureManager() const
{
	return m_TextureManager;
}



WeakPtr<gfx::FramePackManager> PPUController::DebugGetFramePackManager() const
{
	return m_FramePackManager;
}

///////////////////////////////////////////////////////////////////////////////

void PPUController::SignalRender( StateBufferID readyBuffer )
{
	// TODO: If renderer open - push work, if closed - queue work
	RF_ASSERT_MSG( m_RenderState == k_InvalidStateBufferID, "TODO: Asynchronous rendering" );
	RF_ASSERT_MSG( m_QueueToRenderState == k_InvalidStateBufferID, "TODO: Asynchronous rendering" );

	// Set buffer as rendering
	m_RenderState = readyBuffer;

	// HACK: Render now
	Render();
	m_RenderState = k_InvalidStateBufferID;

	return;
}



void PPUController::Render() const
{
	RF_ASSERT( m_RenderState != k_InvalidStateBufferID );
	PPUDebugState const& targetDebugState = m_PPUDebugState[m_RenderState];
	PPUState const& targetState = m_PPUState[m_RenderState];

	// Draw objects
	for( size_t i = 0; i < targetState.m_NumObjects; i++ )
	{
		Object const& object = targetState.m_Objects[i];
		FramePackBase const* const framePack = m_FramePackManager->GetResourceFromManagedResourceID( object.m_FramePackID );
		RF_ASSERT_MSG( framePack != nullptr, "Invalid frame pack ID" );
		uint8_t const slotIndex = framePack->CalculateTimeSlotFromTimeIndex( object.m_TimeIndex );
		FramePackBase::TimeSlot const& timeSlot = framePack->GetTimeSlots()[slotIndex];

		if( timeSlot.m_TextureReference == k_InvalidManagedTextureID )
		{
			// Invisible frame
			continue;
		}

		Texture const* texture = m_TextureManager->GetResourceFromManagedResourceID( timeSlot.m_TextureReference );
		RF_ASSERT_MSG( texture != nullptr, "Failed to fetch texture" );
		DeviceTextureID const deviceTextureID = texture->GetDeviceRepresentation();

		// TODO: Transforms
		PPUCoordElem const x = object.m_XCoord - timeSlot.m_TextureOriginX;
		PPUCoordElem const y = object.m_YCoord - timeSlot.m_TextureOriginY;
		math::Vector2f topLeft = CoordToDevice( x, y );
		math::Vector2f bottomRight = CoordToDevice(
			math::integer_cast<PPUCoordElem>( x + texture->m_WidthPostLoad ),
			math::integer_cast<PPUCoordElem>( y + texture->m_HeightPostLoad ) );

		// Perform flips
		// NOTE: Assuming the device billboards are implemented similarly to a
		//  double-sided quad in a 3D pipeline
		if( object.m_HorizontalFlip )
		{
			rftl::swap( topLeft.x, bottomRight.x );
		}
		if( object.m_VerticalFlip )
		{
			rftl::swap( topLeft.y, bottomRight.y );
		}

		m_DeviceInterface->DrawBillboard( deviceTextureID, topLeft, bottomRight, object.m_ZLayer );
	}

	// Draw text
	for( size_t i = 0; i < targetState.m_NumStrings; i++ )
	{
		PPUState::String const& string = targetState.m_Strings[i];
		char const* text = string.m_Text;
		for( size_t i_char = 0; i_char < PPUState::String::k_MaxLen; i_char++ )
		{
			char const character = text[i_char];
			if( character == '\0' )
			{
				break;
			}
			PPUCoordElem const x1 = string.m_XCoord + math::integer_cast<PPUCoordElem>( i_char * string.m_Width );
			PPUCoordElem const y1 = string.m_YCoord;
			PPUCoordElem const x2 = x1 + string.m_Width;
			PPUCoordElem const y2 = y1 + string.m_Height;

			math::Vector2f const topLeft = CoordToDevice( x1, y1 );
			math::Vector2f const bottomRight = CoordToDevice( x2, y2 );
			m_DeviceInterface->DrawBitmapFont( 7, character, topLeft, bottomRight, 0 );
		}
	}

	// HACK: Draw grid
	// TODO: Configurable
	constexpr bool drawGrid = true;
	if( drawGrid )
	{
		for( PPUCoordElem horizontal = 0; horizontal <= m_Width; horizontal += k_TileSize )
		{
			math::Vector2f const posA = CoordToDevice( horizontal, 0 );
			math::Vector2f const posB = CoordToDevice( horizontal - 1, 0 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( posA.x, 0 ),
				math::Vector2f( posA.x, 1 ),
				0 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( posB.x, 0 ),
				math::Vector2f( posB.x, 1 ),
				0 );
		}
		for( PPUCoordElem vertical = 0; vertical <= m_Height; vertical += k_TileSize )
		{
			math::Vector2f const posA = CoordToDevice( 0, vertical );
			math::Vector2f const posB = CoordToDevice( 0, vertical - 1 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( 0, posA.y ),
				math::Vector2f( 1, posA.y ),
				0 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( 0, posB.y ),
				math::Vector2f( 1, posB.y ),
				0 );
		}
	}

	// Draw lines
	for( size_t i = 0; i < targetDebugState.m_NumLines; i++ )
	{
		PPUDebugState::DebugLine const& line = targetDebugState.m_Lines[i];
		math::Vector2f const p0 = CoordToDevice( line.m_XCoord0, line.m_YCoord0 );
		math::Vector2f const p1 = CoordToDevice( line.m_XCoord1, line.m_YCoord1 );
		m_DeviceInterface->DebugDrawLine( p0, p1, static_cast<float>( line.m_Width * GetZoomFactor() ) );
	}

	// Draw text
	for( size_t i = 0; i < targetDebugState.m_NumStrings; i++ )
	{
		PPUDebugState::DebugString const& string = targetDebugState.m_Strings[i];
		math::Vector2f const pos = CoordToDevice( string.m_XCoord, string.m_YCoord );
		m_DeviceInterface->DebugRenderText( pos, "%s", string.m_Text );
	}

	m_DeviceInterface->RenderFrame();
}



uint8_t PPUController::GetZoomFactor() const
{
	uint16_t const smallestDimenssion = math::Min( m_Width, m_Height );
	uint16_t const approximateDiagonalTiles = math::integer_cast<uint16_t>( smallestDimenssion / k_TileSize );
	uint8_t const zoomFactor = math::integer_cast<uint8_t>( math::Max( 1, approximateDiagonalTiles / k_DesiredDiagonalTiles ) );
	return zoomFactor;
}



math::Vector2f PPUController::CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const
{
	return CoordToDevice( PPUCoord( xCoord, yCoord ) );
}



math::Vector2f PPUController::CoordToDevice( PPUCoord const& coord ) const
{
	// TODO: Windowing
	uint16_t const smallestDimenssion = math::Min( m_Width, m_Height );
	uint8_t const zoomFactor = GetZoomFactor();
	float const diagonalTiles = ( static_cast<float>( smallestDimenssion ) ) / ( k_TileSize * zoomFactor );

	// Baseline
	// [0-64]
	float x = coord.x;
	float y = coord.y;

	// Tiles
	// [0-2]
	float const coordToTiles = 1.f / k_TileSize;
	x *= coordToTiles;
	y *= coordToTiles;

	// NDC, mHeight only
	// [0-2/15.3f]
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting mWidth
	float const heightToWidthNDC = float( m_Height ) / m_Width;
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
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
	float const heightToWidthNDC = float( m_Height ) / m_Width;
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
}

///////////////////////////////////////////////////////////////////////////////
}}

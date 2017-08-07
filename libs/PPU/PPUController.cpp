#include "stdafx.h"
#include "PPUController.h"

#include "PPU/DeviceInterface.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"

#include "core_math/math_casts.inl"

#include "core/ptr/default_creator.h"

#include <stdarg.h>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

PPUController::PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface )
	: m_DeviceInterface( std::move(deviceInterface) )
	, m_TextureManager( nullptr )
{
	//
}



PPUController::~PPUController()
{
	// TODO: Proper, safe cleanup

	// HACK: Cleanup other hack
	m_TextureManager->DestroyTexture( "Placeholder" );

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
	m_TextureManager = DefaultCreator<gfx::TextureManager>::Create();
	success = m_TextureManager->AttachToDevice( m_DeviceInterface );
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}

	// Prepare device
	success = m_DeviceInterface->Initialize2DGraphics();
	RF_ASSERT( success );
	if( success == false )
	{
		return false;
	}
	success = m_DeviceInterface->SetSurfaceSize( width, height );
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

	// Setup buffers
	for( size_t i = 0; i < k_NumStateBuffers; i++ )
	{
		m_PPUState[i].Clear();
		m_PPUDebugState[i].Clear();
	}
	m_WriteState = k_InvalidStateBufferID;
	m_QueueToRenderState = k_InvalidStateBufferID;
	m_RenderState = k_InvalidStateBufferID;

	// HACK: Throw a texture in
	m_TextureManager->LoadNewTexture( "Placeholder", "../../data/textures/common/max_delta_32.png" );

	return true;
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
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
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



bool PPUController::DebugDrawText( PPUCoord pos, const char * fmt, ... )
{
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUDebugState& targetState = m_PPUDebugState[m_WriteState];

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



bool PPUController::DebugDrawLine( PPUCoord p0, PPUCoord p1 )
{
	RF_ASSERT( m_WriteState != k_InvalidStateBufferID );
	PPUDebugState& targetState = m_PPUDebugState[m_WriteState];

	RF_ASSERT( targetState.m_NumLines < PPUDebugState::k_MaxLines );
	PPUDebugState::DebugLine& targetLine = targetState.m_Lines[targetState.m_NumLines];
	targetState.m_NumLines++;

	targetLine.m_XCoord0 = math::integer_cast<PPUCoordElem>( p0.x );
	targetLine.m_YCoord0 = math::integer_cast<PPUCoordElem>( p0.y );
	targetLine.m_XCoord1 = math::integer_cast<PPUCoordElem>( p1.x );
	targetLine.m_YCoord1 = math::integer_cast<PPUCoordElem>( p1.y );

	return true;
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

	// HACK: Do some stuff
	constexpr bool doHack = true;
	if( doHack )
	{
		WeakPtr<gfx::Texture> placeholderTex = m_TextureManager->GetDeviceTextureForRenderFromTextureName( "Placeholder" );
		gfx::TextureID const tex = placeholderTex->GetDeviceRepresentation();
		constexpr size_t k_SpriteSize = 32;
		math::Vector2f const size( 1.f/(640/k_SpriteSize), 1.f/(480/k_SpriteSize) );
		for( float hblank = 0; hblank < 1; hblank += size.y )
		{
			for( float tile = 0; tile < 1; tile += size.x )
			{
				math::Vector2f const startPos( tile, hblank );
				m_DeviceInterface->DrawBillboard(
					tex,
					startPos,
					math::Vector2f( startPos.x + size.x, startPos.y + size.y ),
					0.f );
			}
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
			math::Vector2f const posB = CoordToDevice( horizontal-1, 0 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( posA.x, 0 ),
				math::Vector2f( posA.x, 1 ) );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( posB.x, 0 ),
				math::Vector2f( posB.x, 1 ) );
		}
		for( PPUCoordElem vertical = 0; vertical <= m_Height; vertical += k_TileSize )
		{
			math::Vector2f const posA = CoordToDevice( 0, vertical );
			math::Vector2f const posB = CoordToDevice( 0, vertical-1 );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( 0, posA.y ),
				math::Vector2f( 1, posA.y ) );
			m_DeviceInterface->DebugDrawLine(
				math::Vector2f( 0, posB.y ),
				math::Vector2f( 1, posB.y ) );
		}
	}

	// Draw lines
	for( size_t i = 0; i < targetDebugState.m_NumLines; i++ )
	{
		PPUDebugState::DebugLine const& line = targetDebugState.m_Lines[i];
		math::Vector2f p0 = CoordToDevice( line.m_XCoord0, line.m_YCoord0 );
		math::Vector2f p1 = CoordToDevice( line.m_XCoord1, line.m_YCoord1 );
		m_DeviceInterface->DebugDrawLine( p0, p1 );
	}

	// Draw text
	for( size_t i = 0; i < targetDebugState.m_NumStrings; i++ )
	{
		PPUDebugState::DebugString const& string = targetDebugState.m_Strings[i];
		math::Vector2f pos = CoordToDevice( string.m_XCoord, string.m_YCoord );
		m_DeviceInterface->DebugRenderText( pos, "%s", string.m_Text );
	}

	m_DeviceInterface->RenderFrame();
}



math::Vector2f PPUController::CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const
{
	return CoordToDevice( PPUCoord( xCoord, yCoord ) );
}



math::Vector2f PPUController::CoordToDevice( PPUCoord const & coord ) const
{
	// TODO: Windowing, scaling, etc
	constexpr uint8_t zoomFactor = 1;
	constexpr uint8_t diagonalTiles = 15;

	// Baseline
	float x = coord.x;
	float y = coord.y;

	// Tiles
	float const coordToTiles = 1.f / k_TileSize;
	x *= coordToTiles;
	y *= coordToTiles;

	// NDC, height only
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting width
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

	// NDC, height only
	float const tilesToPartialNDC = 1.f / diagonalTiles;
	x *= tilesToPartialNDC;
	y *= tilesToPartialNDC;

	// NDC, correcting width
	float const heightToWidthNDC = float( m_Height ) / m_Width;
	x *= heightToWidthNDC;

	return math::Vector2f( x, y );
}

///////////////////////////////////////////////////////////////////////////////
}}

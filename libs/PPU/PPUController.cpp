#include "stdafx.h"
#include "PPUController.h"

#include "PPU/DeviceInterface.h"
#include "PPU/TextureManager.h"
#include "PPU/Texture.h"

#include "core/ptr/default_creator.h"


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

	// HACK: Throw a texture in
	m_TextureManager->LoadNewTexture( "Placeholder", "../../data/textures/common/max_delta_32.png" );

	return true;
}



bool PPUController::BeginFrame()
{
	// TODO: Set frame as open
	// TODO: Update timers
	// TODO: Get workspace

	m_DeviceInterface->BeginFrame();
	return true;
}



bool PPUController::SubmitToRender()
{
	// TODO: Set frame as rendering
	// TODO: If renderer open - push work, if closed - queue work

	// HACK: Do some stuff
	{
		WeakPtr<gfx::Texture> placeholderTex = m_TextureManager->GetDeviceTextureForRenderFromTextureName( "Placeholder" );
		gfx::TextureID const tex = placeholderTex->GetDeviceRepresentation();
		constexpr size_t k_SpriteSize = 64;
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

	// HACK: Render now
	m_DeviceInterface->RenderFrame();

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

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/PPUState.h"
#include "PPU/PPUDebugState.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUController
{
	RF_NO_COPY( PPUController );
	RF_NO_MOVE( PPUController );

	// Types
private:
	typedef int8_t StateBufferID;


	// Constants
private:
	static constexpr StateBufferID k_InvalidStateBufferID = -1;
	static constexpr size_t k_NumStateBuffers = 3;
	static_assert( k_NumStateBuffers == 3, "Need 3 buffers for triple buffering" );


	//
	// Public methods
public:
	PPUController() = delete;
	explicit PPUController( UniquePtr<gfx::DeviceInterface> && deviceInterface );
	~PPUController();

	bool Initialize( uint16_t width, uint16_t height );
	bool ResizeSurface( uint16_t width, uint16_t height );

	bool BeginFrame();
	bool SubmitToRender();
	bool WaitForRender();
	bool EndFrame();

	bool DrawObject( Object const& object );

	bool DebugDrawText( PPUCoord pos, const char *fmt, ... );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1 );
	WeakPtr<gfx::TextureManager> DebugGetTextureManager() const;
	WeakPtr<gfx::FramePackManager> DebugGetFramePackManager() const;


	//
	// Private methods
private:
	void SignalRender( StateBufferID readyBuffer );
	void Render() const;

	uint8_t GetZoomFactor() const;

	math::Vector2f CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const;
	math::Vector2f CoordToDevice( PPUCoord const& coord ) const;

	math::Vector2f TileToDevice( PPUTileElem xTile, PPUTileElem yTile ) const;

	//
	// Private data
private:
	UniquePtr<gfx::DeviceInterface> m_DeviceInterface;
	UniquePtr<gfx::TextureManager> m_TextureManager;
	UniquePtr<gfx::FramePackManager> m_FramePackManager;
	uint16_t m_Width;
	uint16_t m_Height;

	StateBufferID m_WriteState;
	StateBufferID m_QueueToRenderState;
	StateBufferID m_RenderState;
	PPUState m_PPUState[k_NumStateBuffers];
	PPUDebugState m_PPUDebugState[k_NumStateBuffers];
};

///////////////////////////////////////////////////////////////////////////////
}}

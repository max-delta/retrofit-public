#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/PPUState.h"
#include "PPU/PPUDebugState.h"

#include "PlatformFilesystem/VFSFwd.h"

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
	static constexpr StateBufferID kInvalidStateBufferID = -1;
	static constexpr size_t kNumStateBuffers = 3;
	static_assert( kNumStateBuffers == 3, "Need 3 buffers for triple buffering" );


	//
	// Public methods
public:
	PPUController() = delete;
	explicit PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs );
	~PPUController();

	bool Initialize( uint16_t width, uint16_t height );
	bool ResizeSurface( uint16_t width, uint16_t height );
	bool LoadFont( void const* buffer, size_t len );

	PPUCoordElem GetWidth() const;
	PPUCoordElem GetHeight() const;

	bool BeginFrame();
	bool SubmitToRender();
	bool WaitForRender();
	bool EndFrame();

	bool DrawObject( Object const& object );
	bool DrawTileLayer( TileLayer const& tileLayer );
	bool DrawText( PPUCoord pos, PPUCoord charSize, const char* fmt, ... );

	bool DebugDrawText( PPUCoord pos, const char* fmt, ... );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width = 0 );
	WeakPtr<gfx::DeviceInterface> DebugGetDeviceInterface() const;
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
	UniquePtr<gfx::DeviceInterface> mDeviceInterface;
	UniquePtr<gfx::TextureManager> mTextureManager;
	UniquePtr<gfx::FramePackManager> mFramePackManager;
	WeakPtr<file::VFS> const mVfs;
	uint16_t mWidth;
	uint16_t mHeight;

	StateBufferID mWriteState;
	StateBufferID mQueueToRenderState;
	StateBufferID mRenderState;
	PPUState mPPUState[kNumStateBuffers];
	PPUDebugState mPPUDebugState[kNumStateBuffers];
};

///////////////////////////////////////////////////////////////////////////////
}}

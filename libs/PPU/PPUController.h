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

	//
	// Forwards
private:
	struct DepthElement;


	//
	// Types and constants
private:
	using StateBufferID = int8_t;
	static constexpr StateBufferID kInvalidStateBufferID = -1;
	static constexpr size_t kNumStateBuffers = 3;
	static_assert( kNumStateBuffers == 3, "Need 3 buffers for triple buffering" );
	static constexpr size_t kMaxTotalElements =
		PPUState::kMaxTotalElements +
		PPUDebugState::kMaxTotalElements;
	using DepthOrder = DepthElement[kMaxTotalElements];
	enum class ElementType : uint8_t
	{
		Invalid = 0,
		Object,
		TileLayer,
		String,
		DebugLine,
		DebugString
	};


	//
	// Structs
private:
	struct DepthElement
	{
		// NOTE: Zero-initialize for optimizer to make cheap clears of array
		PPUDepthLayer mDepth = {};
		ElementType mType = {};
		static_assert( ElementType{} == ElementType::Invalid, "Non-zero invalid" );
		uint8_t mId = {};
	};


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
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, PPUCoord charSize, const char* fmt, ... );
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, PPUCoord charSize, const char* fmt, va_list args );

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

	void CalculateDepthOrder( DepthOrder& depthOrder ) const;

	void RenderObject( Object const& object ) const;
	void RenderTileLayer( TileLayer const& tileLayer ) const;
	void RenderString( PPUState::String const& string ) const;
	void RenderDebugLine( PPUDebugState::DebugLine const& line ) const;
	void RenderDebugString( PPUDebugState::DebugString const& string ) const;
	void RenderDebugGrid() const;

	uint8_t GetZoomFactor() const;

	math::Vector2f CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const;
	math::Vector2f CoordToDevice( PPUCoord const& coord ) const;
	float LayerToDevice( PPUDepthLayer zDepth ) const;

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

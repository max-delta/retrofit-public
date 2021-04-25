#pragma once
#include "project.h"

#include "PPU/PPUState.h"
#include "PPU/PPUDebugState.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"
#include "core_math/AABB4.h"
#include "core_math/Color3f.h"

#include "rftl/string"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUController
{
	RF_NO_COPY( PPUController );
	RF_NO_MOVE( PPUController );

	//
	// Forwards
private:
	struct DepthElement;
	struct LoadRequest;


	//
	// Types and constants
public:
	enum class AssetType : uint8_t
	{
		Invalid = 0,
		Texture,
		FramePack,
		Tileset,
		Font
	};

private:
	enum class LoadType : uint8_t
	{
		Invalid = 0,
		Reserve,
		New,
		Modify,
		Reload
	};
	using StateBufferID = uint8_t;
	static constexpr StateBufferID kInvalidStateBufferID = rftl::numeric_limits<StateBufferID>::max();
	static constexpr size_t kNumStateBuffers = 3;
	static_assert( kNumStateBuffers == 3, "Need 3 buffers for triple buffering" );
	static constexpr size_t kMaxTotalElements =
		PPUState::kMaxTotalElements +
		PPUDebugState::kMaxTotalElements;
	using DepthOrder = DepthElement[kMaxTotalElements];
	using LoadRequests = rftl::vector<LoadRequest>;
	using ResourceName = rftl::string;
	using Filename = file::VFSPath;
	enum class ElementType : uint8_t
	{
		Invalid = 0,
		Object,
		TileLayer,
		String,
		DebugLine,
		DebugString,
		DebugAuxString
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
	struct LoadRequest
	{
		LoadType mLoadType = {};
		AssetType mAssetType = {};
		ResourceName mResourceName = {};
		Filename mFilename = {};
	};


	//
	// Public methods
public:
	PPUController() = delete;
	explicit PPUController( UniquePtr<gfx::DeviceInterface>&& deviceInterface, WeakPtr<file::VFS> const& vfs );
	~PPUController();

	bool Initialize( uint16_t width, uint16_t height );
	bool ResizeSurface( uint16_t width, uint16_t height );

	PPUCoordElem GetWidth() const;
	PPUCoordElem GetHeight() const;

	bool BeginFrame();
	bool SubmitToRender();
	bool WaitForRender();
	bool EndFrame();

	void SuppressDrawRequests( bool suppress );

	void UpdateViewportExtents( Viewport& viewport ) const;
	void ApplyViewport( Viewport const& viewport );
	void ResetViewport();

	bool DrawObject( Object const& object );
	bool DrawTileLayer( TileLayer const& tileLayer );
	bool DrawText( PPUCoord pos, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... );
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... );
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, math::Color3f color, const char* fmt, ... );
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... );
	bool DrawText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args );

	void HideZoomFactor( bool hide );
	PPUZoomFactor GetCurrentZoomFactor() const;
	PPUCoordElem CalculateStringLengthFormatted( uint8_t desiredHeight, ManagedFontID fontID, char const* fmt, ... );
	PPUCoordElem CalculateStringLength( uint8_t desiredHeight, ManagedFontID fontID, char const* text );

	PPUCoord CalculateTileLayerSize( TileLayer const& tileLayer ) const;

	bool HasOutstandingLoadRequests() const;
	bool QueueDeferredLoadRequest( AssetType type, Filename const& filename );
	bool QueueDeferredLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );
	bool QueueDeferredReloadRequest( AssetType type, ResourceName const& resourceName );
	bool ForceImmediateLoadRequest( AssetType type, Filename const& filename );
	bool ForceImmediateLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );

	WeakPtr<TextureManager const> GetTextureManager() const;
	WeakPtr<FramePackManager const> GetFramePackManager() const;
	WeakPtr<TilesetManager const> GetTilesetManager() const;
	WeakPtr<FontManager const> GetFontManager() const;

	void DebugSetGridEnabled( bool enabled );
	void DebugSetBackgroundColor( math::Color3f color );
	bool DebugDrawText( PPUCoord pos, const char* fmt, ... );
	bool DebugDrawAuxText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... );
	bool DebugDrawAuxText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1 );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, math::Color3f color );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width, PPUDepthLayer zLayer, math::Color3f color );
	bool DebugDrawAABB( AABB aabb );
	bool DebugDrawAABB( AABB aabb, PPUCoordElem width );
	bool DebugDrawAABB( AABB aabb, math::Color3f color );
	bool DebugDrawAABB( AABB aabb, PPUCoordElem width, PPUDepthLayer zLayer, math::Color3f color );
	WeakPtr<DeviceInterface> DebugGetDeviceInterface() const;
	WeakPtr<TextureManager> DebugGetTextureManager() const;
	WeakPtr<FramePackManager> DebugGetFramePackManager() const;
	WeakPtr<TilesetManager> DebugGetTilesetManager() const;
	WeakPtr<FontManager> DebugGetFontManager() const;


	//
	// Private methods
private:
	void SignalRender( StateBufferID readyBuffer );
	void Render() const;

	void CalculateDepthOrder( DepthOrder& depthOrder ) const;

	void CalculateDesiredFontZoomShrink( Font const& font, uint8_t desiredHeight, uint8_t& zoomDesired, uint8_t& shrinkDesired ) const;
	void CalculateFontVariableWhitespaceWidth( Font const& font, uint8_t& whitespaceWidth ) const;
	void CalculateFontVariableCharWidth( Font const& font, char character, uint8_t whitespaceWidth, uint8_t zoomDesired, uint8_t shrinkDesired, uint8_t& varCharWidth ) const;

	void CalculateTileSize( TileLayer const& tileLayer, Tileset const& tileset, PPUCoordElem& tileWidth, PPUCoordElem& tileHeight ) const;

	static void ConvertColor( uint8_t ( &dest )[3], math::Color3f const& src );
	static math::Color3f ConvertColor( uint8_t const ( &src )[3] );

	static math::Color3f CalculateBorderColor( math::Color3f contentsColor );

	void RenderObject( Object const& object ) const;
	void RenderTileLayer( TileLayer const& tileLayer ) const;
	void RenderString( PPUState::String const& string ) const;
	void RenderDebugLine( PPUDebugState::DebugLine const& line ) const;
	void RenderDebugString( PPUDebugState::DebugString const& string ) const;
	void RenderDebugGrid() const;

	PPUZoomFactor GetZoomFactor() const;
	math::Vector2f GetDevicePixelStep( bool allowZoomHiding ) const;

	math::Vector2f CoordToDevice( PPUCoordElem xCoord, PPUCoordElem yCoord ) const;
	math::Vector2f CoordToDevice( PPUCoord const& coord ) const;
	float LayerToDevice( PPUDepthLayer zDepth ) const;

	math::Vector2f TileToDevice( PPUTileElem xTile, PPUTileElem yTile ) const;

	void FullfillAllDeferredLoadRequests();
	bool FullfillLoadRequest( LoadRequest const& request );


	//
	// Private data
private:
	UniquePtr<DeviceInterface> mDeviceInterface;
	UniquePtr<TextureManager> mTextureManager;
	UniquePtr<FramePackManager> mFramePackManager;
	UniquePtr<TilesetManager> mTilesetManager;
	UniquePtr<FontManager> mFontManager;
	WeakPtr<file::VFS> const mVfs;

	LoadRequests mDeferredLoadRequests;

	uint16_t mWidth = 0;
	uint16_t mHeight = 0;

	PPUVec mDrawOffset = {};

	bool mDrawRequestsSuppressed = false;
	bool mHideZoomFactor = false;
	bool mDebugDrawGrid = true;

	StateBufferID mWriteState;
	StateBufferID mQueueToRenderState;
	StateBufferID mRenderState;
	PPUState mPPUState[kNumStateBuffers];
	PPUDebugState mPPUDebugState[kNumStateBuffers];
};

///////////////////////////////////////////////////////////////////////////////
}

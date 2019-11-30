#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/PPUState.h"
#include "PPU/PPUDebugState.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"
#include "core_math/AABB4.h"
#include "core_math/Color3f.h"

#include "rftl/string"


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

	bool QueueDeferredLoadRequest( AssetType type, Filename const& filename );
	bool QueueDeferredLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );
	bool QueueDeferredReloadRequest( AssetType type, ResourceName const& resourceName );
	bool ForceImmediateLoadRequest( AssetType type, Filename const& filename );
	bool ForceImmediateLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );

	WeakPtr<gfx::TextureManager const> GetTextureManager() const;
	WeakPtr<gfx::FramePackManager const> GetFramePackManager() const;
	WeakPtr<gfx::TilesetManager const> GetTilesetManager() const;
	WeakPtr<gfx::FontManager const> GetFontManager() const;

	void DebugSetGridEnabled( bool enabled );
	void DebugSetBackgroundColor( math::Color3f color );
	bool DebugDrawText( PPUCoord pos, const char* fmt, ... );
	bool DebugDrawAuxText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... );
	bool DebugDrawAuxText( PPUCoord pos, PPUDepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1 );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, math::Color3f color );
	bool DebugDrawLine( PPUCoord p0, PPUCoord p1, PPUCoordElem width, PPUDepthLayer zLayer, math::Color3f color );
	bool DebugDrawAABB( math::AABB4<PPUCoordElem> aabb );
	bool DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, PPUCoordElem width );
	bool DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, math::Color3f color );
	bool DebugDrawAABB( math::AABB4<PPUCoordElem> aabb, PPUCoordElem width, PPUDepthLayer zLayer, math::Color3f color );
	WeakPtr<gfx::DeviceInterface> DebugGetDeviceInterface() const;
	WeakPtr<gfx::TextureManager> DebugGetTextureManager() const;
	WeakPtr<gfx::FramePackManager> DebugGetFramePackManager() const;
	WeakPtr<gfx::TilesetManager> DebugGetTilesetManager() const;
	WeakPtr<gfx::FontManager> DebugGetFontManager() const;


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
	UniquePtr<gfx::DeviceInterface> mDeviceInterface;
	UniquePtr<gfx::TextureManager> mTextureManager;
	UniquePtr<gfx::FramePackManager> mFramePackManager;
	UniquePtr<gfx::TilesetManager> mTilesetManager;
	UniquePtr<gfx::FontManager> mFontManager;
	WeakPtr<file::VFS> const mVfs;

	LoadRequests mDeferredLoadRequests;

	uint16_t mWidth;
	uint16_t mHeight;

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
}}

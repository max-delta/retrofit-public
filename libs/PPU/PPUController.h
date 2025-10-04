#pragma once
#include "project.h"

#include "PPU/PPUState.h"
#include "PPU/PPUDebugState.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"
#include "core_math/AABB4.h"
#include "core_math/Color3f.h"

#include "rftl/string"
#include "rftl/string_view"


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
	enum class DrawOverflowBehavior : uint8_t
	{
		OverwriteSilent = 0,
		OverwriteAssert,
		FlickerSilent,
		FlickerAssert,
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
		DepthLayer mDepth = {};
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

	CoordElem GetWidth() const;
	CoordElem GetHeight() const;

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
	bool DrawText( Coord pos, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... );
	bool DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, const char* fmt, ... );
	bool DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, math::Color3f color, const char* fmt, ... );
	bool DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... );
	bool DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args );

	void HideZoomFactor( bool hide );
	ZoomFactor GetCurrentZoomFactor() const;
	CoordElem CalculateStringLengthFormatted( uint8_t desiredHeight, ManagedFontID fontID, char const* fmt, ... ) const;
	CoordElem CalculateStringLength( uint8_t desiredHeight, ManagedFontID fontID, rftl::string_view text ) const;

	Coord CalculateTileLayerSize( TileLayer const& tileLayer ) const;

	bool HasOutstandingLoadRequests() const;
	bool QueueDeferredLoadRequest( AssetType type, Filename const& filename );
	bool QueueDeferredLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );
	bool QueueDeferredReloadRequest( AssetType type, ResourceName const& resourceName );
	bool ForceImmediateLoadRequest( AssetType type, Filename const& filename );
	bool ForceImmediateLoadRequest( AssetType type, ResourceName const& resourceName, Filename const& filename );
	bool ForceImmediateLoadAllRequests();

	WeakPtr<TextureManager const> GetTextureManager() const;
	WeakPtr<FramePackManager const> GetFramePackManager() const;
	WeakPtr<TilesetManager const> GetTilesetManager() const;
	WeakPtr<FontManager const> GetFontManager() const;

	void DebugSetGridEnabled( bool enabled );
	void DebugSetBackgroundColor( math::Color3f color );

	template<typename... ArgsT>
	bool DebugDrawText( Coord pos, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
	{
		return DebugDrawTextVA( pos, fmt.get(), rftl::make_format_args( args... ) );
	}
	bool DebugDrawTextVA( Coord pos, rftl::string_view fmt, rftl::format_args&& args );

	bool DebugDrawAuxText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, ... );
	bool DebugDrawAuxText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, const char* fmt, va_list args );

	bool DebugDrawLine( Coord p0, Coord p1 );
	bool DebugDrawLine( Coord p0, Coord p1, CoordElem width );
	bool DebugDrawLine( Coord p0, Coord p1, math::Color3f color );
	bool DebugDrawLine( Coord p0, Coord p1, CoordElem width, DepthLayer zLayer, math::Color3f color );
	bool DebugDrawAABB( AABB aabb );
	bool DebugDrawAABB( AABB aabb, CoordElem width );
	bool DebugDrawAABB( AABB aabb, math::Color3f color );
	bool DebugDrawAABB( AABB aabb, CoordElem width, DepthLayer zLayer, math::Color3f color );
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
	void CalculateFontVariableCharHeight( Font const& font, char character, uint8_t zoomDesired, uint8_t shrinkDesired, uint8_t& varCharHeight ) const;

	void CalculateTileSize( TileLayer const& tileLayer, Tileset const& tileset, CoordElem& tileWidth, CoordElem& tileHeight ) const;

	static void ConvertColor( rftl::array<uint8_t, 3>& dest, math::Color3f const& src );
	static math::Color3f ConvertColor( rftl::array<uint8_t, 3> const& src );

	static math::Color3f CalculateBorderColor( math::Color3f contentsColor );

	void RenderObject( Object const& object ) const;
	void RenderTileLayer( TileLayer const& tileLayer ) const;
	void RenderString( PPUState::String const& string, rftl::string_view const& text ) const;
	void RenderDebugLine( PPUDebugState::DebugLine const& line ) const;
	void RenderDebugString( PPUDebugState::DebugString const& string ) const;
	void RenderDebugGrid() const;

	ZoomFactor GetZoomFactor() const;
	math::Vector2f GetDevicePixelStep( bool allowZoomHiding ) const;

	math::Vector2f CoordToDevice( CoordElem xCoord, CoordElem yCoord ) const;
	math::Vector2f CoordToDevice( Coord const& coord ) const;
	float LayerToDevice( DepthLayer zDepth ) const;

	math::Vector2f TileToDevice( TileElem xTile, TileElem yTile ) const;

	template<DrawOverflowBehavior BehaviorT, typename TypeT, size_t MaxCountT, typename SizeT>
	auto RenderStateListItemSelect( rftl::array<TypeT, MaxCountT>& list, SizeT& count, char const* name ) -> TypeT&;

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

	Vec2 mDrawOffset = {};

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

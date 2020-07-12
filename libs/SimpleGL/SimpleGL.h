#pragma once
#include "project.h"

#include "PPU/DeviceInterface.h"

#include "rftl/unordered_map"
#include "rftl/array"

// Forwards
namespace RF { namespace gfx {
class SIMPLEGL_API DeviceInterface;
}}

namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class SIMPLEGL_API SimpleGL final : public DeviceInterface
{
	RF_NO_COPY( SimpleGL );

	//
	// Types and constants
public:
	enum class ProjectionMode
	{
		TRUE_BUFFER_00UPLEFT, // A unit is a buffer pixel
		NDC01_00UPLEFT, // NDC 0<>1
		NDC01_00DWNLEFT, // NDC 0<>1
		NDC11_11UPRIGHT, // NDC -1<>1
	};
	using BitmapCharacterListStorage = rftl::array<DeviceTextureID, 256>;
	using BitmapFontStorage = rftl::unordered_map<DeviceFontID, BitmapCharacterListStorage>;


	//
	// Public methods
public:
	SimpleGL() = default;

	bool AttachToWindow( shim::HWND hWnd ) override;
	bool DetachFromWindow() override;
	bool Initialize2DGraphics() override;

	bool SetProjectionMode( ProjectionMode mode );
	bool SetSurfaceSize( uint16_t width, uint16_t height ) override;
	bool SetBackgroundColor( math::Color3f color ) override;
	bool SetFontScale( float scale ) override;

	DeviceTextureID LoadTexture( rftl::byte_view const& buffer, uint32_t& width, uint32_t& height ) override;
	bool UnloadTexture( DeviceTextureID textureID ) override;

	DeviceFontID CreateBitmapFont( rftl::byte_view const& buffer, uint32_t& characterWidth, uint32_t& characterHeight, rftl::array<uint32_t, 256>* variableWidth ) override;
	bool DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color ) override;
	bool DrawBitmapFont( DeviceFontID fontID, char character, math::AABB4f pos, float z, math::Color3f color, math::AABB4f texUV ) override;

	bool glPrint( char const* fmt, ... );
	bool glPrint( char const* fmt, va_list args );

	bool DebugRenderText( math::Vector2f pos, float z, const char* fmt, ... ) override;
	bool DebugDrawLine( math::Vector2f p0, math::Vector2f p1, float z, float width, math::Color3f color ) override;
	bool DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z ) override;
	bool DrawBillboard( DeviceTextureID textureID, math::AABB4f pos, float z, math::AABB4f texUV ) override;

	bool BeginFrame() override;
	bool RenderFrame() override;
	bool EndFrame() override;


	//
	// Private methods
private:
	void BuildFont( int8_t height );
	bool DrawBillboardInternal( DeviceTextureID textureID, math::AABB4f pos, float z, math::AABB4f texUV );


	//
	// Private data
private:
	shim::HWND mHWnd;
	shim::HDC mHDC;
	shim::HGLRC mHRC;
	ProjectionMode mProjectionMode;
	int mWidth;
	int mHeight;
	float mXFudge; // HACK: Pixel fudge for anti-aliasing repair
	float mYFudge; // HACK: Pixel fudge for anti-aliasing repair
	BitmapFontStorage mBitmapFonts;
	DeviceFontID mLastCreatedFontId = kInvalidDeviceFontID;
	unsigned int font_base;
};

///////////////////////////////////////////////////////////////////////////////
}}

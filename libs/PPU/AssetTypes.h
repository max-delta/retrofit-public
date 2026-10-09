#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/meta/FailTemplate.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

enum class AssetType : uint8_t
{
	Invalid = 0,
	Texture,
	FramePack,
	Tileset,
	Font
};



template<AssetType T>
struct AssetTypeInfo
{
	static_assert( FailTemplate<T>(), "Unspecialized type" );
	using ManagedTypeID = void*;
	using Manager = void*;
	static constexpr ManagedTypeID kInvalidTypeID = nullptr;
};
template<>
struct AssetTypeInfo<AssetType::Texture>
{
	using ManagedTypeID = ManagedTextureID;
	using Manager = TextureManager;
	static constexpr ManagedTypeID kInvalidTypeID = kInvalidManagedTextureID;
};
template<>
struct AssetTypeInfo<AssetType::FramePack>
{
	using ManagedTypeID = ManagedFramePackID;
	using Manager = FramePackManager;
	static constexpr ManagedTypeID kInvalidTypeID = kInvalidManagedFramePackID;
};
template<>
struct AssetTypeInfo<AssetType::Tileset>
{
	using ManagedTypeID = ManagedTilesetID;
	using Manager = TilesetManager;
	static constexpr ManagedTypeID kInvalidTypeID = kInvalidManagedTilesetID;
};
template<>
struct AssetTypeInfo<AssetType::Font>
{
	using ManagedTypeID = ManagedFontID;
	using Manager = FontManager;
	static constexpr ManagedTypeID kInvalidTypeID = kInvalidManagedFontID;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include <stdint.h>

namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

typedef uint64_t TextureID;
constexpr TextureID k_InvalidTextureID = 0;

typedef uint64_t FrameID;
constexpr FrameID k_InvalidFrameID = 0;
constexpr FrameID k_FirstFrameID = 0;

class Texture;
class TextureManager;
class DeviceInterface;
///////////////////////////////////////////////////////////////////////////////
}}

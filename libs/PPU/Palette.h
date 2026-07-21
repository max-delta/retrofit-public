#pragma once
#include "project.h"

#include "PPU/GfxFwd.h"

#include "rftl/array"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

template<typename ColorT, size_t CountT>
class Palette : public rftl::array<ColorT, CountT>
{
public:
	using Color = ColorT;
	static constexpr size_t kCount = CountT;
private:
	using Base = rftl::array<ColorT, CountT>;
};

///////////////////////////////////////////////////////////////////////////////
}

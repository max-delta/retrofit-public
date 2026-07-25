#pragma once
#include "Palette.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

template<typename ColorT, size_t CountT>
inline constexpr Palette<ColorT, CountT> Palette<ColorT, CountT>::CreateFromFill( Color const& fillColor )
{
	Palette retVal = {};
	for( Color& color : retVal )
	{
		color = fillColor;
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

#pragma once
#include "PPUController.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr DepthLayer kDefaultTextLayer = 0;

}
///////////////////////////////////////////////////////////////////////////////

template<typename... ArgsT>
inline bool PPUController::DrawText( Coord pos, uint8_t desiredHeight, ManagedFontID font, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DrawTextVA( pos, details::kDefaultTextLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt.get(), rftl::make_format_args( args... ) );
}

template<typename... ArgsT>
inline bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DrawTextVA( pos, zLayer, desiredHeight, font, false, math::Color3f::kBlack, fmt.get(), rftl::make_format_args( args... ) );
}

template<typename... ArgsT>
inline bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, math::Color3f color, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DrawTextVA( pos, zLayer, desiredHeight, font, false, color, fmt.get(), rftl::make_format_args( args... ) );
}

template<typename... ArgsT>
inline bool PPUController::DrawText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DrawTextVA( pos, zLayer, desiredHeight, font, border, color, fmt.get(), rftl::make_format_args( args... ) );
}



template<typename... ArgsT>
inline bool PPUController::DebugDrawText( Coord pos, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DebugDrawTextVA( pos, fmt.get(), rftl::make_format_args( args... ) );
}



template<typename... ArgsT>
inline bool PPUController::DebugDrawAuxText( Coord pos, DepthLayer zLayer, uint8_t desiredHeight, ManagedFontID font, bool border, math::Color3f color, rftl::format_string<ArgsT...> fmt, ArgsT&&... args )
{
	return DebugDrawAuxTextVA( pos, zLayer, desiredHeight, font, border, color, fmt.get(), rftl::make_format_args( args... ) );
}

///////////////////////////////////////////////////////////////////////////////
}

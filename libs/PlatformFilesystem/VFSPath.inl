#pragma once
#include "VFSPath.h"

namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

template<typename... Nodes>
inline RF::file::VFSPath::VFSPath( Element element, Nodes... elements )
	: SegmentedIdentifier( element, elements... )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}

// Formats as a basic_string_view
template<typename CtxCharT>
struct rftl::formatter<RF::file::VFSPath, CtxCharT> : rftl::formatter<rftl::basic_string_view<CtxCharT>, CtxCharT>
{
	using Input = RF::file::VFSPath;
	using Shim = rftl::basic_string_view<CtxCharT>;
	using Base = rftl::formatter<Shim, CtxCharT>;

	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		return Base::parse( ctx );
	}

	template<class FmtContext>
	typename FmtContext::iterator format( Input const& arg, FmtContext& ctx ) const
	{
		return Base::format( static_cast<Shim>( arg.CreateString() ), ctx );
	}
};

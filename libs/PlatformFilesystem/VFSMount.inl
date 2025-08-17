#pragma once
#include "VFSMount.inl"

#include "core_math/math_casts.h"

#include "rftl/extension/parseless_format.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}

template<typename CtxCharT>
struct rftl::formatter<RF::file::VFSMount::Permissions, CtxCharT> : public rftl::parseless_format
{
	template<class FmtContext>
	typename FmtContext::iterator format( RF::file::VFSMount::Permissions const& arg, FmtContext& ctx ) const
	{
		uint8_t const asBits = RF::math::enum_bitcast( arg );

		auto iter = ctx.out();
		auto const emit = [&iter, asBits]( uint8_t const& test, CtxCharT ch ) -> void
		{
			*iter = asBits & test ? ch : '-';
			iter++;
		};
		emit( RF::file::VFSMount::kReadBit, 'r' );
		emit( RF::file::VFSMount::kWriteBit, 'w' );
		emit( RF::file::VFSMount::kExecuteBit, 'x' );
		return iter;
	}
};

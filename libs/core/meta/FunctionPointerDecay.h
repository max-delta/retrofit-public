#pragma once
#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

// It is technically standards-compliant to decay function pointers into void
//  pointers, but that's dubious, so we use MSVC's non-standard feature to
//  block that, requiring the use of this function instead
// SEE: /Zc:functionToObjectPtr
template<typename FuncT>
void const* DecayFunction( FuncT* funcPtr )
{
	static_assert( rftl::is_function<FuncT>::value );
	static_assert( rftl::is_pointer<FuncT*>::value );
	return reinterpret_cast<void const*>( funcPtr );
}

///////////////////////////////////////////////////////////////////////////////
}

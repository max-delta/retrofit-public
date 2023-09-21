#pragma once
#include "core_rftype/RFTypeFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

struct ConstructedType
{
	RF_NO_COPY( ConstructedType );

	ConstructedType() = default;
	ConstructedType( ConstructedType&& rhs );

	reflect::ClassInfo const* mClassInfo = nullptr;
	UniquePtr<void> mLocation = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}

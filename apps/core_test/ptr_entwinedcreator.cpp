#include "stdafx.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/entwined_creator.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

TEST( EntwinedCreator, Basic )
{
	{
		UniquePtr<int> uptr = EntwinedCreator<int>::Create( 47 );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

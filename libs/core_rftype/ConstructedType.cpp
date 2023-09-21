#include "stdafx.h"
#include "ConstructedType.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

ConstructedType::ConstructedType( ConstructedType&& rhs )
	: mClassInfo( rftl::move( rhs.mClassInfo ) )
	, mLocation( rftl::move( rhs.mLocation ) )
{
	rhs.mClassInfo = nullptr;
	rhs.mLocation = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

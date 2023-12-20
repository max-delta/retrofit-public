#include "stdafx.h"
#include "CastErrorContext.h"

#include "cc3o3/casting/CastError.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::cast::CastErrorContext )
{
	RFTYPE_META().BaseClass<RF::act::Context>();
}

namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

CastErrorContext::CastErrorContext( UniquePtr<CastError>&& castError )
	: mCastError( rftl::move( castError ) )
{
	RF_ASSERT( mCastError != nullptr );
}



CastErrorContext::CastErrorContext( CastError const& castError )
	: CastErrorContext( castError.Clone() )
{
	//
}



UniquePtr<act::Context> CastErrorContext::Clone() const
{
	RF_ASSERT( mCastError != nullptr );
	return DefaultCreator<CastErrorContext>::Create( *mCastError );
}

///////////////////////////////////////////////////////////////////////////////
}

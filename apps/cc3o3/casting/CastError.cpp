#include "stdafx.h"
#include "CastError.h"

#include "GameAction/Context.h"
#include "GameAction/Environment.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

CastError::CastError(
	act::Environment const& env,
	act::Context const& ctx )
	: mEnv( DefaultCreator<act::Environment>::Create( env ) )
	, mCtx( ctx.Clone() )
{
	//
}



CastError::~CastError() = default;



UniquePtr<CastError> CastError::Create(
	act::Environment const& env,
	act::Context const& ctx )
{
	return DefaultCreator<CastError>::Create(
		env,
		ctx );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "ElemDamageStep.h"

#include "cc3o3/casting/Contexts/CastErrorContext.h"
#include "cc3o3/casting/Contexts/CombatContext.h"
#include "cc3o3/casting/CastError.h"
#include "cc3o3/combat/Cast.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_math/math_casts.h"
#include "core_rftype/VirtualCast.h"


RFTYPE_CREATE_META( RF::cc::cast::step::ElemDamageStep )
{
	using RF::cc::cast::step::ElemDamageStep;
	RFTYPE_META().BaseClass<RF::act::Step>();
	RFTYPE_META().RawProperty( "mElemStrengthModifier", &ElemDamageStep::mElemStrengthModifier );
	RFTYPE_META().RawProperty( "mCastLevelModifier", &ElemDamageStep::mCastLevelModifier );
	RFTYPE_REGISTER_BY_NAME( "ElemDamageStep" );
	RFTYPE_REGISTER_DEFAULT_CREATOR();
}

namespace RF::cc::cast::step {
///////////////////////////////////////////////////////////////////////////////

UniquePtr<act::Context> ElemDamageStep::Execute( act::Environment const& env, act::Context& ctx ) const
{
	CombatContext* const combatCtxPtr = rftype::virtual_cast<CombatContext*>( &ctx );
	if( combatCtxPtr == nullptr )
	{
		// Non-combat context
		UniquePtr<CastErrorContext> err = CastErrorContext::Create( env, ctx );
		err->mCastError->mMissingCombatContext = true;
		return err;
	}
	CombatContext& combatCtx = *combatCtxPtr;

	// Apply the damage
	combat::CastDamageResult const result = combatCtx.mCombatInstance.ApplyCastDamage(
		combatCtx.mSourceFighter,
		combatCtx.mTargetFighter,
		math::integer_cast<combat::SimVal>(
			combatCtx.mElementStrength + mElemStrengthModifier ),
		math::integer_cast<combat::SimVal>(
			combatCtx.mCastedLevel + mCastLevelModifier ),
		combatCtx.mMultiTarget,
		combatCtx.mElementColor );
	RF_ASSERT_MSG( result.mDamage > 0, "No damage applied, suspicious" );
	RF_ASSERT_MSG( result.mCoungerGuageIncrease > 0, "No guage increase, suspicious" );

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

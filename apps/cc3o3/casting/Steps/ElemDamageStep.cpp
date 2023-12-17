#include "stdafx.h"
#include "ElemDamageStep.h"

#include "cc3o3/casting/CombatContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/VirtualCast.h"


RFTYPE_CREATE_META( RF::cc::cast::step::ElemDamageStep )
{
	using RF::cc::cast::step::ElemDamageStep;
	RFTYPE_META().BaseClass<RF::act::Step>();
	RFTYPE_META().RawProperty( "mElemStrengthLevelModifier", &ElemDamageStep::mElemStrengthLevelModifier );
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
		RF_TODO_BREAK_MSG( "Return an error context" );
		RF_RETAIL_FATAL_MSG( "TODO", "TODO" );
		//return CastErrorContext;
	}
	CombatContext& combatCtx = *combatCtxPtr;

	// TODO: Actual implementation, done through the combat engine, with more
	//  information available in the context, like the element level, color,
	//  up/downlevel, etc
	combatCtx.mCombatInstance.DecreaseHealth(
		combatCtx.mTargetFighter,
		static_cast<combat::SimVal>( // HACK: Just showing value usage
			1 + mElemStrengthLevelModifier ) );

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}

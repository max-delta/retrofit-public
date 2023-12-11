#include "stdafx.h"
#include "ConditionRecord.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ConditionRecord::ConditionRecord() = default;



UniquePtr<Condition> ConditionRecord::ReplaceRoot( UniquePtr<Condition>&& newRoot )
{
	UniquePtr<Condition> retVal = rftl::move( mRootCondition );
	mRootCondition = rftl::move( newRoot );
	return retVal;
}



WeakPtr<Condition const> ConditionRecord::GetRoot() const
{
	return mRootCondition;
}



WeakPtr<Condition> ConditionRecord::GetMutableRoot()
{
	return mRootCondition;
}

///////////////////////////////////////////////////////////////////////////////
}

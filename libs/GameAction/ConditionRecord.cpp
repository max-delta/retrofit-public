#include "stdafx.h"
#include "ConditionRecord.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ConditionRecord::ConditionRecord() = default;



UniquePtr<Check> ConditionRecord::ReplaceRoot( UniquePtr<Check>&& newRoot )
{
	UniquePtr<Check> retVal = rftl::move( mRootCheck );
	mRootCheck = rftl::move( newRoot );
	return retVal;
}



WeakPtr<Check const> ConditionRecord::GetRoot() const
{
	return mRootCheck;
}



WeakPtr<Check> ConditionRecord::GetMutableRoot()
{
	return mRootCheck;
}

///////////////////////////////////////////////////////////////////////////////
}

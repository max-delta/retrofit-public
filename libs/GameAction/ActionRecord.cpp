#include "stdafx.h"
#include "ActionRecord.h"


namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ActionRecord::ActionRecord() = default;



UniquePtr<Step> ActionRecord::ReplaceRoot( UniquePtr<Step>&& newRoot )
{
	UniquePtr<Step> retVal = rftl::move( mRootStep );
	mRootStep = rftl::move( newRoot );
	return retVal;
}



WeakPtr<Step const> ActionRecord::GetRoot() const
{
	return mRootStep;
}



WeakPtr<Step> ActionRecord::GetMutableRoot()
{
	return mRootStep;
}

///////////////////////////////////////////////////////////////////////////////
}

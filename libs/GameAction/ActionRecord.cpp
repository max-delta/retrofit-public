#include "stdafx.h"
#include "ActionRecord.h"

#include "GameAction/Step.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/rf_extensions/UniquePtr.h"


RFTYPE_CREATE_META( RF::act::ActionRecord )
{
	using RF::act::ActionRecord;
	RFTYPE_META().ExtensionProperty( "mRootStep", &ActionRecord::mRootStepInternal );
	RFTYPE_REGISTER_BY_NAME( "ActionRecord" );
}

namespace RF::act {
///////////////////////////////////////////////////////////////////////////////

ActionRecord::ActionRecord() = default;



UniquePtr<Step> ActionRecord::ReplaceRoot( UniquePtr<Step>&& newRoot )
{
	UniquePtr<Step> retVal = rftl::move( mRootStepInternal );
	mRootStepInternal = rftl::move( newRoot );
	return retVal;
}



WeakPtr<Step const> ActionRecord::GetRoot() const
{
	return mRootStepInternal;
}



WeakPtr<Step> ActionRecord::GetMutableRoot()
{
	return mRootStepInternal;
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "InstancedController.h"

#include "GameUI/Container.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/FocusManager.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::ui::controller::InstancedController )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::InstancedController );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

InstancedController::InstancedController()
	: mFocusTarget( DefaultCreator<FocusTarget>::Create() )
{
	FocusTarget& focusTarget = *mFocusTarget;
	focusTarget.mUserData = this;
	focusTarget.mEventHandler = HandleFocusEvent;

	// Disable until initialized
	focusTarget.mIsFocusable = false;
}



WeakPtr<FocusTreeNode> InstancedController::AddAsChildToFocusTreeNode( FocusManager& focusManager, FocusTreeNode const& parentNode )
{
	WeakPtr<FocusTreeNode> const retVal = focusManager.GetMutableFocusTree().CreateNewChild( parentNode, mFocusTarget );
	if( retVal != nullptr )
	{
		OnAddedToFocusTree( focusManager, *retVal );
	}
	return retVal;
}



void InstancedController::OnAddedToFocusTree( FocusManager& focusManager, FocusTreeNode const& newNode )
{
	//
}



bool InstancedController::OnFocusEvent( FocusEvent const& focusEvent )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////

void InstancedController::InitializeFocus( Container& container )
{
	FocusTarget& focusTarget = *mFocusTarget;
	focusTarget.mContainerID = container.mContainerID;

	// Enable on initialization by default
	focusTarget.mIsFocusable = false;
}



FocusTarget& InstancedController::GetFocusTarget()
{
	return *mFocusTarget;
}

///////////////////////////////////////////////////////////////////////////////

bool InstancedController::HandleFocusEvent( void* userData, FocusEvent const& focusEvent )
{
	RF_ASSERT( userData != nullptr );
	return reinterpret_cast<InstancedController*>( userData )->OnFocusEvent( focusEvent );
}

///////////////////////////////////////////////////////////////////////////////
}}}

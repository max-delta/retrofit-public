#include "stdafx.h"
#include "InstancedController.h"

#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
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



ContainerID InstancedController::GetContainerID() const
{
	return mContainerID;
}



bool InstancedController::IsCurrentFocus( UIConstContext const& context ) const
{
	return context.GetFocusManager().GetFocusTree().IsCurrentFocus( *mFocusTarget );
}



bool InstancedController::IsInCurrentFocusStack( UIConstContext const& context ) const
{
	return context.GetFocusManager().GetFocusTree().IsInCurrentFocusStack( *mFocusTarget );
}



WeakPtr<FocusTreeNode const> InstancedController::GetFocusTreeNode( UIConstContext const& context ) const
{
	return mFocusTarget->mFocusTreeNode;
}



WeakPtr<FocusTreeNode> InstancedController::GetMutableFocusTreeNode( UIContext& context )
{
	return mFocusTarget->mFocusTreeNode;
}



WeakPtr<FocusTreeNode> InstancedController::AddAsChildToFocusTreeNode( UIContext& context, FocusTreeNode& parentNode )
{
	WeakPtr<FocusTreeNode> const retVal = context.GetMutableFocusManager().GetMutableFocusTree().CreateNewChild( parentNode, mFocusTarget );
	if( retVal != nullptr )
	{
		OnAddedToFocusTree( context, *retVal );
	}
	return retVal;
}



WeakPtr<FocusTreeNode> InstancedController::AddAsSiblingAfterFocusTreeNode( UIContext& context, WeakPtr<FocusTreeNode> previousNode )
{
	WeakPtr<FocusTreeNode> const retVal = context.GetMutableFocusManager().GetMutableFocusTree().CreateNewSiblingAfter( previousNode, mFocusTarget );
	if( retVal != nullptr )
	{
		OnAddedToFocusTree( context, *retVal );
	}
	return retVal;
}



WeakPtr<FocusTreeNode> InstancedController::AddAsSiblingBeforeFocusTreeNode( UIContext& context, WeakPtr<FocusTreeNode> nextNode )
{
	WeakPtr<FocusTreeNode> const retVal = context.GetMutableFocusManager().GetMutableFocusTree().CreateNewSiblingBefore( nextNode, mFocusTarget );
	if( retVal != nullptr )
	{
		OnAddedToFocusTree( context, *retVal );
	}
	return retVal;
}



void InstancedController::OnInstanceAssign( UIContext& context, Container& container )
{
	//
}



void InstancedController::OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode )
{
	//
}



bool InstancedController::OnFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	return false;
}



void InstancedController::OnAssign( UIContext& context, Container& container )
{
	mContainerID = container.mContainerID;

	FocusTarget& focusTarget = *mFocusTarget;
	focusTarget.mContainerID = mContainerID;

	// Enable on initialization by default
	focusTarget.mIsFocusable = true;

	OnInstanceAssign( context, container );
}

///////////////////////////////////////////////////////////////////////////////

FocusTarget const& InstancedController::GetFocusTarget() const
{
	return *mFocusTarget;
}



FocusTarget& InstancedController::GetMutableFocusTarget()
{
	return *mFocusTarget;
}

///////////////////////////////////////////////////////////////////////////////

bool InstancedController::HandleFocusEvent( UIContext& context, void* userData, FocusEvent const& focusEvent )
{
	RF_ASSERT( userData != nullptr );
	return reinterpret_cast<InstancedController*>( userData )->OnFocusEvent( context, focusEvent );
}

///////////////////////////////////////////////////////////////////////////////
}}}

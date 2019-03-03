#include "stdafx.h"
#include "FocusManager.h"

#include "GameUI/FocusTarget.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusKey.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

bool FocusManager::HandleEvent( FocusEvent const& focusEvent ) const
{
	// Walk backwards up the focus stack, looking for someone to handle the
	//  focus event successfully
	FocusTree::ConstNodeStack const currentFocusStack = mFocusTree.GetCurrentFocusStack();
	for(
		FocusTree::ConstNodeStack::const_reverse_iterator iter = currentFocusStack.rbegin();
		iter != currentFocusStack.rend();
		iter++ )
	{
		FocusTreeNode const* const nodeRef = *iter;
		RF_ASSERT( nodeRef != nullptr );
		FocusTreeNode const& node = *nodeRef;
		FocusTarget const* const targetRef = node.mFocusTarget;
		RF_ASSERT( targetRef != nullptr );

		bool const handled = targetRef->HandleEvent( focusEvent );
		if( handled )
		{
			return true;
		}
	}

	return false;
}



FocusTree const& FocusManager::GetFocusTree() const
{
	return mFocusTree;
}



FocusTree& FocusManager::GetMutableFocusTree()
{
	return mFocusTree;
}



void FocusManager::UpdateHardFocus()
{
	WeakPtr<FocusTreeNode const> const currentFocusNode = mFocusTree.GetCurrentFocus();
	WeakPtr<FocusTarget> currentFocusTargetRef = nullptr;
	if( currentFocusNode != nullptr )
	{
		currentFocusTargetRef = currentFocusNode->mFocusTarget;
		RF_ASSERT_MSG( currentFocusTargetRef != nullptr, "Found dead node, should've been trimmed" );
	}

	FocusKey* const oldHardFocus = mHardFocusKey;
	UniquePtr<FocusKey> backingFocusKey = nullptr;
	if( oldHardFocus != nullptr )
	{
		// Focus was previously held
		WeakPtr<FocusTarget> const oldHardFocusTarget = oldHardFocus->mFocusTarget;
		RF_ASSERT( oldHardFocusTarget != nullptr );
		UniquePtr<FocusKey>& oldHardFocusHolder = oldHardFocusTarget->mHardFocusKey;
		RF_ASSERT( oldHardFocusHolder != nullptr );
		RF_ASSERT( oldHardFocusHolder == oldHardFocus );

		if( oldHardFocusTarget == currentFocusTargetRef )
		{
			// Focus hasn't changed
			return;
		}

		// Remove, then notify
		// NOTE: Handler shouldn't try to reference key
		backingFocusKey = rftl::move( oldHardFocusHolder );
		backingFocusKey->mFocusTarget = nullptr;
		oldHardFocusTarget->HandleEvent( focusevent::Notification_FocusLost );
	}
	else
	{
		// Focus was destroyed

		if( currentFocusNode != nullptr )
		{
			// Will need new focus
			backingFocusKey = DefaultCreator<FocusKey>::Create();
			mHardFocusKey = backingFocusKey;
		}
	}
	RF_ASSERT( backingFocusKey != nullptr );
	RF_ASSERT( mHardFocusKey != nullptr );
	RF_ASSERT( mHardFocusKey == backingFocusKey );

	if( currentFocusNode == nullptr )
	{
		// No focus
		backingFocusKey = nullptr;
		RF_ASSERT( mHardFocusKey == nullptr );
		return;
	}
	FocusTarget& currentFocusTarget = *currentFocusTargetRef;

	// Notify, then add
	// NOTE: Handler shouldn't try to reference key
	currentFocusTarget.HandleEvent( focusevent::Notification_FocusGained );
	backingFocusKey->mFocusTarget = currentFocusTargetRef;
	currentFocusTarget.mHardFocusKey = rftl::move( backingFocusKey );
}

///////////////////////////////////////////////////////////////////////////////
}}
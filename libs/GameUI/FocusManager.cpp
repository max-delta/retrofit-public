#include "stdafx.h"
#include "FocusManager.h"

#include "GameUI/FocusTarget.h"


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
		FocusTree::Node const* const nodeRef = *iter;
		RF_ASSERT( nodeRef != nullptr );
		FocusTree::Node const& node = *nodeRef;
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

///////////////////////////////////////////////////////////////////////////////
}}

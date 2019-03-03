#include "stdafx.h"
#include "FocusTree.h"

#include "GameUI/FocusTarget.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

FocusTree::FocusTree()
	: mRootNode( DefaultCreator<FocusTreeNode>::Create() )
	, mRootFocus( DefaultCreator<FocusTarget>::Create() )
{
	FocusTreeNode& rootNode = *mRootNode;
	rootNode.mFocusTarget = mRootFocus;

	FocusTarget& rootFocus = *mRootFocus;
	rootFocus.mEventHandler = OnRootFocusEvent;
	rootFocus.mContainerID = kRootContainerID;
}



FocusTreeNode const& FocusTree::GetRootNode() const
{
	return *mRootNode;
}



FocusTree::ConstNodeStack FocusTree::GetCurrentFocusStack() const
{
	ConstNodeStack retVal;
	WeakPtr<FocusTreeNode const> traverse = mRootNode;
	while( traverse != nullptr )
	{
		FocusTreeNode const& node = *traverse;

		if( IsValidForFocus( node ) )
		{
			retVal.emplace_back( traverse );
		}

		if( ShouldTraverseForFocus( node ) )
		{
			traverse = node.mFavoredChild;
		}
	}

	return retVal;
}



WeakPtr<FocusTreeNode const> FocusTree::GetCurrentFocus() const
{
	WeakPtr<FocusTreeNode const> retVal;
	WeakPtr<FocusTreeNode const> traverse = mRootNode;
	while( traverse != nullptr )
	{
		FocusTreeNode const& node = *traverse;

		if( IsValidForFocus( node ) )
		{
			retVal = traverse;
		}

		if( ShouldTraverseForFocus( node ) )
		{
			traverse = node.mFavoredChild;
		}
	}

	return retVal;
}



bool FocusTree::IsCurrentFocus( FocusTreeNode const& node ) const
{
	FocusTreeNode const* currentFocus = GetCurrentFocus();
	return &node == currentFocus;
}



bool FocusTree::IsCurrentFocus( FocusTarget const& target ) const
{
	FocusTreeNode const* currentFocus = GetCurrentFocus();
	if( currentFocus == nullptr )
	{
		return false;
	}
	FocusTarget const* currentTarget = currentFocus->mFocusTarget;
	RF_ASSERT( currentFocus != nullptr );
	return &target == currentTarget;
}



bool FocusTree::IsValidForFocus( FocusTreeNode const& node )
{
	FocusTarget const* const target = node.mFocusTarget;
	if( target == nullptr )
	{
		return false;
	}

	if( target->mIsFocusable == false )
	{
		return false;
	}

	return true;
}



bool FocusTree::ShouldTraverseForFocus( FocusTreeNode const& node )
{
	FocusTarget const* const target = node.mFocusTarget;
	if( target == nullptr )
	{
		return false;
	}

	if( target->mIsFocusable == false )
	{
		return false;
	}

	if( target->mPropogatesFocus == false )
	{
		return false;
	}

	return true;
}



WeakPtr<FocusTreeNode const> FocusTree::FindNode( FocusTarget const& target ) const
{
	for( BackingNodeStorage::value_type const& nodeRef : mBackingNodeStorage )
	{
		WeakPtr<FocusTreeNode const> possibleReturn = nodeRef;
		FocusTreeNode const* const node = possibleReturn;
		RF_ASSERT( node != nullptr );
		if( node->mFocusTarget == &target )
		{
			return possibleReturn;
		}
	}
	return nullptr;
}



WeakPtr<FocusTreeNode> FocusTree::CreateNewChild( FocusTreeNode const& parentNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( parentNode.mFavoredChild != nullptr )
	{
		RF_DBGFAIL();
		return nullptr;
	}

	FocusTreeNode& node = const_cast<FocusTreeNode&>( parentNode );
	UniquePtr<FocusTreeNode> newNode = DefaultCreator<FocusTreeNode>::Create();
	newNode->mFocusTarget = newTarget;

	node.mFavoredChild = newNode;

	WeakPtr<FocusTreeNode> const retVal = newNode;
	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return retVal;
}



WeakPtr<FocusTreeNode> FocusTree::CreateNewSiblingAfter( WeakPtr<FocusTreeNode> previousNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( previousNode == nullptr )
	{
		RF_DBGFAIL();
		return nullptr;
	}

	FocusTreeNode& node = *previousNode;
	UniquePtr<FocusTreeNode> newNode = DefaultCreator<FocusTreeNode>::Create();
	newNode->mFocusTarget = newTarget;

	// A' - B'' - C
	WeakPtr<FocusTreeNode> next = node.mNextSibling; // C
	node.mNextSibling = newNode; // A -> B
	newNode->mPreviousSibling = previousNode; // A <- B
	if( next != nullptr )
	{
		newNode->mNextSibling = next; // B -> C
		next->mPreviousSibling = newNode; // B <- C
	}

	WeakPtr<FocusTreeNode> const retVal = newNode;
	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return retVal;
}



WeakPtr<FocusTreeNode> FocusTree::CreateNewSiblingBefore( WeakPtr<FocusTreeNode> nextNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( nextNode == nullptr )
	{
		RF_DBGFAIL();
		return nullptr;
	}

	FocusTreeNode& node = *nextNode;
	UniquePtr<FocusTreeNode> newNode = DefaultCreator<FocusTreeNode>::Create();
	newNode->mFocusTarget = newTarget;

	// A - B'' - C'
	WeakPtr<FocusTreeNode> previous = node.mPreviousSibling; // A
	node.mNextSibling = newNode; // B <- C
	newNode->mPreviousSibling = nextNode; // B -> C
	if( previous != nullptr )
	{
		newNode->mPreviousSibling = previous; // A <- B
		previous->mNextSibling = newNode; // A -> B
	}

	WeakPtr<FocusTreeNode> const retVal = newNode;
	mBackingNodeStorage.emplace_back( rftl::move( newNode ) );
	return retVal;
}



void FocusTree::TrimDeadLinks()
{
	// TODO
}

///////////////////////////////////////////////////////////////////////////////

bool FocusTree::OnRootFocusEvent( void* userData, FocusEvent const& focusEvent )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}
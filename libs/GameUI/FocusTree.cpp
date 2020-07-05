#include "stdafx.h"
#include "FocusTree.h"

#include "GameUI/FocusTarget.h"

#include "core/ptr/default_creator.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

ContainerID FocusTreeNode::GetContainerID() const
{
	if( mFocusTarget == nullptr )
	{
		return kInvalidContainerID;
	}

	return mFocusTarget->mContainerID;
}



WeakPtr<FocusTreeNode const> FocusTreeNode::GetFavoredChild() const
{
	return mFavoredChild;
}



WeakPtr<FocusTreeNode> FocusTreeNode::GetMutableFavoredChild()
{
	return mFavoredChild;
}

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



FocusTreeNode& FocusTree::GetMutableRootNode()
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

		if( ShouldTraverseForFocus( node ) == false )
		{
			break;
		}

		traverse = node.mFavoredChild;
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

		if( ShouldTraverseForFocus( node ) == false )
		{
			break;
		}

		traverse = node.mFavoredChild;
	}

	return retVal;
}



ContainerID FocusTree::GetCurrentFocusContainerID() const
{
	WeakPtr<FocusTreeNode const> const currentFocus = GetCurrentFocus();
	if( currentFocus == nullptr )
	{
		return kInvalidContainerID;
	}

	WeakPtr<ui::FocusTarget const> const currentFocusTarget = currentFocus->mFocusTarget;
	if( currentFocusTarget == nullptr )
	{
		return kInvalidContainerID;
	}

	RF_ASSERT( currentFocusTarget->HasHardFocus() );
	return currentFocusTarget->mContainerID;
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



bool FocusTree::IsCurrentFocus( ContainerID const& containerID ) const
{
	RF_ASSERT( containerID != kInvalidContainerID );
	return GetCurrentFocusContainerID() == containerID;
}



bool FocusTree::IsInCurrentFocusStack( FocusTreeNode const& node ) const
{
	RF_ASSERT( node.mFocusTarget != nullptr );
	ConstNodeStack const stack = GetCurrentFocusStack();
	for( WeakPtr<FocusTreeNode const> const& elem : stack )
	{
		RF_ASSERT( elem != nullptr );
		if( elem == &node )
		{
			return true;
		}
	}
	return false;
}



bool FocusTree::IsInCurrentFocusStack( FocusTarget const& target ) const
{
	ConstNodeStack const stack = GetCurrentFocusStack();
	for( WeakPtr<FocusTreeNode const> const& elem : stack )
	{
		RF_ASSERT( elem != nullptr );
		RF_ASSERT( elem->mFocusTarget != nullptr );
		if( elem->mFocusTarget == &target )
		{
			return true;
		}
	}
	return false;
}



bool FocusTree::IsInCurrentFocusStack( ContainerID const& containerID ) const
{
	ConstNodeStack const stack = GetCurrentFocusStack();
	for( WeakPtr<FocusTreeNode const> const& elem : stack )
	{
		RF_ASSERT( elem != nullptr );
		RF_ASSERT( elem->mFocusTarget != nullptr );
		if( elem->mFocusTarget->mContainerID == containerID )
		{
			return true;
		}
	}
	return false;
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



WeakPtr<FocusTreeNode> FocusTree::CreateNewChild( FocusTreeNode& parentNode, UniquePtr<FocusTarget> const& newTarget )
{
	if( parentNode.mFavoredChild != nullptr )
	{
		RF_DBGFAIL_MSG( "Cannot create new focus node as child, a child already exists here" );
		return nullptr;
	}

	FocusTreeNode& node = const_cast<FocusTreeNode&>( parentNode );
	UniquePtr<FocusTreeNode> newNode = DefaultCreator<FocusTreeNode>::Create();
	newNode->mFocusTarget = newTarget;
	newNode->mFocusTarget->mFocusTreeNode = newNode;

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
	newNode->mFocusTarget->mFocusTreeNode = newNode;

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
	newNode->mFocusTarget->mFocusTreeNode = newNode;

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



bool FocusTree::HasImmediateChild( FocusTreeNode const& parentNode, WeakPtr<FocusTreeNode const> queryNode ) const
{
	if( queryNode == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	// Current
	if( parentNode.mFavoredChild == queryNode )
	{
		return true;
	}

	// Before
	WeakPtr<FocusTreeNode const> reverse = parentNode.mFavoredChild->mPreviousSibling;
	while( reverse != nullptr )
	{
		if( reverse == queryNode )
		{
			return true;
		}
		reverse = reverse->mPreviousSibling;
	}

	// After
	WeakPtr<FocusTreeNode const> forward = parentNode.mFavoredChild->mNextSibling;
	while( forward != nullptr )
	{
		if( forward == queryNode )
		{
			return true;
		}
		forward = forward->mNextSibling;
	}

	return false;
}



bool FocusTree::CycleRootFocusToNextChild( bool wrapIfLastChild )
{
	return CycleFocusToNextChild( *mRootNode, wrapIfLastChild );
}



bool FocusTree::CycleRootFocusToPreviousChild( bool wrapIfLastChild )
{
	return CycleFocusToPreviousChild( *mRootNode, wrapIfLastChild );
}



bool FocusTree::SetRootFocusToSpecificChild( WeakPtr<FocusTreeNode> targetNode )
{
	return SetFocusToSpecificChild( *mRootNode, targetNode );
}



bool FocusTree::CycleFocusToNextChild( FocusTreeNode& parentNode, bool wrapIfLastChild )
{
	if( parentNode.mFavoredChild == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	// Try next
	WeakPtr<FocusTreeNode> const& next = parentNode.mFavoredChild->mNextSibling;
	if( next != nullptr )
	{
		parentNode.mFavoredChild = next;
		return true;
	}

	if( wrapIfLastChild == false )
	{
		return false;
	}

	WeakPtr<FocusTreeNode> reverse = parentNode.mFavoredChild->mPreviousSibling;
	if( reverse == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	// Walk to first
	while( reverse != nullptr )
	{
		parentNode.mFavoredChild = reverse;
		reverse = reverse->mPreviousSibling;
	}
	return true;
}



bool FocusTree::CycleFocusToPreviousChild( FocusTreeNode& parentNode, bool wrapIfLastChild )
{
	if( parentNode.mFavoredChild == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	// Try previous
	WeakPtr<FocusTreeNode> const& previous = parentNode.mFavoredChild->mPreviousSibling;
	if( previous != nullptr )
	{
		parentNode.mFavoredChild = previous;
		return true;
	}

	if( wrapIfLastChild == false )
	{
		return false;
	}

	WeakPtr<FocusTreeNode> reverse = parentNode.mFavoredChild->mNextSibling;
	if( reverse == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	// Walk to last
	while( reverse != nullptr )
	{
		parentNode.mFavoredChild = reverse;
		reverse = reverse->mNextSibling;
	}
	return true;
}



bool FocusTree::CycleFocusToFirstChild( FocusTreeNode& parentNode )
{
	bool anyChange = false;
	while( CycleFocusToPreviousChild( parentNode, false ) )
	{
		anyChange = true;
	}
	return anyChange;
}



bool FocusTree::CycleFocusToLastChild( FocusTreeNode& parentNode )
{
	bool anyChange = false;
	while( CycleFocusToNextChild( parentNode, false ) )
	{
		anyChange = true;
	}
	return anyChange;
}



bool FocusTree::SetFocusToSpecificChild( FocusTreeNode& parentNode, WeakPtr<FocusTreeNode> targetNode )
{
	if( parentNode.mFavoredChild == targetNode )
	{
		// No change
		return false;
	}

	if( HasImmediateChild( parentNode, targetNode ) == false )
	{
		RF_DBGFAIL();
		return false;
	}

	parentNode.mFavoredChild = targetNode;
	return true;
}



void FocusTree::TrimDeadLinks()
{
	rftl::unordered_set<FocusTreeNode const*> liveNodes;

	// Root ALWAYS has focus
	RF_ASSERT( GetRootNode().mFocusTarget != nullptr );

	// Visit all nodes, starting with root
	rftl::unordered_set<FocusTreeNode*> nodesToVisit;
	nodesToVisit.emplace( mRootNode );
	while( nodesToVisit.empty() == false )
	{
		// Pop from unvisited list
		FocusTreeNode* const currentNodeRef = *nodesToVisit.begin();
		nodesToVisit.erase( nodesToVisit.begin() );
		RF_ASSERT( currentNodeRef != nullptr );

		// Add node to live list
		RF_ASSERT( currentNodeRef->mFocusTarget != nullptr );
		RF_ASSERT( liveNodes.count( currentNodeRef ) == 0 );
		liveNodes.emplace( currentNodeRef );

		static constexpr auto onChainNode = []( WeakPtr<FocusTreeNode>& chainNode, rftl::unordered_set<FocusTreeNode*>& nodesToVisit ) -> void //
		{
			FocusTreeNode const* ptr = chainNode;
			if( ptr != nullptr )
			{
				FocusTreeNode const& val = *ptr;
				if( val.mFocusTarget == nullptr )
				{
					// Dead, trim
					chainNode = nullptr;
				}
				else
				{
					// Alive, add
					nodesToVisit.emplace( chainNode );
				}
			}
		};

		FocusTreeNode& currentNode = *currentNodeRef;
		onChainNode( currentNode.mPreviousSibling, nodesToVisit );
		onChainNode( currentNode.mNextSibling, nodesToVisit );
		onChainNode( currentNode.mFavoredChild, nodesToVisit );
	}

	// Wipe out all orphaned nodes
	BackingNodeStorage::const_iterator iter = mBackingNodeStorage.begin();
	while( iter != mBackingNodeStorage.end() )
	{
		if( liveNodes.count( *iter ) > 0 )
		{
			// Still in use
			iter++;
		}
		else
		{
			// Orphaned
			iter = mBackingNodeStorage.erase( iter );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

bool FocusTree::OnRootFocusEvent( UIContext& context, void* userData, FocusEvent const& focusEvent )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}

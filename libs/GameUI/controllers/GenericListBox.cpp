#include "stdafx.h"
#include "GenericListBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/FocusManager.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::ui::controller::GenericListBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::GenericListBox );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

GenericListBox::GenericListBox(
	size_t numSlots )
	: GenericListBox(
		  Orientation::Vertical,
		  numSlots )
{
	//
}



GenericListBox::GenericListBox(
	Orientation orientation,
	size_t numSlots )
	: mOrientation( orientation )
	, mNumSlots( numSlots )
{
	RF_ASSERT( mNumSlots >= 2 );
}



ContainerID GenericListBox::GetChildContainerID() const
{
	return mChildContainerID;
}



size_t GenericListBox::GetNumSlots() const
{
	return mNumSlots;
}



WeakPtr<InstancedController const> GenericListBox::GetSlotController( size_t slotIndex ) const
{
	WeakPtr<InstancedController const> const retVal = mSlotControllers.at( slotIndex );
	RF_ASSERT_MSG( retVal != nullptr, "All slot controllers should've been assigned" );
	return retVal;
}



WeakPtr<InstancedController> GenericListBox::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<InstancedController> const retVal = mSlotControllers.at( slotIndex );
	RF_ASSERT_MSG( retVal != nullptr, "All slot controllers should've been assigned" );
	return retVal;
}



void GenericListBox::SetWrapping( bool wrapping )
{
	mWrapping = wrapping;
}



void GenericListBox::SetPagination( bool pagination )
{
	mPagination = pagination;
}



bool GenericListBox::SlotHasCurrentFocus( UIConstContext const& context ) const
{
	InstancedController const* const slotWithFocus = GetSlotWithFocus( context );
	return slotWithFocus != nullptr;
}



size_t GenericListBox::GetSlotIndexWithSoftFocus( UIConstContext const& context ) const
{
	WeakPtr<ui::FocusTreeNode const> const listTreeNode = GetFocusTreeNode( context );
	RF_ASSERT( listTreeNode != nullptr );
	WeakPtr<ui::FocusTreeNode const> const currentSoftFocus = listTreeNode->GetFavoredChild();
	RF_ASSERT( currentSoftFocus != nullptr );

	for( size_t i = 0; i < mSlotControllers.size(); i++ )
	{
		WeakPtr<InstancedController const> const& slotController = mSlotControllers.at( i );
		RF_ASSERT( slotController != nullptr );
		WeakPtr<ui::FocusTreeNode const> const slotNode = slotController->GetFocusTreeNode( context );
		RF_ASSERT( slotNode != nullptr );
		if( slotNode == currentSoftFocus )
		{
			return i;
		}
	}

	RF_DBGFAIL_MSG( "Focus corrupt, or not set up yet" );
	return mSlotControllers.size();
}



bool GenericListBox::SetSlotIndexWithSoftFocus( UIContext& context, size_t index )
{
	WeakPtr<ui::FocusTreeNode> const listTreeNode = GetMutableFocusTreeNode( context );
	RF_ASSERT( listTreeNode != nullptr );

	RF_ASSERT( index < mSlotControllers.size() );
	WeakPtr<InstancedController> const& desiredController = mSlotControllers.at( index );
	RF_ASSERT( desiredController != nullptr );
	if( ShouldSkipFocus( context, desiredController ) )
	{
		return false;
	}
	WeakPtr<ui::FocusTreeNode> const desiredFocus = desiredController->GetMutableFocusTreeNode( context );
	RF_ASSERT( desiredFocus != nullptr );

	FocusManager& focusMan = context.GetMutableFocusManager();
	focusMan.GetMutableFocusTree().SetFocusToSpecificChild( *listTreeNode, desiredFocus );
	return true;
}



void GenericListBox::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	mSlicerChildContainerIDs.resize( mNumSlots );
	if( mOrientation == Orientation::Vertical )
	{
		WeakPtr<RowSlicer> const slicer =
			context.GetMutableContainerManager().AssignStrongController(
				mChildContainerID,
				DefaultCreator<RowSlicer>::Create(
					mNumSlots ) );
		for( size_t i = 0; i < mNumSlots; i++ )
		{
			ContainerID const slicerChildContainerID = slicer->GetChildContainerID( i );
			RF_ASSERT( slicerChildContainerID != kInvalidContainerID );
			mSlicerChildContainerIDs.at( i ) = slicerChildContainerID;
		}
	}
	else
	{
		WeakPtr<ColumnSlicer> const slicer =
			context.GetMutableContainerManager().AssignStrongController(
				mChildContainerID,
				DefaultCreator<ColumnSlicer>::Create(
					mNumSlots ) );
		for( size_t i = 0; i < mNumSlots; i++ )
		{
			ContainerID const slicerChildContainerID = slicer->GetChildContainerID( i );
			RF_ASSERT( slicerChildContainerID != kInvalidContainerID );
			mSlicerChildContainerIDs.at( i ) = slicerChildContainerID;
		}
	}

	RF_ASSERT( mSlotControllers.empty() );
	mSlotControllers.resize( mNumSlots );
	PostInstanceAssign( context, container );
}



void GenericListBox::OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode )
{
	// Add all the slots to focus tree
	WeakPtr<FocusTreeNode> mostRecentNode = newNode.GetMutableFavoredChild();
	for( WeakPtr<InstancedController> const& slotController : mSlotControllers )
	{
		RF_ASSERT( slotController != nullptr );
		if( mostRecentNode == nullptr )
		{
			mostRecentNode = slotController->AddAsChildToFocusTreeNode( context, newNode );
		}
		else
		{
			mostRecentNode = slotController->AddAsSiblingAfterFocusTreeNode( context, mostRecentNode );
		}
	}
}



bool GenericListBox::OnFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	bool isPrevious;
	bool isNext;
	if( mOrientation == Orientation::Vertical )
	{
		isPrevious = focusEvent.mEventType == focusevent::Command_NavigateUp;
		isNext = focusEvent.mEventType == focusevent::Command_NavigateDown;
	}
	else
	{
		isPrevious = focusEvent.mEventType == focusevent::Command_NavigateLeft;
		isNext = focusEvent.mEventType == focusevent::Command_NavigateRight;
	}
	bool const isPageUp = mPagination && focusEvent.mEventType == focusevent::Command_NavigateToPreviousGroup;
	bool const isPageDown = mPagination && focusEvent.mEventType == focusevent::Command_NavigateToNextGroup;
	bool const isFirst = focusEvent.mEventType == focusevent::Command_NavigateToFirst;
	bool const isLast = focusEvent.mEventType == focusevent::Command_NavigateToLast;
	bool const isIncremental = isPrevious || isNext || isPageUp || isPageDown;
	bool const isTargeted = isFirst || isLast;
	bool const isCycle = isIncremental || isTargeted;

	if( isCycle )
	{
		// Do any of our slots have direct focus?
		InstancedController const* const slotWithFocus = GetSlotWithFocus( context );
		bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

		if( listBoxHasImplicitFocus == false )
		{
			return false;
		}

		WeakPtr<FocusTreeNode> const nodeRef = GetMutableFocusTreeNode( context );
		RF_ASSERT_MSG( nodeRef != nullptr, "Handling focus event without being in tree?" );
		FocusTreeNode& node = *nodeRef;

		void const* const initial = node.GetFavoredChild();
		RF_ASSERT( initial != nullptr );

		FocusTree& focusTree = context.GetMutableFocusManager().GetMutableFocusTree();
		static constexpr size_t kMaxIter = 500;

		if( isPrevious )
		{
			// Pass 1, probe forward
			for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
			{
				bool const modified = focusTree.CycleFocusToPreviousChild( node, mWrapping );
				if( modified == false )
				{
					// At end, or error
					break;
				}

				bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
				if( shouldSkip == false )
				{
					// Found a valid item
					break;
				}
			}

			// Pass 2, back up if we reached end and it was bad
			if( mWrapping == false )
			{
				for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
				{
					bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
					if( shouldSkip )
					{
						bool const modified = focusTree.CycleFocusToNextChild( node, mWrapping );
						if( modified == false )
						{
							// Error
							break;
						}
						else
						{
							// Iterate
							continue;
						}
					}
					else
					{
						// Valid
						break;
					}
				}
			}
		}
		else if( isNext )
		{
			// Pass 1, probe forward
			for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
			{
				bool const modified = focusTree.CycleFocusToNextChild( node, mWrapping );
				if( modified == false )
				{
					// At end, or error
					break;
				}

				bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
				if( shouldSkip == false )
				{
					// Found a valid item
					break;
				}
			}

			// Pass 2, back up if we reached end and it was bad
			if( mWrapping == false )
			{
				for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
				{
					bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
					if( shouldSkip )
					{
						bool const modified = focusTree.CycleFocusToPreviousChild( node, mWrapping );
						if( modified == false )
						{
							// Error
							break;
						}
						else
						{
							// Iterate
							continue;
						}
					}
					else
					{
						// Valid
						break;
					}
				}
			}
		}
		else if( isFirst || isPageUp )
		{
			// Pass 1, snap forward
			focusTree.CycleFocusToFirstChild( node );

			// Pass 2, back up if we reached end and it was bad
			for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
			{
				bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
				if( shouldSkip )
				{
					bool const modified = focusTree.CycleFocusToNextChild( node, mWrapping );
					if( modified == false )
					{
						// Error
						break;
					}
					else
					{
						// Iterate
						continue;
					}
				}
				else
				{
					// Valid
					break;
				}
			}
		}
		else if( isLast || isPageDown )
		{
			// Pass 1, snap forward
			focusTree.CycleFocusToLastChild( node );

			// Pass 2, back up if we reached end and it was bad
			for( size_t infinitePrevention = 0; infinitePrevention < kMaxIter; infinitePrevention++ )
			{
				bool const shouldSkip = ShouldSkipFocus( context, *node.GetFavoredChild() );
				if( shouldSkip )
				{
					bool const modified = focusTree.CycleFocusToPreviousChild( node, mWrapping );
					if( modified == false )
					{
						// Error
						break;
					}
					else
					{
						// Iterate
						continue;
					}
				}
				else
				{
					// Valid
					break;
				}
			}
		}

		if( node.GetFavoredChild() != initial )
		{
			// Event resulted in a change
			return true;
		}
		else if( isIncremental && mWrapping )
		{
			// Sink
			// TODO: Configurable?
			return false;
		}
		else
		{
			// Fallthrough
		}
	}

	return OnUnhandledFocusEvent( context, focusEvent );
}

///////////////////////////////////////////////////////////////////////////////

void GenericListBox::AssignSlotControllerInternal( UIContext& context, size_t slotIndex, UniquePtr<InstancedController>&& controller )
{
	RF_ASSERT( controller != nullptr );

	WeakPtr<InstancedController>& slot = mSlotControllers.at( slotIndex );
	RF_ASSERT( slot == nullptr );

	ContainerID const slicerChildContainerID = mSlicerChildContainerIDs.at( slotIndex );
	RF_ASSERT( slicerChildContainerID != kInvalidContainerID );

	slot = context.GetMutableContainerManager().AssignStrongController(
		slicerChildContainerID,
		rftl::move( controller ) );
}



InstancedController const* GenericListBox::GetSlotWithFocus( UIConstContext const& context ) const
{
	for( WeakPtr<InstancedController> const& slotController : mSlotControllers )
	{
		RF_ASSERT( slotController != nullptr );
		if( slotController->IsCurrentFocus( context ) )
		{
			return slotController;
		}
	}
	return nullptr;
}



bool GenericListBox::ShouldSkipFocus( UIConstContext const& context, FocusTreeNode const& potentialFocus ) const
{
	ContainerID const containerID = potentialFocus.GetContainerID();
	RF_ASSERT( containerID != kInvalidContainerID );

	WeakPtr<InstancedController const> attemptedFocus;
	for( WeakPtr<InstancedController const> const slotController : mSlotControllers )
	{
		if( slotController->GetContainerID() == containerID )
		{
			attemptedFocus = slotController;
			break;
		}
	}
	RF_ASSERT( attemptedFocus != nullptr );

	return ShouldSkipFocus( context, attemptedFocus );
}



void GenericListBox::PostInstanceAssign( UIContext& context, Container& container )
{
	//
}



bool GenericListBox::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	return false;
}



bool GenericListBox::OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}}

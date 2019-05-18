#include "stdafx.h"
#include "ListBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/FocusManager.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::ui::controller::ListBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::ListBox );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

ListBox::ListBox( size_t numSlots, FontPurposeID purpose )
	: ListBox(
		numSlots,
		purpose,
		Justification::MiddleCenter,
		math::Color3f::kGray50,
		math::Color3f::kWhite,
		math::Color3f::kYellow )
{
	//
}



ListBox::ListBox(
	size_t numSlots,
	FontPurposeID purpose,
	Justification justification,
	math::Color3f unfocusedColor,
	math::Color3f unselectedColor,
	math::Color3f selectedColor )
	: mNumSlots( numSlots )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mUnfocusedColor( unfocusedColor )
	, mUnselectedColor( unselectedColor )
	, mSelectedColor( selectedColor )
{
	RF_ASSERT( mNumSlots >= 2 );
}



ContainerID ListBox::GetChildContainerID() const
{
	return mChildContainerID;
}



WeakPtr<TextLabel> ListBox::GetSlotController( size_t slotIndex )
{
	return mSlotControllers.at( slotIndex );
}



void ListBox::SetText( rftl::vector<rftl::string> const& text )
{
	RF_ASSERT( text.size() == mNumSlots );
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		GetSlotController( i )->SetText( text.at( i ).c_str() );
	}
}



void ListBox::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	WeakPtr<ui::controller::RowSlicer> const rowSlicer =
		context.GetMutableContainerManager().AssignStrongController(
			mChildContainerID,
			DefaultCreator<ui::controller::RowSlicer>::Create(
				mNumSlots ) );

	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<ui::controller::TextLabel> const slotController =
			context.GetMutableContainerManager().AssignStrongController(
				rowSlicer->GetChildContainerID( i ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		slotController->SetJustification( mJustification );
		slotController->SetFont( mFontPurpose );
		slotController->SetText( "<UNSET>" );
		slotController->SetColor( mUnfocusedColor );
		slotController->SetBorder( true );
		mSlotControllers.emplace_back( slotController );
	}
}



void ListBox::OnAddedToFocusTree( UIContext& context, FocusTreeNode const& newNode )
{
	// Add all the slots to focus tree
	WeakPtr<FocusTreeNode> mostRecentNode = newNode.mFavoredChild;
	for( WeakPtr<TextLabel> const& slotController : mSlotControllers )
	{
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



bool ListBox::OnFocusEvent( UIContext& context, FocusEvent const& focusEvent )
{
	bool const isPrevious = focusEvent.mEventType == focusevent::Command_NavigateUp;
	bool const isNext = focusEvent.mEventType == focusevent::Command_NavigateDown;
	bool const isFirst = focusEvent.mEventType == focusevent::Command_NavigateToFirst;
	bool const isLast = focusEvent.mEventType == focusevent::Command_NavigateToLast;
	bool const isCycle = isPrevious || isNext || isFirst || isLast;

	if( isCycle )
	{
		// Do any of our slots have direct focus?
		TextLabel const* const slotWithFocus = GetSlotWithFocus( context );
		bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

		if( listBoxHasImplicitFocus == false )
		{
			return false;
		}

		WeakPtr<FocusTreeNode> const nodeRef = GetMutableFocusTreeNode( context );
		RF_ASSERT_MSG( nodeRef != nullptr, "Handling focus event without being in tree?" );
		FocusTreeNode& node = *nodeRef;

		RF_ASSERT( node.mFavoredChild != nullptr );
		FocusTreeNode& current = *node.mFavoredChild;

		if( isPrevious )
		{
			WeakPtr<FocusTreeNode> previous = current.mPreviousSibling;
			while( previous != nullptr && ShouldSkipFocus( context, *previous ) )
			{
				previous = previous->mPreviousSibling;
			}

			if( previous != nullptr )
			{
				node.mFavoredChild = previous;
			}
			else
			{
				// Already at start of list, allow something else to handle
				return false;
			}
		}
		else if( isNext )
		{
			WeakPtr<FocusTreeNode> next = current.mNextSibling;
			while( next != nullptr && ShouldSkipFocus( context, *next ) )
			{
				next = next->mNextSibling;
			}

			if( next != nullptr )
			{
				node.mFavoredChild = next;
			}
			else
			{
				// Already at end of list, allow something else to handle
				return false;
			}
		}
		else if( isFirst )
		{
			WeakPtr<FocusTreeNode> previous = current.mPreviousSibling;
			while( previous != nullptr )
			{
				while( previous != nullptr && ShouldSkipFocus( context, *previous ) )
				{
					previous = previous->mPreviousSibling;
				}

				if( previous != nullptr )
				{
					node.mFavoredChild = previous;
					previous = previous->mPreviousSibling;
				}
			}
		}
		else if( isLast )
		{
			WeakPtr<FocusTreeNode> next = current.mNextSibling;
			while( next != nullptr )
			{
				while( next != nullptr && ShouldSkipFocus( context, *next ) )
				{
					next = next->mNextSibling;
				}

				if( next != nullptr )
				{
					node.mFavoredChild = next;
					next = next->mNextSibling;
				}
			}
		}

		return true;
	}

	return false;
}



void ListBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Do any of our slots have direct focus?
	TextLabel const* const slotWithFocus = GetSlotWithFocus( context );
	bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

	// Update colors
	for( WeakPtr<TextLabel> const& slotController : mSlotControllers )
	{
		if( listBoxHasImplicitFocus )
		{
			if( slotController == slotWithFocus )
			{
				slotController->SetColor( mSelectedColor );
			}
			else
			{
				slotController->SetColor( mUnselectedColor );
			}
		}
		else
		{
			slotController->SetColor( mUnfocusedColor );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

TextLabel const* ListBox::GetSlotWithFocus( UIConstContext const& context ) const
{
	for( WeakPtr<TextLabel> const& slotController : mSlotControllers )
	{
		if( slotController->IsCurrentFocus( context ) )
		{
			return slotController;
		}
	}
	return nullptr;
}



bool ListBox::ShouldSkipFocus( UIConstContext const& context, FocusTreeNode const& potentialFocus ) const
{
	RF_ASSERT( potentialFocus.mFocusTarget != nullptr );
	ContainerID const containerID = potentialFocus.mFocusTarget->mContainerID;
	RF_ASSERT( containerID != kInvalidContainerID );

	WeakPtr<TextLabel const> attemptedFocus;
	for( WeakPtr<TextLabel const> const& slotController : mSlotControllers )
	{
		if( slotController->GetContainerID() == containerID )
		{
			attemptedFocus = slotController;
			break;
		}
	}
	RF_ASSERT( attemptedFocus != nullptr );

	if( attemptedFocus->HasText() == false )
	{
		// No text, skip
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}}

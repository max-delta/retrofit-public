#include "stdafx.h"
#include "ListBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
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



bool ListBox::OnFocusEvent( FocusEvent const& focusEvent )
{
	// TODO
	return false;
}



void ListBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Do any of our slots have direct focus?
	TextLabel const* slotWithFocus = nullptr;
	for( WeakPtr<TextLabel> const& slotController : mSlotControllers )
	{
		if( slotController->IsCurrentFocus( context ) )
		{
			slotWithFocus = slotController;
			break;
		}
	}
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
}}}
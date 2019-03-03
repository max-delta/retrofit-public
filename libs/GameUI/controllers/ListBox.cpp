#include "stdafx.h"
#include "ListBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::ui::controller::ListBox )
{
	RFTYPE_META().BaseClass<RF::ui::Controller>();
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
	, mFocusTarget( DefaultCreator<FocusTarget>::Create() )
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



void ListBox::OnAssign( UIContext& context, Container& container )
{
	FocusTarget& focusTarget = *mFocusTarget;
	focusTarget.mUserData = this;
	focusTarget.mEventHandler = HandleFocusEvent;
	focusTarget.mContainerID = container.mContainerID;

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



bool ListBox::OnFocusEvent( FocusEvent const& focusEvent )
{
	// TODO
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool ListBox::HandleFocusEvent( void* userData, FocusEvent const& focusEvent )
{
	RF_ASSERT( userData != nullptr );
	return reinterpret_cast<ListBox*>( userData )->OnFocusEvent( focusEvent );
}

///////////////////////////////////////////////////////////////////////////////
}}}

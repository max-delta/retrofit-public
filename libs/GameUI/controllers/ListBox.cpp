#include "stdafx.h"
#include "ListBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/FocusManager.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"

#include "rftl/functional"


RFTYPE_CREATE_META( RF::ui::controller::ListBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::GenericListBox>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::ListBox );
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

ListBox::ListBox(
	size_t numSlots,
	FontPurposeID purpose )
	: ListBox(
		  Orientation::Vertical,
		  numSlots,
		  purpose )
{
	//
}



ListBox::ListBox(
	Orientation orientation,
	size_t numSlots,
	FontPurposeID purpose )
	: ListBox(
		  orientation,
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
	Orientation orientation,
	size_t numSlots,
	FontPurposeID purpose,
	Justification justification,
	math::Color3f unfocusedColor,
	math::Color3f unselectedColor,
	math::Color3f selectedColor )
	: GenericListBox( orientation, numSlots )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mUnfocusedColor( unfocusedColor )
	, mUnselectedColor( unselectedColor )
	, mSelectedColor( selectedColor )
{
	//
}



WeakPtr<TextLabel> ListBox::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<TextLabel> textLabel;
	PtrTransformer<TextLabel>::PerformNonTypesafeTransformation(
		GenericListBox::GetMutableSlotController( slotIndex ), textLabel );
	return textLabel;
}



void ListBox::SetText( rftl::vector<rftl::string> const& text )
{
	RF_ASSERT( text.size() == mNumSlots );
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		GetMutableSlotController( i )->SetText( text.at( i ).c_str() );
	}
}



void ListBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Do any of our slots have direct focus?
	InstancedController const* const slotWithFocus = GetSlotWithFocus( context );
	bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

	// Update colors
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<TextLabel> const slotController = GetMutableSlotController( i );

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

void ListBox::PostInstanceAssign( UIContext& context, Container& container )
{
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<TextLabel> const controller = AssignSlotController<TextLabel>( context, i, DefaultCreator<TextLabel>::Create() );
		controller->SetJustification( mJustification );
		controller->SetFont( mFontPurpose );
		controller->SetText( "<UNSET>" );
		controller->SetColor( mUnfocusedColor );
		controller->SetBorder( true );
	}
}



bool ListBox::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	WeakPtr<TextLabel const> textLabel;
	PtrTransformer<TextLabel>::PerformNonTypesafeTransformation( attemptedFocus, textLabel );
	if( textLabel->HasText() == false )
	{
		// No text, skip
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

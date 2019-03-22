#include "stdafx.h"
#include "TextRows.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::ui::controller::TextRows )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::TextRows );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

TextRows::TextRows( size_t numSlots, FontPurposeID purpose )
	: TextRows(
		numSlots,
		purpose,
		Justification::MiddleCenter,
		math::Color3f::kWhite )
{
	//
}



TextRows::TextRows(
	size_t numSlots,
	FontPurposeID purpose,
	Justification justification,
	math::Color3f color )
	: mNumSlots( numSlots )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
{
	RF_ASSERT( mNumSlots >= 2 );
}



ContainerID TextRows::GetChildContainerID() const
{
	return mChildContainerID;
}



WeakPtr<TextLabel> TextRows::GetSlotController( size_t slotIndex )
{
	return mSlotControllers.at( slotIndex );
}



void TextRows::SetText( rftl::vector<rftl::string> const& text )
{
	RF_ASSERT( text.size() == mNumSlots );
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		GetSlotController( i )->SetText( text.at( i ).c_str() );
	}
}



void TextRows::OnInstanceAssign( UIContext& context, Container& container )
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
		slotController->SetColor( mColor );
		slotController->SetBorder( true );
		mSlotControllers.emplace_back( slotController );
	}
}



void TextRows::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Update colors
	for( WeakPtr<TextLabel> const& slotController : mSlotControllers )
	{
		slotController->SetColor( mColor );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

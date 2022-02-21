#include "stdafx.h"
#include "OptionSlot.h"

#include "cc3o3/options/Option.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::OptionSlot )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::OptionSlot );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

bool OptionSlot::HasOption() const
{
	return mColumnSlicer->IsChildRenderingBlocked() == false;
}



void OptionSlot::ClearOption()
{
	mColumnSlicer->SetChildRenderingBlocked( true );
}



void OptionSlot::UpdateOption( options::Option const& option, options::OptionValue const& value )
{
	mColumnSlicer->SetChildRenderingBlocked( false );

	mNameLabel->SetText( option.mDisplayName );
	mValueLabel->SetText( "TODO" );
}



void OptionSlot::UpdateState( bool active, bool selected )
{
	if( active == false )
	{
		// Inactive
		mNameLabel->SetColor( math::Color3f::kGray50 );
		mValueLabel->SetColor( math::Color3f::kGray50 );
	}
	else if( selected )
	{
		// Selected
		mNameLabel->SetColor( math::Color3f::kYellow );
		mValueLabel->SetColor( math::Color3f::kYellow );
	}
	else
	{
		// Unselected
		mNameLabel->SetColor( math::Color3f::kWhite );
		mValueLabel->SetColor( math::Color3f::kWhite );
	}
}



void OptionSlot::OnInstanceAssign( UIContext& context, Container& container )
{
	ui::ContainerManager& uiManager = context.GetMutableContainerManager();

	mChildContainerID = CreateChildContainer(
		uiManager,
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	// Display split between name and value
	ui::controller::ColumnSlicer::Ratios const columnRatios = {
		{ 1.f / 2.f, true },
		{ 1.f / 2.f, true },
	};
	mColumnSlicer =
		uiManager.AssignStrongController(
			mChildContainerID,
			DefaultCreator<ColumnSlicer>::Create(
				columnRatios ) );
	mColumnSlicer->SetChildRenderingBlocked( true );

	// Name
	mNameLabel =
		uiManager.AssignStrongController(
			mColumnSlicer->GetChildContainerID( 0 ),
			DefaultCreator<TextLabel>::Create() );
	mNameLabel->SetJustification( ui::Justification::MiddleLeft );
	mNameLabel->SetFont( ui::font::SmallMenuSelection );
	mNameLabel->SetText( "UNSET" );
	mNameLabel->SetColor( math::Color3f::kWhite );
	mNameLabel->SetBorder( true );

	// Value
	mValueLabel =
		uiManager.AssignStrongController(
			mColumnSlicer->GetChildContainerID( 1 ),
			DefaultCreator<TextLabel>::Create() );
	mValueLabel->SetJustification( ui::Justification::MiddleCenter );
	mValueLabel->SetFont( ui::font::SmallMenuSelection );
	mValueLabel->SetText( "UNSET" );
	mValueLabel->SetColor( math::Color3f::kWhite );
	mValueLabel->SetBorder( true );
}

///////////////////////////////////////////////////////////////////////////////
}

#include "stdafx.h"
#include "OptionSlot.h"

#include "cc3o3/options/Option.h"
#include "cc3o3/options/OptionLogic.h"
#include "cc3o3/options/OptionValue.h"

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
	options::OptionDesc const& desc = option.mDesc;

	mColumnSlicer->SetChildRenderingBlocked( false );

	mNameLabel->SetText( option.mDisplayName );

	rftl::optional<rftl::string_view> valueText;

	// NOTE: Can fall-through to other displays
	if( desc.mAction.has_value() )
	{
		// Action
		valueText = "";
	}

	if( desc.mList.has_value() )
	{
		// List
		if( value.mList.has_value() == false )
		{
			RF_DBGFAIL_MSG( "Invalid value" );
			valueText = "!!!INVALID!!!";
		}
		else
		{
			// Find the current item
			size_t index;
			bool const foundCurrent = options::OptionLogic::FindListIdentifier( index, desc, value.mList->mCurrent );
			if( foundCurrent )
			{
				valueText = desc.mList->mItems.at( index ).mDisplayName;
			}
			else
			{
				RF_DBGFAIL_MSG( "Missing entry" );
				valueText = "!!!MISSING!!!";
			}
		}
	}

	if( valueText.has_value() == false )
	{
		RF_DBGFAIL_MSG( "Unknown option type" );
		valueText = "!!!UKNOWN!!!";
	}
	mValueLabel->SetText( rftl::string( valueText.value() ) );
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

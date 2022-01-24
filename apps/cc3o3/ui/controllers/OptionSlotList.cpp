#include "stdafx.h"
#include "OptionSlotList.h"

#include "cc3o3/ui/controllers/OptionSlot.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::OptionSlotList )
{
	RFTYPE_META().BaseClass<RF::ui::controller::GenericListBox>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::OptionSlotList );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

OptionSlotList::OptionSlotList( size_t numSlots )
	: GenericListBox(
		  Orientation::Vertical,
		  numSlots )
{
	//
}



bool OptionSlotList::HasOption( size_t slotIndex ) const
{
	return GetSlotController( slotIndex )->HasOption();
}



void OptionSlotList::ClearOption( size_t slotIndex )
{
	GetMutableSlotController( slotIndex )->ClearOption();
}



void OptionSlotList::UpdateOption( size_t slotIndex, options::Option const& option )
{
	GetMutableSlotController( slotIndex )->UpdateOption( option );
}



WeakPtr<OptionSlot const> OptionSlotList::GetSlotController( size_t slotIndex ) const
{
	WeakPtr<OptionSlot const> retVal;
	PtrTransformer<OptionSlot>::PerformNonTypesafeTransformation(
		GenericListBox::GetSlotController( slotIndex ), retVal );
	return retVal;
}



WeakPtr<OptionSlot> OptionSlotList::GetMutableSlotController( size_t slotIndex )
{
	WeakPtr<OptionSlot> retVal;
	PtrTransformer<OptionSlot>::PerformNonTypesafeTransformation(
		GenericListBox::GetMutableSlotController( slotIndex ), retVal );
	return retVal;
}



void OptionSlotList::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	// Do any of our slots have direct focus?
	InstancedController const* const slotWithFocus = GetSlotWithFocus( context );
	bool const listBoxHasImplicitFocus = slotWithFocus != nullptr;

	// Update colors
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		WeakPtr<OptionSlot> const slotController = GetMutableSlotController( i );

		if( listBoxHasImplicitFocus )
		{
			if( slotController == slotWithFocus )
			{
				slotController->UpdateState( true, true );
			}
			else
			{
				slotController->UpdateState( true, false );
			}
		}
		else
		{
			slotController->UpdateState( false, false );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void OptionSlotList::PostInstanceAssign( UIContext& context, Container& container )
{
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		AssignSlotController<OptionSlot>( context, i, DefaultCreator<OptionSlot>::Create() );
	}
}



bool OptionSlotList::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	WeakPtr<OptionSlot const> controller;
	PtrTransformer<OptionSlot>::PerformNonTypesafeTransformation( attemptedFocus, controller );
	if( controller->HasOption() == false )
	{
		// No option, skip
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

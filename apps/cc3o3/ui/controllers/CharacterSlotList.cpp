#include "stdafx.h"
#include "CharacterSlotList.h"

#include "cc3o3/ui/controllers/CharacterSlot.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"


RFTYPE_CREATE_META( RF::cc::ui::controller::CharacterSlotList )
{
	RFTYPE_META().BaseClass<RF::ui::controller::GenericListBox>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::ui::controller::CharacterSlotList );
}

namespace RF::cc::ui::controller {
///////////////////////////////////////////////////////////////////////////////

CharacterSlotList::CharacterSlotList()
	: GenericListBox(
		  Orientation::Vertical,
		  3 )
{
	//
}



bool CharacterSlotList::HasCharacter( size_t slotIndex ) const
{
	// TODO: Fix API
	return const_cast<CharacterSlotList*>( this )->GetSlotController( slotIndex )->HasCharacter();
}



void CharacterSlotList::ClearCharacter( size_t slotIndex )
{
	GetSlotController( slotIndex )->ClearCharacter();
}



void CharacterSlotList::UpdateCharacter( size_t slotIndex, state::ObjectRef const& character )
{
	GetSlotController( slotIndex )->UpdateCharacter( character );
}



WeakPtr<CharacterSlot> CharacterSlotList::GetSlotController( size_t slotIndex )
{
	WeakPtr<CharacterSlot> retVal;
	PtrTransformer<CharacterSlot>::PerformNonTypesafeTransformation(
		GenericListBox::GetSlotController( slotIndex ), retVal );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void CharacterSlotList::PostInstanceAssign( UIContext& context, Container& container )
{
	for( size_t i = 0; i < mNumSlots; i++ )
	{
		AssignSlotController<CharacterSlot>( context, i, DefaultCreator<CharacterSlot>::Create() );
	}
}



bool CharacterSlotList::ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const
{
	WeakPtr<CharacterSlot const> controller;
	PtrTransformer<CharacterSlot>::PerformNonTypesafeTransformation( attemptedFocus, controller );
	if( controller->HasCharacter() == false )
	{
		// No character, skip
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}

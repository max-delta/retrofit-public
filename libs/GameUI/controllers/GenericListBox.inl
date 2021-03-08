#pragma once
#include "GenericListBox.h"

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
inline WeakPtr<T> GenericListBox::AssignSlotController( UIContext& context, size_t slotIndex, UniquePtr<T>&& controller )
{
	WeakPtr<T> const retVal = controller;
	AssignSlotControllerInternal( context, slotIndex, rftl::move( controller ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

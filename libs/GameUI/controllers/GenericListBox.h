#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core/ptr/unique_ptr.h"

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// A horizontal / vertical list
class GAMEUI_API GenericListBox : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( GenericListBox );

	//
	// Public methods
public:
	GenericListBox() = delete;
	GenericListBox(
		size_t numSlots );
	GenericListBox(
		Orientation orientation,
		size_t numSlots );

	ContainerID GetChildContainerID() const;
	size_t GetNumSlots() const;
	WeakPtr<InstancedController const> GetSlotController( size_t slotIndex ) const;
	WeakPtr<InstancedController> GetMutableSlotController( size_t slotIndex );
	template<typename T>
	WeakPtr<T> AssignSlotController( UIContext& context, size_t slotIndex, UniquePtr<T>&& controller );
	void SetWrapping( bool wrapping );
	void SetPagination( bool pagination );

	bool SlotHasCurrentFocus( UIConstContext const& context ) const;
	size_t GetSlotIndexWithSoftFocus( UIConstContext const& context ) const;
	bool SetSlotIndexWithSoftFocus( UIContext& context, size_t index );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override final;
	virtual void OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode ) override final;
	virtual bool OnFocusEvent( UIContext& context, FocusEvent const& focusEvent ) override final;


	//
	// Protected methods
protected:
	void AssignSlotControllerInternal( UIContext& context, size_t slotIndex, UniquePtr<InstancedController>&& controller );
	InstancedController const* GetSlotWithFocus( UIConstContext const& context ) const;
	bool ShouldSkipFocus( UIConstContext const& context, FocusTreeNode const& potentialFocus ) const;

	virtual void PostInstanceAssign( UIContext& context, Container& container );
	virtual bool ShouldSkipFocus( UIConstContext const& context, WeakPtr<InstancedController const> attemptedFocus ) const;
	virtual bool OnUnhandledFocusEvent( UIContext& context, FocusEvent const& focusEvent );


	//
	// Protected data
protected:
	Orientation const mOrientation;
	size_t const mNumSlots;
	bool mWrapping = false;
	bool mPagination = false;
	ContainerID mChildContainerID = kInvalidContainerID;
	rftl::vector<ContainerID> mSlicerChildContainerIDs;
	rftl::vector<WeakPtr<InstancedController>> mSlotControllers;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "GenericListBox.inl"

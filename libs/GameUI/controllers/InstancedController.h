#pragma once
#include "project.h"

#include "GameUI/Controller.h"

#include "core/ptr/unique_ptr.h"


namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API InstancedController : public Controller
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( InstancedController );

	//
	// Public methods
public:
	InstancedController();

	ContainerID GetContainerID() const;

	bool IsCurrentFocus( UIConstContext const& context ) const;
	bool IsInCurrentFocusStack( UIConstContext const& context ) const;

	WeakPtr<FocusTreeNode const> GetFocusTreeNode( UIConstContext const& context ) const;
	WeakPtr<FocusTreeNode> GetMutableFocusTreeNode( UIContext& context );
	WeakPtr<FocusTreeNode> AddAsChildToFocusTreeNode( UIContext& context, FocusTreeNode& parentNode );
	WeakPtr<FocusTreeNode> AddAsSiblingAfterFocusTreeNode( UIContext& context, WeakPtr<FocusTreeNode> previousNode );
	WeakPtr<FocusTreeNode> AddAsSiblingBeforeFocusTreeNode( UIContext& context, WeakPtr<FocusTreeNode> nextNode );

	virtual void OnInstanceAssign( UIContext& context, Container& container );
	virtual void OnAddedToFocusTree( UIContext& context, FocusTreeNode& newNode );
	virtual bool OnFocusEvent( UIContext& context, FocusEvent const& focusEvent );

	virtual void OnAssign( UIContext& context, Container& container ) override final;


	//
	// Protected methods
protected:
	FocusTarget const& GetFocusTarget() const;
	FocusTarget& GetMutableFocusTarget();

	void RequestHardRecalc( UIContext& context );


	//
	// Private methods
private:
	static bool HandleFocusEvent( UIContext& context, void* userData, FocusEvent const& focusEvent );


	//
	// Private data
private:
	ContainerID mContainerID = kInvalidContainerID;
	UniquePtr<FocusTarget> mFocusTarget;
};

///////////////////////////////////////////////////////////////////////////////
}

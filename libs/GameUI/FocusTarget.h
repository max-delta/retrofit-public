#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API FocusTarget
{
	RF_NO_COPY( FocusTarget );

	//
	// Types
public:
	using EventHandlerFunc = bool ( * )( UIContext& context, void* userData, FocusEvent const& focusEvent );


	//
	// Public methods
public:
	FocusTarget() = default;
	bool HandleEvent( UIContext& context, FocusEvent const& focusEvent ) const;

	bool HasHardFocus() const;


	//
	// Public data
public:
	// Can this be focused?
	bool mIsFocusable = true;

	// Can children be focused?
	bool mPropogatesFocus = true;

	// Callback, intended to be used for communicating with the owner of the
	//  FocusTarget via 'this' pointer stored in user data
	// NOTE: Handler cannot be null
	void* mUserData = nullptr;
	EventHandlerFunc mEventHandler = nullptr;

	// The container, for hit tests
	// NOTE: Container cannot be invalid, and must outlive focus target
	ContainerID mContainerID = kInvalidContainerID;

	// If set, this target has the hard focus
	UniquePtr<FocusKey> mHardFocusKey = nullptr;

	// If set, this in a focus tree
	WeakPtr<FocusTreeNode> mFocusTreeNode = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}}

#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API UIContext
{
	RF_NO_COPY( UIContext );

	//
	// Public methods
public:
	UIContext( ContainerManager& containerManager, FocusManager& focusManager );

	ContainerManager const& GetContainerManager() const;
	ContainerManager& GetMutableContainerManager();

	FocusManager const& GetFocusManager() const;
	FocusManager& GetMutableFocusManager();


	//
	// Private data
private:
	ContainerManager& mContainerManager;
	FocusManager& mFocusManager;
};

///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API UIConstContext
{
	RF_NO_COPY( UIConstContext );

	//
	// Public methods
public:
	UIConstContext( ContainerManager const& containerManager, FocusManager const& focusManager );

	ContainerManager const& GetContainerManager() const;

	FocusManager const& GetFocusManager() const;


	//
	// Private data
private:
	ContainerManager const& mContainerManager;
	FocusManager const& mFocusManager;
};

///////////////////////////////////////////////////////////////////////////////
}}

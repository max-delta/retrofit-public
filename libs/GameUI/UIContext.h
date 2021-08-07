#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"


namespace RF::ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API UIContext
{
	RF_NO_COPY( UIContext );

	//
	// Friends
private:
	friend struct UIConstContext;


	//
	// Public methods
public:
	UIContext( ContainerManager& containerManager );

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
	explicit UIConstContext( ContainerManager const& containerManager );
	UIConstContext( UIContext const& context );

	ContainerManager const& GetContainerManager() const;

	FocusManager const& GetFocusManager() const;


	//
	// Private data
private:
	ContainerManager const& mContainerManager;
	FocusManager const& mFocusManager;
};

///////////////////////////////////////////////////////////////////////////////
}

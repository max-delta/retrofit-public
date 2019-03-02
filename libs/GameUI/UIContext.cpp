#include "stdafx.h"
#include "UIContext.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

UIContext::UIContext( ContainerManager& containerManager, FocusManager& focusManager )
	: mContainerManager( containerManager )
	, mFocusManager( focusManager )
{
	//
}



ContainerManager const& UIContext::GetContainerManager() const
{
	return mContainerManager;
}



ContainerManager& UIContext::GetMutableContainerManager()
{
	return mContainerManager;
}



FocusManager const& UIContext::GetFocusManager() const
{
	return mFocusManager;
}



FocusManager& UIContext::GetMutableFocusManager()
{
	return mFocusManager;
}

///////////////////////////////////////////////////////////////////////////////

UIConstContext::UIConstContext( ContainerManager const& containerManager, FocusManager const& focusManager )
	: mContainerManager( containerManager )
	, mFocusManager( focusManager )
{
	//
}



ContainerManager const& UIConstContext::GetContainerManager() const
{
	return mContainerManager;
}



FocusManager const& UIConstContext::GetFocusManager() const
{
	return mFocusManager;
}

///////////////////////////////////////////////////////////////////////////////
}}

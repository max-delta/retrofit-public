#include "stdafx.h"
#include "UIContext.h"

#include "GameUI/ContainerManager.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

UIContext::UIContext( ContainerManager& containerManager )
	: mContainerManager( containerManager )
	, mFocusManager( containerManager.GetMutableFocusManager() )
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

UIConstContext::UIConstContext( ContainerManager const& containerManager )
	: mContainerManager( containerManager )
	, mFocusManager( containerManager.GetFocusManager() )
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

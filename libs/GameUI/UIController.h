#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API UIController
{
public:
	virtual ~UIController() = default;

	virtual void OnAssign( ContainerManager& manager, Container& container );
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container );

	// NOTE: Do NOT destroy child containers or anchors, this will happen
	//  automatically, and attempting to do so here may cause re-entrancy
	//  violations and undefined behavior
	// NOTE: Container is const to help enforce this
	virtual void OnImminentDestruction( ContainerManager& manager, Container const& container );
};

///////////////////////////////////////////////////////////////////////////////
}}

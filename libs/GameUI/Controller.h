#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "PPU/PPUFwd.h"

#include "RFType/CreateClassInfoDeclaration.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API Controller : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	//
	// Public methods
public:
	virtual void OnAssign( ContainerManager& manager, Container& container );
	virtual void OnAABBRecalc( ContainerManager& manager, Container& container );

	// NOTE: Do NOT destroy child containers or anchors, this will happen
	//  automatically, and attempting to do so here may cause re-entrancy
	//  violations and undefined behavior
	// NOTE: Container is const to help enforce this
	virtual void OnImminentDestruction( ContainerManager& manager, Container const& container );

	// NOTE: Do not attempt to mutate the hierachy while a render pass is in
	//  progress, as optimizations may be in place to assume it is immutable
	// NOTE: Manager is const to help enforce this
	virtual void OnRender( ContainerManager const& manager, Container const& container, bool& blockChildRendering );


	//
	// Protected methods
protected:
	gfx::PPUController& GetRenderer( ContainerManager const& manager ) const;
	FontRegistry const& GetFontRegistry( ContainerManager const& manager ) const;

	Container& GetMutableContainer( ContainerManager& manager, ContainerID containerID );
	ContainerID CreateChildContainer(
		ContainerManager& manager,
		Container& parentContainer,
		AnchorID leftConstraint,
		AnchorID rightConstraint,
		AnchorID topConstraint,
		AnchorID bottomConstraint );
	void DestroyContainer( ContainerManager& manager, ContainerID containerID );

	AnchorID CreateAnchor( ContainerManager& manager, Container& container );
	void MoveAnchor( ContainerManager& manager, AnchorID anchorID, gfx::PPUCoord pos );
};

///////////////////////////////////////////////////////////////////////////////
}}

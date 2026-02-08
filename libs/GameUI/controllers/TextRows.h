#pragma once
#include "project.h"

#include "GameUI/controllers/InstancedController.h"

#include "core_math/Color3f.h"

#include "rftl/span"
#include "rftl/string_view"

// Forwards
namespace RF::ui::controller {
class TextLabel;
}

namespace RF::ui::controller {
///////////////////////////////////////////////////////////////////////////////

// Multiple lines of text
class GAMEUI_API TextRows final : public InstancedController
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
	RF_NO_COPY( TextRows );

	//
	// Public methods
public:
	TextRows() = delete;
	TextRows( size_t numSlots, FontPurposeID purpose );
	TextRows(
		size_t numSlots,
		FontPurposeID purpose,
		Justification::Value justification,
		math::Color3f color );

	ContainerID GetChildContainerID() const;
	WeakPtr<TextLabel const> GetSlotController( size_t slotIndex ) const;
	WeakPtr<TextLabel> GetMutableSlotController( size_t slotIndex );
	void SetText( rftl::span<rftl::string const> text );
	void SetText( rftl::span<rftl::string_view const> text );

	virtual void OnInstanceAssign( UIContext& context, Container& container ) override;
	virtual void OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering ) override;


	//
	// Private methods
private:
	template<typename T>
	void SetTextInternal( rftl::span<T const> text );


	//
	// Private data
private:
	size_t const mNumSlots;
	FontPurposeID const mFontPurpose;
	Justification::Value const mJustification;
	math::Color3f const mColor;
	ContainerID mChildContainerID = kInvalidContainerID;
	rftl::vector<WeakPtr<TextLabel>> mSlotControllers;
};

///////////////////////////////////////////////////////////////////////////////
}

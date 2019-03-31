#include "stdafx.h"
#include "MessageBox.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FontRegistry.h"
#include "GameUI/Container.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/TextBox.h"

#include "PPU/PPUController.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core/ptr/default_creator.h"

#include "rftl/algorithm"


RFTYPE_CREATE_META( RF::ui::controller::MessageBox )
{
	RFTYPE_META().BaseClass<RF::ui::controller::InstancedController>();
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::ui::controller::MessageBox );
}

namespace RF { namespace ui { namespace controller {
///////////////////////////////////////////////////////////////////////////////

MessageBox::MessageBox(
	size_t numRows,
	FontPurposeID purpose,
	rftl::unordered_set<char> const& breakableChars )
	: MessageBox(
		numRows,
		purpose,
		Justification::MiddleCenter,
		math::Color3f::kWhite,
		breakableChars )
{
	//
}



MessageBox::MessageBox(
	size_t numRows,
	FontPurposeID purpose,
	Justification justification,
	math::Color3f color,
	rftl::unordered_set<char> const& breakableChars )
	: mNumRows( numRows )
	, mFontPurpose( purpose )
	, mJustification( justification )
	, mColor( color )
	, mBreakableChars( breakableChars )
{
	RF_ASSERT( mNumRows >= 2 );
}



void MessageBox::SetFrameTileset( ui::UIContext& context, gfx::ManagedTilesetID tileset, gfx::PPUCoordElem expectedTileWidth, gfx::PPUCoordElem expectedTileHeight )
{
	mFrameController->SetTileset( context, tileset, expectedTileWidth, expectedTileHeight );
}



ContainerID MessageBox::GetChildContainerID() const
{
	return mChildContainerID;
}



void MessageBox::SetText( rftl::string const& text, bool rightToLeft )
{
	mRightToLeft = rightToLeft;
	mText = text;
}



size_t MessageBox::GetNumCharactersDispatchedLastRender() const
{
	RF_TODO_BREAK();
	return 0;
}



size_t MessageBox::GetNumCharactersTruncatedLastRender() const
{
	RF_TODO_BREAK();
	return 0;
}



void MessageBox::OnInstanceAssign( UIContext& context, Container& container )
{
	mChildContainerID = CreateChildContainer(
		context.GetMutableContainerManager(),
		container,
		container.mLeftConstraint,
		container.mRightConstraint,
		container.mTopConstraint,
		container.mBottomConstraint );

	WeakPtr<ui::controller::BorderFrame> const frame =
		context.GetMutableContainerManager().AssignStrongController(
			mChildContainerID,
			DefaultCreator<ui::controller::BorderFrame>::Create() );
	frame->SetJustification( ui::Justification::MiddleCenter );
	mFrameController = frame;

	WeakPtr<ui::controller::TextBox> const text =
		context.GetMutableContainerManager().AssignStrongController(
			frame->GetChildContainerID(),
			DefaultCreator<ui::controller::TextBox>::Create(
				mNumRows,
				mFontPurpose,
				mJustification,
				mColor,
				mBreakableChars ) );
	mTextController = text;
}



void MessageBox::OnRender( UIConstContext const& context, Container const& container, bool& blockChildRendering )
{
	if( mText.empty() )
	{
		mTextController->SetText( rftl::string(), mRightToLeft );
		return;
	}

	// TODO: Animation logic
	mTextController->SetText( mText, mRightToLeft );
}

///////////////////////////////////////////////////////////////////////////////
}}}

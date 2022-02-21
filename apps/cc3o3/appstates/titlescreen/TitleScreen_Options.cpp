#include "stdafx.h"
#include "TitleScreen_Options.h"

#include "cc3o3/cc3o3.h"
#include "cc3o3/appstates/titlescreen/TitleScreen.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/options/OptionSet.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"
#include "cc3o3/ui/controllers/OptionSlotList.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusTarget.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"
#include "GameUI/controllers/BorderFrame.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_Options::InternalState
{
public:
	static constexpr size_t kNumOptionsFields = 14;

public:
	RF_NO_COPY( InternalState );
	InternalState() = default;

	void GenerateOptions();
	void UpdateOptions();

public:
	options::OptionSet mTempOptions;
	bool mReturnToMainMenu = false;
	WeakPtr<ui::controller::OptionSlotList> mOptionsListBox;
};



void TitleScreen_Options::InternalState::GenerateOptions()
{
	using namespace options;

	mTempOptions = {};

	mTempOptions.mIdentifier = "temp";
	mTempOptions.mDisplayName = "Temp";
	OptionSet::Options& options = mTempOptions.mOptions;

	static auto const addUnboundOption = [&options]( Option&& option ) -> void {
		options.emplace_back( OptionSet::Entry{ option, OptionValue{} } );
	};

	static constexpr auto makeDevHop =
		[]( rftl::string_view identifier, std::string_view displayName, AppStateID id ) -> Option {
		return Option::MakeAction( identifier, displayName, [id] {
			RequestGlobalDeferredStateChange( id );
		} );
	};

	{
		Option option = makeDevHop( "devhop_rollback", "DevHop -> Rollback", id::DevTestRollback );
		addUnboundOption( rftl::move( option ) );
	}
	{
		Option option = makeDevHop( "devhop_comcharts", "DevHop -> Combat charts", id::DevTestCombatCharts );
		addUnboundOption( rftl::move( option ) );
	}
	{
		Option option = makeDevHop( "devhop_gridcharts", "DevHop -> Grid charts", id::DevTestGridCharts );
		addUnboundOption( rftl::move( option ) );
	}
	{
		Option option = Option::MakeList( "test", "Test",
			{ { "a", "A" }, { "b", "B" }, { "c", "C" } } );
		addUnboundOption( rftl::move( option ) );
	}
	{
		Option option = Option::MakeAction(
			"mainmenu",
			ui::LocalizeKey( "$titleopt_mainmenu" ),
			[this] {
				this->mReturnToMainMenu = true;
			} );
		addUnboundOption( rftl::move( option ) );
	}
}



void TitleScreen_Options::InternalState::UpdateOptions()
{
	// TODO: Other sets
	options::OptionSet const& options = mTempOptions;

	ui::controller::OptionSlotList& listBox = *mOptionsListBox;

	// TODO: Scrolling?
	size_t const numOptions = options.mOptions.size();
	size_t const numSlots = listBox.GetNumSlots();
	RF_ASSERT( numOptions <= numSlots );

	for( size_t i = 0; i < numSlots; i++ )
	{
		if( i < numOptions )
		{
			// TODO: Values
			options::OptionSet::Entry const& entry = options.mOptions.at( i );
			options::Option const& option = entry.mOption;
			options::OptionValue const& value = entry.mValue;
			listBox.UpdateOption( i, option, value );
		}
		else
		{
			listBox.ClearOption( i );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_Options::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	gfx::ppu::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	// Setup UI
	{
		ui::ContainerManager& uiManager = *app::gUiManager;
		ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
		ui::UIContext uiContext( uiManager );
		uiManager.RecreateRootContainer();

		// Cut the whole screen into columns
		ui::controller::ColumnSlicer::Ratios const rootColumnRatios = {
			{ 1.f / 20.f, false },
			{ 18.f / 20.f, true },
			{ 1.f / 20.f, false },
		};
		WeakPtr<ui::controller::ColumnSlicer> const rootColumnSlicer =
			uiManager.AssignStrongController(
				ui::kRootContainerID,
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					rootColumnRatios ) );

		// Cut the center in 4
		ui::controller::ColumnSlicer::Ratios const centerRowRatios = {
			{ 1.f / 14.f, false },
			{ 1.f / 14.f, true },
			{ 11.f / 14.f, true },
			{ 1.f / 14.f, false },
		};
		WeakPtr<ui::controller::RowSlicer> const centerRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					centerRowRatios ) );

		// Header in top center
		WeakPtr<ui::controller::TextLabel> const header =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		header->SetJustification( ui::Justification::BottomCenter );
		header->SetFont( ui::font::LargeMenuHeader );
		header->SetText( "Options" );
		header->SetColor( math::Color3f::kWhite );
		header->SetBorder( true );

		// Frame in middle center
		WeakPtr<ui::controller::BorderFrame> const frame =
			uiManager.AssignStrongController(
				centerRowSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::BorderFrame>::Create() );
		frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

		// Create menu selections in the left columns
		WeakPtr<ui::controller::OptionSlotList> const listBox =
			uiManager.AssignStrongController(
				frame->GetChildContainerID(),
				DefaultCreator<ui::controller::OptionSlotList>::Create(
					InternalState::kNumOptionsFields ) );
		listBox->SetWrapping( true );
		listBox->SetPagination( true );
		listBox->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
		mInternalState->mOptionsListBox = listBox;
	}

	// HACK: Temp options
	// TODO: Pages
	mInternalState->GenerateOptions();
	mInternalState->UpdateOptions();
}



void TitleScreen_Options::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_Options::OnTick( AppStateTickContext& context )
{
	bool shouldUpdateOptions = false;

	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetMainMenuInputToProcess();

	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		if( handled == false )
		{
			// Wasn't handled by general UI

			// Focused on the list?
			bool const listHasFocus = focusMan.GetFocusTree().IsInCurrentFocusStack( mInternalState->mOptionsListBox->GetContainerID() );
			if( listHasFocus )
			{
				// Focused on an option?
				size_t const focusIndex = mInternalState->mOptionsListBox->GetSlotIndexWithSoftFocus( uiContext );
				options::OptionSet::Options const& options = mInternalState->mTempOptions.mOptions;
				if( focusIndex < options.size() )
				{
					options::OptionSet::Entry const& entry = options.at( focusIndex );
					options::Option const& option = entry.mOption;
					options::OptionDesc const& desc = option.mDesc;

					if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
					{
						// Activate

						if( desc.mAction.has_value() )
						{
							// Perform action
							desc.mAction->mFunc();
							shouldUpdateOptions = true;
						}
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}

	if( shouldUpdateOptions )
	{
		mInternalState->UpdateOptions();
	}

	if( mInternalState->mReturnToMainMenu )
	{
		context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

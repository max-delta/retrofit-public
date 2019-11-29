#include "stdafx.h"
#include "TitleScreen_Options.h"

#include "cc3o3/appstates/TitleScreen.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/ui/LocalizationHelpers.h"
#include "cc3o3/ui/UIFwd.h"

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
#include "GameUI/controllers/ListBox.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core/ptr/default_creator.h"

#include "rftl/functional"


namespace RF { namespace cc { namespace appstate {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr size_t kNumOptionsPerSet = 12;
static constexpr size_t kNumOptionsFields = kNumOptionsPerSet + 2;
static constexpr char const* kOptTag[kNumOptionsPerSet] = {
	"OPT0",
	"OPT1",
	"OPT2",
	"OPT3",
	"OPT4",
	"OPT5",
	"OPT6",
	"OPT7",
	"OPT8",
	"OPT9",
	"OPT10",
	"OPT11"
};
static constexpr char kUnusedTag[] = "UNUSED";
static constexpr char kReturnTag[] = "RETURN";

struct OptionSet
{
	struct Option
	{
		rftl::string mStaticText;
		rftl::function<void()> mFunction = nullptr;

		rftl::function<rftl::string()> mDynamicText = nullptr;
	};

	void Update( ui::controller::ListBox& listBox ) const;

	Option mOptions[kNumOptionsPerSet] = {};
};

void OptionSet::Update( ui::controller::ListBox& listBox ) const
{
	rftl::vector<rftl::string> optionsText;
	for( size_t i = 0; i < kNumOptionsPerSet; i++ )
	{
		Option const& opt = mOptions[i];
		if( opt.mDynamicText != nullptr )
		{
			optionsText.emplace_back( opt.mDynamicText() );
		}
		else
		{
			optionsText.emplace_back( opt.mStaticText );
		}
	}
	optionsText.emplace_back(); // Unused
	optionsText.emplace_back( ui::LocalizeKey( "$titleopt_mainmenu" ) );
	listBox.SetText( optionsText );
}

struct UpdateTest
{
	static inline char sVal = '0';
	static rftl::string String()
	{
		return rftl::string( "Option update test: " ) + sVal;
	}
	static void Action()
	{
		sVal++;
		if( sVal > '9' )
		{
			sVal = '0';
		}
	}
	static inline OptionSet::Option const sOption{
		"",
		Action,
		String
	};
};

struct ToggleTest
{
	static inline bool sVal = false;
	static rftl::string String()
	{
		return rftl::string( "Option toggle test: " ) + ( sVal ? "True" : "False" );
	}
	static void Action()
	{
		sVal = !sVal;
	}
	static inline OptionSet::Option const sOption{
		"",
		Action,
		String
	};
};

static OptionSet sDevOptions = { { //
	UpdateTest::sOption,
	ToggleTest::sOption,
	{ "Menu option text for slot 3" },
	{ "Menu option text for slot 4" },
	{ "Menu option text for slot 5" },
	{ "Menu option text for slot 6" },
	{ "Menu option text for slot 7" },
	{ "Menu option text for slot 8" },
	{ "Menu option text for slot 9" },
	{ "Menu option text for slot 10" },
	{ "Menu option text for slot 11" },
	{ "Menu option text for slot 12" } } };

}
///////////////////////////////////////////////////////////////////////////////

struct TitleScreen_Options::InternalState
{
	RF_NO_COPY( InternalState );
	InternalState() = default;

	WeakPtr<ui::controller::ListBox> mOptionsListBox;
};

///////////////////////////////////////////////////////////////////////////////

void TitleScreen_Options::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();

	gfx::PPUController const& ppu = *app::gGraphics;
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
		frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "frame9_24" ), { 4, 4 }, { 2, 0 } );

		// Cut the frame into columns
		ui::controller::ColumnSlicer::Ratios const frameColumnRatios = {
			{ 1.f / 2.f, true },
			{ 1.f / 2.f, true },
		};
		WeakPtr<ui::controller::ColumnSlicer> const frameColumnSlicer =
			uiManager.AssignStrongController(
				frame->GetChildContainerID(),
				DefaultCreator<ui::controller::ColumnSlicer>::Create(
					frameColumnRatios ) );

		// Create menu selections in the left columns
		WeakPtr<ui::controller::ListBox> const leftOptions =
			uiManager.AssignStrongController(
				frameColumnSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::ListBox>::Create(
					details::kNumOptionsFields,
					ui::font::SmallMenuSelection,
					ui::Justification::MiddleLeft,
					math::Color3f::kGray50,
					math::Color3f::kWhite,
					math::Color3f::kYellow ) );
		details::sDevOptions.Update( *leftOptions );
		leftOptions->AddAsChildToFocusTreeNode( uiContext, focusMan.GetFocusTree().GetRootNode() );
		for( size_t i = 0; i < details::kNumOptionsPerSet; i++ )
		{
			uiManager.AssignLabel( leftOptions->GetSlotController( i )->GetContainerID(), details::kOptTag[i] );
		}
		uiManager.AssignLabel( leftOptions->GetSlotController( details::kNumOptionsPerSet )->GetContainerID(), details::kUnusedTag );
		uiManager.AssignLabel( leftOptions->GetSlotController( details::kNumOptionsPerSet + 1 )->GetContainerID(), details::kReturnTag );
		mInternalState->mOptionsListBox = leftOptions;
	}
}



void TitleScreen_Options::OnExit( AppStateChangeContext& context )
{
	app::gUiManager->RecreateRootContainer();

	mInternalState = nullptr;
}



void TitleScreen_Options::OnTick( AppStateTickContext& context )
{
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

			// Figure out the useful focus information
			WeakPtr<ui::FocusTreeNode const> currentFocus;
			WeakPtr<ui::FocusTarget const> currentFocusTarget;
			ui::ContainerID currentFocusContainerID = ui::kInvalidContainerID;
			{
				currentFocus = focusMan.GetFocusTree().GetCurrentFocus();
				if( currentFocus != nullptr )
				{
					currentFocusTarget = currentFocus->mFocusTarget;
				}
				if( currentFocusTarget != nullptr )
				{
					RF_ASSERT( currentFocusTarget->HasHardFocus() );
					currentFocusContainerID = currentFocusTarget->mContainerID;
				}
			}

			if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
			{
				if( currentFocusContainerID != ui::kInvalidContainerID )
				{
					if( currentFocusContainerID == uiManager.GetContainerID( details::kReturnTag ) )
					{
						// Return
						context.mManager.RequestDeferredStateChange( id::TitleScreen_MainMenu );
					}
					else
					{
						for( size_t i = 0; i < details::kNumOptionsPerSet; i++ )
						{
							char const* const tag = details::kOptTag[i];
							if( currentFocusContainerID == uiManager.GetContainerID( tag ) )
							{
								// Option
								details::OptionSet::Option const& opt = details::sDevOptions.mOptions[i];
								if( opt.mFunction != nullptr )
								{
									opt.mFunction();
								}
								if( opt.mDynamicText != nullptr )
								{
									rftl::string const newText = opt.mDynamicText();
									mInternalState->mOptionsListBox->GetSlotController( i )->SetText( newText.c_str() );
								}
								break;
							}
						}
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}

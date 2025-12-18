#include "stdafx.h"
#include "Gameplay_Site.h"

#include "cc3o3/Common.h"
#include "cc3o3/CommonPaths.h"
#include "cc3o3/appstates/InputHelpers.h"
#include "cc3o3/campaign/CampaignManager.h"
#include "cc3o3/site/Site.h"
#include "cc3o3/ui/LocalizationHelpers.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "GameAppState/AppStateTickContext.h"
#include "GameAppState/AppStateManager.h"

#include "GameUI/ContainerManager.h"
#include "GameUI/FocusEvent.h"
#include "GameUI/FocusManager.h"
#include "GameUI/UIContext.h"
#include "GameUI/controllers/BorderFrame.h"
#include "GameUI/controllers/ColumnSlicer.h"
#include "GameUI/controllers/Floater.h"
#include "GameUI/controllers/ListBox.h"
#include "GameUI/controllers/MultiPassthrough.h"
#include "GameUI/controllers/Passthrough.h"
#include "GameUI/controllers/RowSlicer.h"
#include "GameUI/controllers/TextLabel.h"

#include "PPU/PPUController.h"
#include "PPU/TilesetManager.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::appstate {
///////////////////////////////////////////////////////////////////////////////

struct Gameplay_Site::InternalState
{
	RF_NO_COPY( InternalState );

public:
	struct TopLevelSections
	{
		enum Section : size_t
		{
			kVista = 0,
			kHACKBattle,
			kMenus,
			kLeave,

			kNumSections
		};

		static constexpr char const* kLabels[kNumSections] = {
			"H_vista",
			"H_hackbattle",
			"H_menus",
			"H_leave",
		};

		static constexpr char const* kText[kNumSections] = {
			"$sitemenu_toplevel_vista",
			"$sitemenu_toplevel_hackbattle",
			"$sitemenu_toplevel_menus",
			"$sitemenu_toplevel_leave",
		};
	};


public:
	InternalState() = default;

	void HideAllTopLevelSections();
	void SwitchTopLevelSection( TopLevelSections::Section section );
	bool IsActiveSection( TopLevelSections::Section section ) const;
	void SwitchToSelector( ui::UIContext& context );
	void LeaveSelector( ui::UIContext& context, TopLevelSections::Section chosenSection );


public:
	WeakPtr<ui::controller::TextLabel> mMainHeader;

	WeakPtr<ui::controller::InstancedController> mSectionSelectorFloater;
	WeakPtr<ui::controller::ListBox> mSectionSelector;

	using TopLevelControllers = rftl::array<WeakPtr<ui::controller::InstancedController>, TopLevelSections::kNumSections>;
	TopLevelControllers mTopLevelControllers;
};



void Gameplay_Site::InternalState::HideAllTopLevelSections()
{
	for( WeakPtr<ui::controller::InstancedController> const& controller : mTopLevelControllers )
	{
		controller->SetChildRenderingBlocked( true );
	}
}



void Gameplay_Site::InternalState::SwitchTopLevelSection( TopLevelSections::Section section )
{
	// Only show current section
	HideAllTopLevelSections();
	mTopLevelControllers.at( section )->SetChildRenderingBlocked( false );
}



bool Gameplay_Site::InternalState::IsActiveSection( TopLevelSections::Section section ) const
{
	return mTopLevelControllers.at( section )->IsChildRenderingBlocked() == false;
}



void Gameplay_Site::InternalState::SwitchToSelector( ui::UIContext& context )
{
	HideAllTopLevelSections();

	mSectionSelectorFloater->SetChildRenderingBlocked( false );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mSectionSelectorFloater->GetMutableFocusTreeNode( context ) );
}



void Gameplay_Site::InternalState::LeaveSelector( ui::UIContext& context, TopLevelSections::Section chosenSection )
{
	SwitchTopLevelSection( chosenSection );

	mSectionSelectorFloater->SetChildRenderingBlocked( true );
	context.GetMutableFocusManager().GetMutableFocusTree().SetRootFocusToSpecificChild(
		mTopLevelControllers.at( chosenSection )->GetMutableFocusTreeNode( context ) );
}

///////////////////////////////////////////////////////////////////////////////

void Gameplay_Site::OnEnter( AppStateChangeContext& context )
{
	mInternalState = DefaultCreator<InternalState>::Create();
	InternalState& internalState = *mInternalState;

	gfx::ppu::PPUController const& ppu = *app::gGraphics;
	gfx::TilesetManager const& tsetMan = *ppu.GetTilesetManager();

	// Setup site
	{
		campaign::CampaignManager& campaign = *gCampaignManager;

		// Load site
		site::Site const site = campaign.LoadDataForSite();

		// TODO
		( (void)site );
	}

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

		// Cut the middle in 3
		ui::controller::RowSlicer::Ratios const middleRowRatios = {
			{ 1.f / 14.f, true },
			{ 12.f / 14.f, true },
			{ 1.f / 14.f, true },
		};
		WeakPtr<ui::controller::RowSlicer> const middleRowSlicer =
			uiManager.AssignStrongController(
				rootColumnSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::RowSlicer>::Create(
					middleRowRatios ) );

		// Header in top center
		WeakPtr<ui::controller::TextLabel> const mainHeader =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 0 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainHeader->SetJustification( ui::Justification::BottomCenter );
		mainHeader->SetFont( ui::font::LargeMenuHeader );
		mainHeader->SetText( "UNSET" );
		mainHeader->SetColor( math::Color3f::kWhite );
		mainHeader->SetBorder( true );
		internalState.mMainHeader = mainHeader;

		// Controls in bottom right
		WeakPtr<ui::controller::TextLabel> const mainFooter =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 2 ),
				DefaultCreator<ui::controller::TextLabel>::Create() );
		mainFooter->SetJustification( ui::Justification::TopRight );
		mainFooter->SetFont( ui::font::SmallMenuSelection );
		mainFooter->SetText( "UNSET" );
		mainFooter->SetColor( math::Color3f::kWhite );
		mainFooter->SetBorder( true );

		// Main sections are in the center
		// NOTE: Using one of the passthroughs for the section selector
		using TopLevelSections = InternalState::TopLevelSections;
		static constexpr size_t kPassthroughCount = TopLevelSections::kNumSections + 1;
		WeakPtr<ui::controller::MultiPassthrough> const sectionPassthroughs =
			uiManager.AssignStrongController(
				middleRowSlicer->GetChildContainerID( 1 ),
				DefaultCreator<ui::controller::MultiPassthrough>::Create( kPassthroughCount ) );

		// Section selector sits below everything else
		WeakPtr<ui::controller::Floater> const sectionSelectorFloater =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kNumSections ),
				DefaultCreator<ui::controller::Floater>::Create(
					math::integer_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize * 2 ),
					math::integer_cast<gfx::ppu::CoordElem>( gfx::ppu::kTileSize * 5 ),
					ui::Justification::TopLeft ) );
		uiManager.AdjustRecommendedRenderDepth( sectionSelectorFloater->GetContainerID(), +1 );
		sectionSelectorFloater->AddAsChildToFocusTreeNode( uiContext, focusMan.GetMutableFocusTree().GetMutableRootNode() );
		internalState.mSectionSelectorFloater = sectionSelectorFloater;

		// Sections each get a passthrough that can be used to shut off all
		//  children when switching sections
		WeakPtr<ui::controller::Passthrough> const vistaPassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kVista ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const HACKBattlePassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kHACKBattle ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const menusPassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kMenus ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		WeakPtr<ui::controller::Passthrough> const leavePassthrough =
			uiManager.AssignStrongController(
				sectionPassthroughs->GetChildContainerID( TopLevelSections::kLeave ),
				DefaultCreator<ui::controller::Passthrough>::Create() );
		internalState.mTopLevelControllers.at( TopLevelSections::kVista ) = vistaPassthrough;
		internalState.mTopLevelControllers.at( TopLevelSections::kHACKBattle ) = HACKBattlePassthrough;
		internalState.mTopLevelControllers.at( TopLevelSections::kMenus ) = menusPassthrough;
		internalState.mTopLevelControllers.at( TopLevelSections::kLeave ) = leavePassthrough;
		for( WeakPtr<ui::controller::InstancedController> const& controller : internalState.mTopLevelControllers )
		{
			controller->AddAsSiblingAfterFocusTreeNode( uiContext, sectionSelectorFloater->GetMutableFocusTreeNode( uiContext ) );
		}

		// Section selector
		{
			// Frame
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					sectionSelectorFloater->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "retro1_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );

			// Implement selector as vertical list
			WeakPtr<ui::controller::ListBox> const selector =
				uiManager.AssignStrongController(
					frame->GetChildContainerID(),
					DefaultCreator<ui::controller::ListBox>::Create(
						ui::Orientation::Vertical,
						TopLevelSections::kNumSections,
						ui::font::NarrowHalfTileMono ) );
			selector->SetWrapping( true );
			for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
			{
				WeakPtr<ui::controller::TextLabel> const slotController = selector->GetMutableSlotController( i );
				slotController->SetText( ui::LocalizeKey( TopLevelSections::kText[i] ) );
				uiManager.AssignLabel( slotController->GetContainerID(), TopLevelSections::kLabels[i] );
			}
			selector->AddAsChildToFocusTreeNode( uiContext, *sectionSelectorFloater->GetMutableFocusTreeNode( uiContext ) );
			internalState.mSectionSelector = selector;
		}

		// Vista section
		{
			// Placeholder
			WeakPtr<ui::controller::TextLabel> const placeholder =
				uiManager.AssignStrongController(
					vistaPassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::TextLabel>::Create() );
			placeholder->SetJustification( ui::Justification::MiddleRight );
			placeholder->SetFont( ui::font::SmallMenuSelection );
			placeholder->SetText( "UNSET_VISTA" );
			placeholder->SetColor( math::Color3f::kWhite );
			placeholder->SetBorder( true );
		}

		// HACK battle section
		{
			// Placeholder
			WeakPtr<ui::controller::BorderFrame> const frame =
				uiManager.AssignStrongController(
					HACKBattlePassthrough->GetChildContainerID(),
					DefaultCreator<ui::controller::BorderFrame>::Create() );
			frame->SetTileset( uiContext, tsetMan.GetManagedResourceIDFromResourceName( "wood_8_48" ), { 8, 8 }, { 48, 48 }, { 0, 0 } );
		}

		// Menus section
		{
			// No entry, currently just an instant effect
		}

		// Leave section
		{
			// No entry, currently just an instant effect
		}

		// Start off on the selector
		internalState.SwitchToSelector( uiContext );
	}
}



void Gameplay_Site::OnExit( AppStateChangeContext& context )
{
	mInternalState = nullptr;
}



void Gameplay_Site::OnTick( AppStateTickContext& context )
{
	using TopLevelSections = InternalState::TopLevelSections;

	InternalState& internalState = *mInternalState;
	ui::ContainerManager& uiManager = *app::gUiManager;
	ui::FocusManager& focusMan = uiManager.GetMutableFocusManager();
	campaign::CampaignManager& campaign = *gCampaignManager;

	ui::UIContext uiContext( uiManager );
	focusMan.UpdateHardFocus( uiContext );

	// Process menu actions
	rftl::vector<ui::FocusEventType> const focusEvents = InputHelpers::GetGameMenuInputToProcess( InputHelpers::GetSinglePlayer() );
	for( ui::FocusEventType const& focusEvent : focusEvents )
	{
		bool const handled = focusMan.HandleEvent( uiContext, focusEvent );
		focusMan.UpdateHardFocus( uiContext );

		// Figure out the useful focus information
		ui::ContainerID const currentFocusContainerID = focusMan.GetFocusTree().GetCurrentFocusContainerID();

		if( handled )
		{
			// Handled by the normal UI
			RF_TODO_ANNOTATION(
				"Check for certain selection changes and update some ancillary"
				" displays, similar to how the section selector works in the"
				" save/loadout menus by side-glancing at the current focus" );
		}
		else
		{
			// Wasn't handled by general UI

			if( currentFocusContainerID != ui::kInvalidContainerID )
			{
				if( internalState.mSectionSelector->SlotHasCurrentFocus( uiContext ) )
				{
					// Section selection

					if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
					{
						bool foundSection = false;
						for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
						{
							if( currentFocusContainerID == uiManager.GetContainerID( TopLevelSections::kLabels[i] ) )
							{
								TopLevelSections::Section const chosenSection = math::enum_bitcast<TopLevelSections::Section>( i );

								if( chosenSection == TopLevelSections::kMenus )
								{
									// Special case - Menus

									// Bring up menus
									campaign.EnterMenus( context );
								}
								else if( chosenSection == TopLevelSections::kLeave )
								{
									// Special case - Leave

									// HACK: Assume site can always be left
									RF_TODO_ANNOTATION(
										"Check if a site is leavable, and why"
										" not if it can't be left" );
									RF_ASSERT( campaign.CanLeaveSite() );
									campaign.LeaveSite( context );
								}
								else
								{
									// Standard case - Enter subsection

									// Hide the selector, moving focus to the
									//  chosen section instead
									internalState.LeaveSelector( uiContext, chosenSection );
								}

								foundSection = true;
								break;
							}
						}
						RF_ASSERT( foundSection );
					}
					else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
					{
						// Intentionally ignoring trying to back out of the
						//  site mode, requiring the player to intentionally
						//  try to 'Leave' the site instead
					}
				}
				else
				{
					// Sections
					TopLevelSections::Section currentSection = TopLevelSections::kNumSections;
					for( size_t i = 0; i < TopLevelSections::kNumSections; i++ )
					{
						if( internalState.mTopLevelControllers.at( i )->IsInCurrentFocusStack( uiContext ) )
						{
							currentSection = math::enum_bitcast<TopLevelSections::Section>( i );
						}
					}
					RF_ASSERT( currentSection != TopLevelSections::kNumSections );

					if( currentSection == TopLevelSections::kVista )
					{
						// Vista

						if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
						{
							// TODO
						}
						else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
						{
							// Return to section selector
							internalState.SwitchToSelector( uiContext );
						}
					}
					else if( currentSection == TopLevelSections::kHACKBattle )
					{
						// HACK battle

						if( focusEvent == ui::focusevent::Command_ActivateCurrentFocus )
						{
							// Start encounter
							campaign.StartEncounter( context, "TODO" );
						}
						else if( focusEvent == ui::focusevent::Command_CancelCurrentFocus )
						{
							// Return to section selector
							internalState.SwitchToSelector( uiContext );
						}
					}
					else
					{
						// Uknown/unhandled section
						RF_DBGFAIL();
					}
				}
			}
		}
		focusMan.UpdateHardFocus( uiContext );
	}

	app::gGraphics->DebugDrawText( gfx::ppu::Coord( 128, 32 ), "TODO: Site" );
}

///////////////////////////////////////////////////////////////////////////////
}

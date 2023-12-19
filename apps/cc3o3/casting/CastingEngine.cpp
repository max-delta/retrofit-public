#include "stdafx.h"
#include "CastingEngine.h"

#include "cc3o3/casting/CastError.h"
#include "cc3o3/casting/CombatContext.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "GameAction/ActionDatabase.h"
#include "GameAction/ActionRecord.h"
#include "GameAction/ConditionDatabase.h"
#include "GameAction/Context.h"
#include "GameAction/Environment.h"
#include "GameAction/Step.h"

#include "PlatformFilesystem/VFS.h"

#include "core_rftype/VirtualPtrCast.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////
namespace details {

UniquePtr<CastError> ExecuteCast(
	act::Environment&& env,
	combat::CombatInstance& combatInstance,
	combat::FighterID const& source,
	combat::FighterID const& target,
	rftl::string_view const& key,
	element::ElementLevel castedLevel )
{
	// Prepare context
	// NOTE: Combat instance is copied by the context, will need to extract it
	//  at the end and stomp the caller-provided instance
	combat::CombatInstance const& constCombatInstance = combatInstance;
	CombatContext ctx( constCombatInstance );
	ctx.mSourceFighter = source;
	ctx.mTargetFighter = target;
	ctx.mCastedLevel = castedLevel;

	// Fetch action from environment
	RF_ASSERT( env.mActionDatabase != nullptr );
	act::ActionDatabase const& actionDatabase = *env.mActionDatabase;
	WeakPtr<act::ActionRecord const> actionPtr = actionDatabase.GetAction( key );
	if( actionPtr == nullptr )
	{
		RFLOG_ERROR( key, RFCAT_CC3O3, "Couldn't fetch action definition from key" );
		UniquePtr<CastError> err = CastError::Create( env, ctx );
		err->mMissingActionKey = key;
		return err;
	}
	act::ActionRecord const& action = *actionPtr;

	// Make sure the cast can be performed
	bool const canCast = ctx.mCombatInstance.CanPerformCast( ctx.mSourceFighter, ctx.mCastedLevel );
	if( canCast == false )
	{
		RFLOG_ERROR( key, RFCAT_CC3O3, "Couldn't perform cast (basic checks failed)" );
		UniquePtr<CastError> err = CastError::Create( env, ctx );
		err->mPreActionCastInvalidatation = true;
		return err;
	}

	// Start the cast
	bool const startSuccess = ctx.mCombatInstance.StartCast( ctx.mSourceFighter, ctx.mCastedLevel );
	RF_ASSERT( startSuccess );

	// Get root step
	WeakPtr<act::Step const> rootPtr = action.GetRoot();
	RF_ASSERT( rootPtr );
	act::Step const& root = *rootPtr;

	// Execute the action
	CombatContext* currentCombatCtx = &ctx;
	UniquePtr<act::Context> newCtx = root.Execute( env, ctx );
	if( newCtx != nullptr )
	{
		// Resulted in a new context

		if( newCtx->IsATerminalError() )
		{
			// Error context
			RF_TODO_BREAK_MSG(
				"Check if it's an error ctx that we can pull a cast error out"
				" of, otherwise create a cast error that wraps it" );
			return CastError::Create( env, *newCtx );
		}

		WeakPtr<CombatContext> const combatCtx = rftype::virtual_ptr_cast<CombatContext>( WeakPtr<act::Context>( newCtx ) );
		if( combatCtx == nullptr )
		{
			// Non-combat context
			return CastError::Create( env, *newCtx );
		}

		// New combat context
		currentCombatCtx = combatCtx.Get();
	}

	// Finish the cast
	bool const finishSuccess = ctx.mCombatInstance.FinishCast( ctx.mSourceFighter );
	RF_ASSERT( finishSuccess );

	// Success!
	// NOTE: Caller-provided combat instance was left unmodified until this
	//  point, where it is now stomped
	RF_ASSERT( currentCombatCtx != nullptr );
	combatInstance = currentCombatCtx->mCombatInstance;
	return CastError::kNoError;
}

}
///////////////////////////////////////////////////////////////////////////////

CastingEngine::CastingEngine(
	WeakPtr<file::VFS const> vfs,
	WeakPtr<combat::CombatEngine const> const& combatEngine,
	WeakPtr<element::ElementDatabase const> const& elementDatabase )
	: mVfs( vfs )
	, mCombatEngine( combatEngine )
	, mElementDatabase( elementDatabase )
	, mActionDatabase( DefaultCreator<act::ActionDatabase>::Create() )
	, mConditionDatabase( DefaultCreator<act::ConditionDatabase>::Create() )
{
	//
}



CastingEngine::~CastingEngine() = default;



bool CastingEngine::LoadActionDefinitions(
	file::VFSPath const& actionDefinitionsDir )
{
	// Unload current actions
	mActionDatabase->RemoveAllActions();

	// Enumerate action definitions
	rftl::vector<file::VFSPath> actionFiles;
	{
		mVfs->EnumerateDirectoryRecursive(
			actionDefinitionsDir,
			file::VFSMount::Permissions::ReadOnly,
			actionFiles );
	}

	if( actionFiles.empty() )
	{
		RFLOG_NOTIFY( actionDefinitionsDir, RFCAT_CC3O3, "Failed to find any action definition files" );
		return false;
	}

	// Load all action definitions
	for( file::VFSPath const& actionFile : actionFiles )
	{
		// Will use the relative path from the directory as the key
		bool isBranch = false;
		file::VFSPath asBranch = actionFile.GetAsBranchOf( actionDefinitionsDir, isBranch );
		if( isBranch == false )
		{
			RFLOG_NOTIFY( actionFile, RFCAT_CC3O3, "Couldn't convert action definition file to relative path" );
			return false;
		}

		// Trim extensions, use that as key
		asBranch.RemoveTrailingExtensions();
		rftl::string key = asBranch.CreateString();

		// Load
		bool const loadResult = LoadActionDefinition( actionFile, rftl::move( key ) );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( actionFile, RFCAT_CC3O3, "Couldn't load action definition file" );
			return false;
		}
	}

	return true;
}



UniquePtr<CastError> CastingEngine::ExecuteElementCast(
	combat::CombatInstance& combatInstance,
	combat::FighterID const& source,
	combat::FighterID const& target,
	element::ElementIdentifier identifier,
	element::ElementLevel castedLevel ) const
{
	static constexpr char kPrefix[] = "elements/";
	static constexpr size_t kPrefixSize = rftl::extent<decltype( kPrefix )>::value - sizeof( '\0' );

	element::ElementString const asString = element::GetElementString( identifier );

	rftl::static_string<kPrefixSize + element::ElementString::fixed_capacity> key = {};
	key = kPrefix;
	key += asString;

	return ExecuteRawCast(
		combatInstance,
		source,
		target,
		key,
		castedLevel );
}




UniquePtr<CastError> CastingEngine::ExecuteRawCast(
	combat::CombatInstance& combatInstance,
	combat::FighterID const& source,
	combat::FighterID const& target,
	rftl::string_view const& key,
	element::ElementLevel castedLevel ) const
{
	act::Environment env = {};
	env.mActionDatabase = mActionDatabase;
	env.mConditionDatabase = nullptr;

	return details::ExecuteCast(
		rftl::move( env ),
		combatInstance,
		source,
		target,
		key,
		castedLevel );
}

///////////////////////////////////////////////////////////////////////////////

bool CastingEngine::LoadActionDefinition(
	file::VFSPath const& actionDefinitionPath,
	rftl::string&& key )
{
	act::ActionDatabase& actionDatabase = *mActionDatabase;

	// Load
	UniquePtr<act::ActionRecord> action =
		resource::LoadFromFile<act::ActionRecord>(
			actionDefinitionPath );
	if( action == nullptr )
	{
		RFLOG_NOTIFY( actionDefinitionPath, RFCAT_CC3O3, "Failed to load action from file" );
		return false;
	}

	// Validate
	if( action->GetRoot() == nullptr )
	{
		RFLOG_NOTIFY( actionDefinitionPath, RFCAT_CC3O3, "Action has no root step" );
		return false;
	}

	// Store
	bool const success = actionDatabase.AddAction(
		rftl::move( key ),
		rftl::move( action ) );
	if( success == false )
	{
		RFLOG_NOTIFY( actionDefinitionPath, RFCAT_CC3O3, "Failed to action to database" );
		return false;
	}

	RFLOG_DEBUG( actionDefinitionPath, RFCAT_CC3O3, "Added action to database" );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

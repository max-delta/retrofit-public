#include "stdafx.h"
#include "CastingEngine.h"

#include "cc3o3/casting/CastError.h"
#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "GameAction/ActionDatabase.h"
#include "GameAction/ActionRecord.h"
#include "GameAction/ConditionDatabase.h"
#include "GameAction/Context.h"
#include "GameAction/Environment.h"
#include "GameAction/Step.h"

#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////
namespace details {

UniquePtr<CastError> ExecuteCast(
	act::Environment&& env,
	combat::CombatInstance& combatInstance,
	combat::FighterID const& source,
	combat::FighterID const& target,
	rftl::string_view const& key )
{
	// Prepare context
	RF_TODO_ANNOTATION( "Store the combat instance, target, etc in here" );
	struct HACKContext : public act::Context
	{
		virtual UniquePtr<Context> Clone() const override
		{
			return nullptr;
		};
	};
	HACKContext ctx = {};

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

	// Get root step
	WeakPtr<act::Step const> rootPtr = action.GetRoot();
	RF_ASSERT( rootPtr );
	act::Step const& root = *rootPtr;

	// Execute the action
	UniquePtr<act::Context> newCtx = root.Execute( env, ctx );
	if( newCtx != nullptr )
	{
		if( newCtx->IsATerminalError() )
		{
			RF_TODO_BREAK_MSG(
				"Check if it's an error ctx that we can pull a cast error out"
				" of, otherwise create a cast error that wraps it" );
			return CastError::Create( env, ctx );
		}
	}

	// Success!
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
	element::ElementIdentifier identifier ) const
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
		key );
}




UniquePtr<CastError> CastingEngine::ExecuteRawCast(
	combat::CombatInstance& combatInstance,
	combat::FighterID const& source,
	combat::FighterID const& target,
	rftl::string_view const& key ) const
{
	act::Environment env = {};
	env.mActionDatabase = mActionDatabase;
	env.mConditionDatabase = nullptr;

	return details::ExecuteCast(
		rftl::move( env ),
		combatInstance,
		source,
		target,
		key );
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

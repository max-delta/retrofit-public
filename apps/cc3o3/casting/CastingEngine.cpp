#include "stdafx.h"
#include "CastingEngine.h"

#include "cc3o3/elements/IdentifierUtils.h"
#include "cc3o3/resource/ResourceLoad.h"

#include "PlatformFilesystem/VFS.h"


namespace RF::cc::cast {
///////////////////////////////////////////////////////////////////////////////

CastingEngine::CastingEngine(
	WeakPtr<file::VFS const> vfs,
	WeakPtr<combat::CombatEngine const> const& combatEngine,
	WeakPtr<element::ElementDatabase const> const& elementDatabase )
	: mVfs( vfs )
	, mCombatEngine( combatEngine )
	, mElementDatabase( elementDatabase )
{
	//
}



bool CastingEngine::LoadActionDefinitions(
	file::VFSPath const& actionDefinitionsDir )
{
	// Unload current actions
	mActionDatabase.RemoveAllActions();

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



WeakPtr<act::ActionRecord const> CastingEngine::GetElementActionDefinifion(
	element::ElementIdentifier identifier ) const
{
	static constexpr char kPrefix[] = "elements/";
	static constexpr size_t kPrefixSize = rftl::extent<decltype( kPrefix )>::value - sizeof( '\0' );

	element::ElementString const asString = element::GetElementString( identifier );

	rftl::static_string<kPrefixSize + element::ElementString::fixed_capacity> buffer = {};
	buffer = kPrefix;
	buffer += asString;

	return GetRawActionDefinifion( buffer );
}



WeakPtr<act::ActionRecord const> CastingEngine::GetRawActionDefinifion(
	rftl::string_view const& key ) const
{
	WeakPtr<act::ActionRecord const> retVal = mActionDatabase.GetAction( key );
	if( retVal == nullptr )
	{
		RFLOG_ERROR( key, RFCAT_CC3O3, "Couldn't fetch action definition from key" );
		return nullptr;
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

bool CastingEngine::LoadActionDefinition(
	file::VFSPath const& actionDefinitionPath,
	rftl::string&& key )
{
	// Load
	UniquePtr<act::ActionRecord> action =
		resource::LoadFromFile<act::ActionRecord>(
			actionDefinitionPath );
	if( action == nullptr )
	{
		RFLOG_NOTIFY( actionDefinitionPath, RFCAT_CC3O3, "Failed to load action from file" );
		return false;
	}

	// Store
	bool const success = mActionDatabase.AddAction(
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

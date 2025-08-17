#include "stdafx.h"
#include "ElementDatabase.h"

#include "cc3o3/elements/ElementDesc.h"
#include "cc3o3/elements/IdentifierUtils.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "core_math/math_casts.h"

#include "rftl/sstream"
#include "rftl/algorithm"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

ElementDatabase::ElementDatabase( WeakPtr<file::VFS const> vfs )
	: mVfs( vfs )
{
	//
}



bool ElementDatabase::LoadDescTables( file::VFSPath const& descTablesDir )
{
	// Unload current descs
	mElementDescs.clear();

	// Enumerate desc tables
	rftl::vector<file::VFSPath> descTableFiles;
	{
		rftl::vector<file::VFSPath> unexpectedDirectories;
		mVfs->EnumerateDirectory(
			descTablesDir,
			file::VFSMount::Permissions::ReadOnly,
			descTableFiles,
			unexpectedDirectories );
		for( file::VFSPath const& unexpectedDirectory : unexpectedDirectories )
		{
			RFLOG_WARNING( unexpectedDirectory, RFCAT_CC3O3, "Unexpected directory found alongside element desc table files" );
		}
	}

	if( descTableFiles.empty() )
	{
		RFLOG_NOTIFY( descTablesDir, RFCAT_CC3O3, "Failed to find any element desc table files" );
		return false;
	}

	// Load all desc tables
	for( file::VFSPath const& descTableFile : descTableFiles )
	{
		bool const loadResult = LoadDescTable( descTableFile );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( descTableFile, RFCAT_CC3O3, "Couldn't load element desc table file" );
			return false;
		}
	}

	return true;
}



bool ElementDatabase::LoadTierUnlockTables( file::VFSPath const& tierUnlockTablesDir )
{
	// Unload current tier unlocks
	mTierUnlocks.clear();

	// Enumerate tier unlock tables
	rftl::vector<file::VFSPath> iterUnlockTableFiles;
	{
		rftl::vector<file::VFSPath> unexpectedDirectories;
		mVfs->EnumerateDirectory(
			tierUnlockTablesDir,
			file::VFSMount::Permissions::ReadOnly,
			iterUnlockTableFiles,
			unexpectedDirectories );
		for( file::VFSPath const& unexpectedDirectory : unexpectedDirectories )
		{
			RFLOG_WARNING( unexpectedDirectory, RFCAT_CC3O3, "Unexpected directory found alongside tier unlock table files" );
		}
	}

	if( iterUnlockTableFiles.empty() )
	{
		RFLOG_NOTIFY( tierUnlockTablesDir, RFCAT_CC3O3, "Failed to find any tier unlock table files" );
		return false;
	}

	// Load all tier unlock tables
	for( file::VFSPath const& tierUnlockTableFile : iterUnlockTableFiles )
	{
		bool const loadResult = LoadTierUnlockTable( tierUnlockTableFile );
		if( loadResult == false )
		{
			RFLOG_NOTIFY( tierUnlockTableFile, RFCAT_CC3O3, "Couldn't load tier unlock table file" );
			return false;
		}
	}

	return true;
}



ElementDatabase::ElementIdentifiers ElementDatabase::GetAllElementIdentifiers() const
{
	ElementIdentifiers retVal;
	retVal.reserve( mElementDescs.size() );
	for( ElementDesc const& desc : mElementDescs )
	{
		retVal.emplace_back( desc.mIdentifier );
	}
	return retVal;
}



ElementDesc ElementDatabase::GetElementDesc( ElementIdentifier identifier ) const
{
	ElementDesc asDesc = {};
	asDesc.mIdentifier = identifier;

	ElementDescs::const_iterator const iter = rftl::find(
		mElementDescs.begin(),
		mElementDescs.end(),
		asDesc );
	if( iter == mElementDescs.end() )
	{
		return ElementDesc{};
	}

	return *iter;
}



ElementDatabase::ElementCounts ElementDatabase::GetElementsBasedOnTier( company::StoryTier tier ) const
{
	RF_ASSERT( tier != company::kInvalidStoryTier );

	ElementCounts retVal;

	for( TierUnlockLookup::value_type const& pair : mTierUnlocks )
	{
		ElementIdentifier const& identifier = pair.first;
		TierUnlocks const& unlocks = pair.second;

		size_t const count = unlocks.at( tier - 1u );
		if( count > 0 )
		{
			retVal[identifier] = count;
		}
	}

	return retVal;
}



rftl::deque<rftl::deque<rftl::string>> ElementDatabase::LoadCSV( file::VFSPath const& path )
{
	file::FileHandlePtr const handle = mVfs->GetFileForRead( path );
	if( handle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get file for read" );
		return {};
	}

	file::FileBuffer const buffer{ *handle.Get(), false };
	if( buffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to get data from file buffer" );
		return {};
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( buffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_CC3O3, "Failed to read file as csv" );
		return {};
	}

	return csv;
}



bool ElementDatabase::LoadDescTable( file::VFSPath const& descTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> descTable = LoadCSV( descTablePath );
	if( descTable.empty() )
	{
		RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Failed to load element desc table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = descTable.front();
	descTable.pop_front();
	if(
		header.size() != 8 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "innate" ||
		header.at( 2 ) != "base" ||
		header.at( 3 ) != "min" ||
		header.at( 4 ) != "max" ||
		header.at( 5 ) != "stack" ||
		header.at( 6 ) != "chain" ||
		header.at( 7 ) != "remark" )
	{
		RFLOG_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc table header" );
		return false;
	}
	line++;

	while( descTable.empty() == false )
	{
		rftl::deque<rftl::string> const entry = descTable.front();
		if( entry.size() != 8 )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, expected 8 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		rftl::string const& innate = entry.at( 1 );
		rftl::string const& baseLevel = entry.at( 2 );
		rftl::string const& minLevel = entry.at( 3 );
		rftl::string const& maxLevel = entry.at( 4 );
		rftl::string const& stack = entry.at( 5 );
		rftl::string const& chain = entry.at( 6 );

		static constexpr auto convertLevel = []( rftl::string const& in ) -> ElementLevel
		{
			uint16_t stringstreamIsTerrible = 0;
			( rftl::stringstream() << in ) >> stringstreamIsTerrible;
			return math::integer_cast<ElementLevel>( stringstreamIsTerrible );
		};

		ElementDesc desc = {};

		if( id.empty() || id.size() > sizeof( ElementIdentifier ) )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad id", math::integer_cast<int>( line ) );
			return false;
		}
		desc.mIdentifier = MakeElementIdentifier( id );

		if( GetElementDesc( desc.mIdentifier ).mIdentifier != kInvalidElementIdentifier )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Duplicate element desc identifier {}", GetElementString( desc.mIdentifier ) );
			return false;
		}

		if( innate.empty() || innate.size() > sizeof( InnateIdentifier ) )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad innate", math::integer_cast<int>( line ) );
			return false;
		}
		desc.mInnate = MakeInnateIdentifier( innate );

		desc.mBaseLevel = convertLevel( baseLevel );
		if( desc.mBaseLevel == 0 )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad base level", math::integer_cast<int>( line ) );
			return false;
		}

		desc.mMinLevel = convertLevel( minLevel );
		if( desc.mMinLevel == 0 )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad min level", math::integer_cast<int>( line ) );
			return false;
		}

		desc.mMaxLevel = convertLevel( maxLevel );
		if( desc.mMaxLevel == 0 )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad max level", math::integer_cast<int>( line ) );
			return false;
		}

		( rftl::stringstream() << stack ) >> desc.mStackSize;
		if( desc.mStackSize > 99 )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad stack size", math::integer_cast<int>( line ) );
			return false;
		}

		if( chain != "Y" && chain != "N" )
		{
			RFLOGF_NOTIFY( descTablePath, RFCAT_CC3O3, "Malformed element desc entry at line {}, bad chain", math::integer_cast<int>( line ) );
			return false;
		}
		desc.mChain = chain == "Y" ? true : false;

		mElementDescs.emplace_back( rftl::move( desc ) );

		descTable.pop_front();
		line++;
	}

	return true;
}



bool ElementDatabase::LoadTierUnlockTable( file::VFSPath const& tierUnlockTablePath )
{
	rftl::deque<rftl::deque<rftl::string>> csv = LoadCSV( tierUnlockTablePath );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( tierUnlockTablePath, RFCAT_CC3O3, "Failed to load tier unlock table file" );
		return false;
	}

	size_t line = 0;

	rftl::deque<rftl::string> const header = csv.front();
	csv.pop_front();
	if(
		header.size() != 9 ||
		header.at( 0 ) != "id" ||
		header.at( 1 ) != "1" ||
		header.at( 2 ) != "2" ||
		header.at( 3 ) != "3" ||
		header.at( 4 ) != "4" ||
		header.at( 5 ) != "5" ||
		header.at( 6 ) != "6" ||
		header.at( 7 ) != "7" ||
		header.at( 8 ) != "8" )
	{
		RFLOG_NOTIFY( tierUnlockTablePath, RFCAT_CC3O3, "Malformed tier unlock table header" );
		return false;
	}
	line++;

	while( csv.empty() == false )
	{
		rftl::deque<rftl::string> const entry = csv.front();
		if( entry.size() != 9 )
		{
			RFLOGF_NOTIFY( tierUnlockTablePath, RFCAT_CC3O3, "Malformed tier unlock entry at line {}, expected 9 columns", math::integer_cast<int>( line ) );
			return false;
		}
		rftl::string const& id = entry.at( 0 );
		static constexpr size_t kTierColFirst = 1;
		static constexpr size_t kTierColLast = 8;

		ElementIdentifier identifier = {};
		TierUnlocks unlocks = {};

		if( id.empty() || id.size() > sizeof( ElementIdentifier ) )
		{
			RFLOGF_NOTIFY( tierUnlockTablePath, RFCAT_CC3O3, "Malformed tier unlock entry at line {}, bad id", math::integer_cast<int>( line ) );
			return false;
		}
		identifier = MakeElementIdentifier( id );

		if( mTierUnlocks.count( identifier ) != 0 )
		{
			RFLOGF_NOTIFY( tierUnlockTablePath, RFCAT_CC3O3, "Duplicate tier unlock identifier {}", GetElementString( identifier ) );
			return false;
		}

		bool hasUnlocks = false;
		for( size_t col = kTierColFirst; col <= kTierColLast; col++ )
		{
			size_t const tier = col - kTierColFirst;
			size_t& unlock = unlocks.at( tier );
			( rftl::stringstream() << entry.at( col ) ) >> unlock;
			if( unlock > 0 )
			{
				hasUnlocks = true;
			}
		}

		if( hasUnlocks )
		{
			mTierUnlocks[identifier] = rftl::move( unlocks );
		}
		else
		{
			// Don't bother tracking things with no unlocks
			// TODO: Clean up data long-term and make this an error case?
		}

		csv.pop_front();
		line++;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

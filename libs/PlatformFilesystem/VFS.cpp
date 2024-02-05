#include "stdafx.h"
#include "VFS.h"

#include "PlatformFilesystem/FileHandle.h"
#include "Logging/Logging.h"

#include "core/macros.h"
#include "core/ptr/default_creator.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "rftl/algorithm"
#include "rftl/ios"
#include "rftl/filesystem"
#include "rftl/fstream"
#include "rftl/system_error"
#include "rftl/unordered_set"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

VFSPath const VFS::kRoot = VFSPath( "RF:" );
VFSPath const VFS::kInvalid = VFSPath( "INVALID:" );
VFSPath const VFS::kEmpty = VFSPath();

///////////////////////////////////////////////////////////////////////////////
namespace details {

static rftl::ios_base::openmode GetOpenModeBitFromEnumValue( OpenFlags openFlags )
{
	switch( openFlags )
	{
		case OpenFlags::Read:
			return rftl::ios_base::binary | rftl::ios_base::in;
		case OpenFlags::Write:
			return rftl::ios_base::binary | rftl::ios_base::in | rftl::ios_base::out | rftl::ios_base::trunc;
		case OpenFlags::Modify:
			return rftl::ios_base::binary | rftl::ios_base::in | rftl::ios_base::out;
		case OpenFlags::Append:
			return rftl::ios_base::binary | rftl::ios_base::in | rftl::ios_base::out | rftl::ios_base::app;
		case OpenFlags::Execute:
			return rftl::ios_base::binary | rftl::ios_base::in;
		case OpenFlags::Invalid:
		default:
			RF_DBGFAIL();
			return 0;
	}
}

}
///////////////////////////////////////////////////////////////////////////////

VFS::VFS()
{
	//
}



VFS::~VFS() = default;



FileHandlePtr VFS::GetFileForRead( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File read request" );
	return OpenFile( path, VFSMount::Permissions::ReadOnly, OpenFlags::Read, true );
}



FileHandlePtr VFS::GetFileForWrite( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File write request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, OpenFlags::Write, false );
}



FileHandlePtr VFS::GetFileForModify( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File modify request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, OpenFlags::Modify, true );
}



FileHandlePtr VFS::GetFileForAppend( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File append request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, OpenFlags::Append, false );
}



FileHandlePtr VFS::GetFileForExecute( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File execute request" );
	return OpenFile( path, VFSMount::Permissions::ReadExecute, OpenFlags::Execute, true );
}



FileHandlePtr VFS::GetRawFileForWrite( char const* rawPath ) const
{
	RFLOG_INFO( rawPath, RFCAT_VFS, "File write request" );
	rftl::filebuf rawFile = {};
	rftl::filebuf const* const errCheck = rawFile.open( rawPath,
		details::GetOpenModeBitFromEnumValue( OpenFlags::Write ) );
	if( errCheck == nullptr || rawFile.is_open() == false )
	{
		int32_t const assumedErr = math::integer_cast<int32_t>( errno );
		RFLOG_ERROR( rawPath, RFCAT_VFS, "Failed to open file for raw access, last error code was %i", assumedErr );
		return nullptr;
	}

	return DefaultCreator<FileHandle>::Create( DefaultCreator<rftl::filebuf>::Create( rftl::move( rawFile ) ) );
}



void VFS::EnumerateDirectory(
	VFSPath const& directory,
	VFSMount::Permissions permissions,
	rftl::vector<VFSPath>& files,
	rftl::vector<VFSPath>& folders ) const
{
	files.clear();
	folders.clear();

	// There may be some overlap across layers, but we only want to retun
	//  unique results
	rftl::unordered_set<VFSPath> tempFiles;
	rftl::unordered_set<VFSPath> tempFolders;

	// Chroot for basic safety and sanitization
	VFSPath const path = ChrootCollapse( directory );
	if( path.Empty() )
	{
		RFLOG_ERROR( directory, RFCAT_VFS, "Failed to chroot path" );
		return;
	}

	// Evaluate each mount point in order
	for( VFSMount const& mount : mMountTable )
	{
		// Attempt mount
		rftl::string const finalFilename = AttemptMountMapping( mount, path, permissions );
		if( finalFilename.empty() )
		{
			// Cannot mount here
			continue;
		}

		bool const exists = rftl::filesystem::exists( finalFilename );
		if( exists == false )
		{
			// Not here, maybe it's in an overlapping mount point
			continue;
		}

		bool const isDir = rftl::filesystem::is_directory( finalFilename );
		if( isDir == false )
		{
			// Not a directory in this mount point
			RFLOG_WARNING( path, RFCAT_VFS, "Looking for directory, found file: %s", finalFilename.c_str() );
			continue;
		}

		// Iterate the directory
		rftl::filesystem::directory_iterator dir( finalFilename );
		for( rftl::filesystem::directory_entry const& entry : dir )
		{
			rftl::string const entryPath = entry.path().generic_string();
			VFSPath reverseMapping = AttemptMapToVFS( entryPath, permissions );
			if( entry.is_directory() )
			{
				tempFolders.emplace( rftl::move( reverseMapping ) );
			}
			else if( entry.is_regular_file() )
			{
				tempFiles.emplace( rftl::move( reverseMapping ) );
			}
			else
			{
				RFLOG_WARNING( path, RFCAT_VFS, "Enumerating directory, found unknown object: %s", entryPath.c_str() );
			}
		}
	}

	// Return unique results
	for( VFSPath const& file : tempFiles )
	{
		files.emplace_back( file );
	}
	for( VFSPath const& folder : tempFolders )
	{
		folders.emplace_back( folder );
	}
}



void VFS::EnumerateDirectoryRecursive(
	VFSPath const& directory,
	VFSMount::Permissions permissions,
	rftl::vector<VFSPath>& files ) const
{
	files.clear();

	// Initial crawl
	rftl::vector<VFSPath> folders;
	EnumerateDirectory( directory, file::VFSMount::Permissions::ReadOnly, files, folders );

	// Recursion
	size_t recursionCount = 0;
	static constexpr size_t kRecursionWarningCount = 1'000;
	static constexpr size_t kRecursionErrorCount = 10'000;
	while( folders.empty() == false )
	{
		recursionCount++;
		if( recursionCount == kRecursionWarningCount )
		{
			RFLOG_WARNING( directory, RFCAT_VFS, "Large number of folders recursed into, may indicate infinite loop" );
		}
		if( recursionCount >= kRecursionErrorCount )
		{
			RFLOG_NOTIFY( directory, RFCAT_VFS, "Massive number of folders recursed into, assumed to be infinite loop" );
			files.clear();
			return;
		}

		// Pop front
		file::VFSPath const curFolder = folders.front();
		folders.erase( folders.begin() );

		// Crawl
		rftl::vector<file::VFSPath> newFiles;
		rftl::vector<file::VFSPath> newFolders;
		EnumerateDirectory( curFolder, file::VFSMount::Permissions::ReadOnly, newFiles, newFolders );

		// Merge
		newFiles.reserve( newFiles.size() + files.size() );
		for( file::VFSPath& file : newFiles )
		{
			files.emplace_back( rftl::move( file ) );
		}
		newFolders.reserve( newFolders.size() + folders.size() );
		for( file::VFSPath& folder : newFolders )
		{
			folders.emplace_back( rftl::move( folder ) );
		}
	}
}



bool VFS::AttemptInitialMount( MountPriority priority, rftl::string const& mountTableFile, rftl::string const& userDirectory )
{
	RF_ASSERT( mountTableFile.empty() == false );
	RF_ASSERT( userDirectory.empty() == false );

	rftl::string absoluteSearchDirectory = rftl::filesystem::absolute( "." ).generic_string();
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file search directory: %s", absoluteSearchDirectory.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file param: %s", mountTableFile.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "User directory param: %s", userDirectory.c_str() );

	rftl::string absoluteMountTableFilename = rftl::filesystem::absolute( mountTableFile ).generic_string();
	mMountTableFile = CollapsePath( VFSPath::CreatePathFromString( absoluteMountTableFilename ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "Mount table file: %s", mMountTableFile.CreateString().c_str() );

	mConfigDirectory = mMountTableFile.GetParent();
	RFLOG_INFO( nullptr, RFCAT_VFS, "Config directory: %s", mConfigDirectory.CreateString().c_str() );

	rftl::string absoluteUserDirectory = rftl::filesystem::absolute( userDirectory ).generic_string();
	mUserDirectory = CollapsePath( VFSPath::CreatePathFromString( absoluteUserDirectory ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "User directory: %s", mUserDirectory.CreateString().c_str() );

	if( rftl::filesystem::exists( absoluteMountTableFilename ) == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to find mount table file" );
		return false;
	}

	if( rftl::filesystem::exists( absoluteUserDirectory ) == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to find user directory" );
		return false;
	}

	rftl::filebuf rawFile = {};
	rftl::string collapsedMountFilename = mMountTableFile.CreateString();
	rftl::filebuf const* const errCheck = rawFile.open( collapsedMountFilename,
		details::GetOpenModeBitFromEnumValue( OpenFlags::Read ) );
	if( errCheck == nullptr || rawFile.is_open() == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to open mount table file" );
		return false;
	}
	FileHandle fileHandle( DefaultCreator<rftl::filebuf>::Create( rftl::move( rawFile ) ) );
	RF_ASSERT( rawFile.is_open() == false );

	return ProcessMountFile( priority, fileHandle );
}



bool VFS::AttemptSubsequentMount( MountPriority priority, VFSPath const& mountTableFile )
{
	RFLOG_INFO( nullptr, RFCAT_VFS, "Subsequent mount table file: %s", mountTableFile.CreateString().c_str() );
	FileHandlePtr const filePtr = GetFileForRead( mountTableFile );
	if( filePtr == nullptr )
	{
		RFLOG_ERROR( mountTableFile, RFCAT_VFS, "Failed to open mount table file" );
		return false;
	}
	return ProcessMountFile( priority, *filePtr );
}



bool VFS::AttemptPassthroughMount()
{
	RF_ASSERT( mMountTable.empty() );

	VFSMount mountRule = {};
	mountRule.mPriority = kMountPriorityHighest;
	mountRule.mType = VFSMount::Type::Absolute;
	mountRule.mPermissions = VFSMount::Permissions::ReadOnly;
	mountRule.mVirtualPath = kRoot;
	mountRule.mRealMount = {};
	mMountTable.emplace_back( rftl::move( mountRule ) );

	DebugDumpMountTable();
	return true;
}



VFSPath VFS::AttemptMapToVFS( rftl::string const& physicalPath, VFSMount::Permissions desiredPermissions ) const
{
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mapping request: <%i> %s", desiredPermissions, physicalPath.c_str() );

	VFSPath const physicalAsVFS = VFSPath::CreatePathFromString( physicalPath );

	for( VFSMount const& mount : mMountTable )
	{
		// NOTE: Technically faster to check permissions before path, but
		//  less easy to debug, and not performance-critical at time of writing

		VFSPath const* physRoot;
		switch( mount.mType )
		{
			case VFSMount::Type::Absolute:
				physRoot = &kEmpty;
				break;
			case VFSMount::Type::ConfigRelative:
				physRoot = &mConfigDirectory;
				break;
			case VFSMount::Type::UserRelative:
				physRoot = &mUserDirectory;
				break;
			case VFSMount::Type::Invalid:
			default:
				RFLOG_FATAL( nullptr, RFCAT_VFS, "Unhandled mount type" );
		}
		VFSPath const realMount = CollapsePath( physRoot->GetChild( mount.mRealMount ) );
		bool const isDescendent = physicalAsVFS.IsDescendantOf( realMount );
		if( isDescendent == false )
		{
			// Unrelated path
			continue;
		}

		uint8_t const applicablePermissions = math::integer_cast<uint8_t>( static_cast<uint8_t>( mount.mPermissions ) & static_cast<uint8_t>( desiredPermissions ) );
		if( applicablePermissions != static_cast<uint8_t>( desiredPermissions ) )
		{
			// Not all permissions fulfilled
			continue;
		}

		// Match! Calculate branch
		bool isBranch;
		VFSPath const branchFromMount = physicalAsVFS.GetAsBranchOf( realMount, isBranch );
		RF_ASSERT( isBranch );
		return mount.mVirtualPath.GetChild( branchFromMount );
	}

	return VFSPath();
}



void VFS::DebugDumpMountTable() const
{
	RFLOG_INFO( nullptr, RFCAT_VFS, "Mount table:" );
	for( VFSMount const& mountRule : mMountTable )
	{
		char const* type = nullptr;
		switch( mountRule.mType )
		{
			case VFSMount::Type::Absolute:
				type = "Absolute      ";
				break;
			case VFSMount::Type::ConfigRelative:
				type = "ConfigRelative";
				break;
			case VFSMount::Type::UserRelative:
				type = "UserRelative  ";
				break;
			case VFSMount::Type::Invalid:
			default:
				type = "INVALID";
				break;
		}
		char const* permissions = nullptr;
		switch( mountRule.mPermissions )
		{
			case VFSMount::Permissions::ReadOnly:
				permissions = "Read   ";
				break;
			case VFSMount::Permissions::ReadWrite:
				permissions = "Write  ";
				break;
			case VFSMount::Permissions::ReadExecute:
				permissions = "Execute";
				break;
			case VFSMount::Permissions::Invalid:
			default:
				permissions = "INVALID";
				break;
		}
		RFLOG_INFO(
			nullptr,
			RFCAT_VFS,
			"  % 3u %s %s \"%s\" \"%s\"",
			mountRule.mPriority,
			type,
			permissions,
			mountRule.mVirtualPath.CreateString().c_str(),
			mountRule.mRealMount.CreateString().c_str() );
	}
}

///////////////////////////////////////////////////////////////////////////////

VFSPath VFS::CollapsePath( VFSPath const& path )
{
	VFSPath retVal;
	for( VFSPath::Element const& element : path )
	{
		// Ascencion
		if( element == kPathAscensionElement )
		{
			if( retVal.NumElements() == 0 )
			{
				RFLOG_ERROR( path, RFCAT_VFS, "Excessive ascencions found during collapse" );
				return kInvalid.GetChild( path );
			}

			retVal.GoUp();
			continue;
		}

		// Current
		if( element == kPathCurrentElement )
		{
			// Do nothing
			continue;
		}

		// Descension
		retVal.Append( element );
	}

	return retVal;
}



VFSPath VFS::ChrootCollapse( VFSPath const& path )
{
	// Collapse path first, make sure they aren't trying to trivially jump out
	//  of mount points
	// NOTE: Not actually secure
	VFSPath const collapsedPath = CollapsePath( path );
	if( collapsedPath.IsDescendantOf( kRoot ) == false )
	{
		RFLOG_ERROR( collapsedPath, RFCAT_VFS, "Virtual path doesn't descend from root" );
		return VFSPath();
	}

	return collapsedPath;
}



bool VFS::ProcessMountFile( MountPriority priority, FileHandle& file )
{
	rftl::string tokenBuilder;
	rftl::vector<rftl::string> tokenStream;
	enum class ReadMode : uint8_t
	{
		k_HuntingForToken,
		k_DiscardingComment,
		k_ReadingToken,
	} curReadMode = ReadMode::k_HuntingForToken;
	int readCode = 0;
	while( ( readCode = file.GetChar() ) != EOF )
	{
		// Reading chars
		char const ch = math::integer_cast<char>( readCode );

		if( curReadMode == ReadMode::k_HuntingForToken )
		{
			// Hunting for a token

			// Is it just filler?
			bool isIgnorable = false;
			for( char const& ignoreCh : VFSMountTableTokens::kMountFillerIgnores )
			{
				if( ch == ignoreCh )
				{
					isIgnorable = true;
				}
			}
			if( isIgnorable )
			{
				// Filler, ignore
				continue;
			}

			if( ch == VFSMountTableTokens::kMountTokenAffix[0] )
			{
				// Token, start reading
				curReadMode = ReadMode::k_ReadingToken;
				if( tokenBuilder.empty() == false )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Internal parser failure" );
					return false;
				}
			}
			else if( ch == VFSMountTableTokens::kMountCommentPrefix )
			{
				// Comment, start tossing
				curReadMode = ReadMode::k_DiscardingComment;
			}
			else
			{
				RFLOG_ERROR( nullptr, RFCAT_VFS, "Unknown character encountered when searching for token" );
				return false;
			}
		}

		if( curReadMode == ReadMode::k_DiscardingComment )
		{
			// Discarding comment

			if( ch == VFSMountTableTokens::kMountCommentSuffix )
			{
				// Done with comment, back to token searching
				curReadMode = ReadMode::k_HuntingForToken;
				continue;
			}
			else
			{
				// Still in comment, keep discarding
				continue;
			}
		}

		if( curReadMode == ReadMode::k_ReadingToken )
		{
			// Reading token

			tokenBuilder.push_back( ch );
			RF_ASSERT( tokenBuilder[0] == VFSMountTableTokens::kMountTokenAffix[0] );

			// Check integrity of initial tokens
			static_assert( sizeof( VFSMountTableTokens::kMountTokenAffix ) == 2, "Token len changed" );
			if( tokenBuilder.size() == 1 )
			{
				if( tokenBuilder[0] != VFSMountTableTokens::kMountTokenAffix[0] )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Malformed token prefix at start" );
					return false;
				}
				continue;
			}
			else if( tokenBuilder.size() == 2 )
			{
				if( tokenBuilder[1] != VFSMountTableTokens::kMountTokenAffix[1] )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Malformed token prefix at start" );
					return false;
				}
				continue;
			}

			// Check for possible termination
			if( tokenBuilder.size() >= 4 && ch == VFSMountTableTokens::kMountTokenAffix[1] )
			{
				RF_ASSERT( tokenBuilder.size() > sizeof( VFSMountTableTokens::kMountTokenAffix ) );
				char const& previousToken = *( tokenBuilder.rbegin() + 1 );
				if( previousToken == VFSMountTableTokens::kMountTokenAffix[0] )
				{
					// Termination! Save off the token
					tokenStream.emplace_back( rftl::move( tokenBuilder ) );
					RF_ASSERT( tokenBuilder.empty() );

					RF_ASSERT( tokenBuilder.size() <= VFSMountTableTokens::kNumColumns );
					if( tokenStream.size() == VFSMountTableTokens::kNumColumns )
					{
						// Form mount rule from tokens
						VFSMount mountRule = ProcessMountRule(
							priority,
							tokenStream[0],
							tokenStream[1],
							tokenStream[2],
							tokenStream[3] );
						if(
							mountRule.mType == VFSMount::Type::Invalid ||
							mountRule.mPermissions == VFSMount::Permissions::Invalid ||
							mountRule.mVirtualPath.NumElements() == 0 )
						{
							RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to parse mount rule" );
							return false;
						}
						tokenStream.clear();

						mMountTable.emplace_back( rftl::move( mountRule ) );
					}

					// Keep collectiong tokens
					curReadMode = ReadMode::k_HuntingForToken;
				}
			}

			// Part of the token, keep processing
			continue;
		}

		RFLOG_ERROR( nullptr, RFCAT_VFS, "Internal parse logic failure" );
		return false;
	}

	if( curReadMode != ReadMode::k_HuntingForToken )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Unterminated token at file end" );
		mMountTable.clear();
		return false;
	}
	else if( tokenBuilder.empty() == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Unclean token buffer at file end" );
		mMountTable.clear();
		return false;
	}
	else if( tokenStream.empty() == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Unclean token stream at file end" );
		mMountTable.clear();
		return false;
	}

	DebugDumpMountTable();
	return true;
}



VFSMount VFS::ProcessMountRule( MountPriority priority, rftl::string const& type, rftl::string const& permissions, rftl::string const& virtualPoint, rftl::string const& realPoint )
{
	constexpr size_t prefixLen = sizeof( VFSMountTableTokens::kMountTokenAffix );
	constexpr size_t affixesLen = sizeof( VFSMountTableTokens::kMountTokenAffix ) * 2;
	RF_ASSERT( type.size() >= affixesLen );
	RF_ASSERT( permissions.size() >= affixesLen );
	RF_ASSERT( virtualPoint.size() >= affixesLen );
	RF_ASSERT( realPoint.size() >= affixesLen );
	VFSMount retVal = {};

	// Priority
	retVal.mPriority = priority;

	// Type
	rftl::string typeVal = type.substr( prefixLen, type.size() - affixesLen );
	if( typeVal == VFSMountTableTokens::kMountTokenAbsolute )
	{
		retVal.mType = VFSMount::Type::Absolute;
	}
	else if( typeVal == VFSMountTableTokens::kMountTokenConfigRelative )
	{
		retVal.mType = VFSMount::Type::ConfigRelative;
	}
	else if( typeVal == VFSMountTableTokens::kMountTokenUserRelative )
	{
		retVal.mType = VFSMount::Type::UserRelative;
	}
	else
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Invalid mount type" );
		retVal.mType = VFSMount::Type::Invalid;
		return retVal;
	}

	// Permissions
	rftl::string permissionsVal = permissions.substr( prefixLen, permissions.size() - affixesLen );
	if( permissionsVal == VFSMountTableTokens::kMountTokenReadOnly )
	{
		retVal.mPermissions = VFSMount::Permissions::ReadOnly;
	}
	else if( permissionsVal == VFSMountTableTokens::kMountTokenReadWrite )
	{
		retVal.mPermissions = VFSMount::Permissions::ReadWrite;
	}
	else if( permissionsVal == VFSMountTableTokens::kMountTokenReadExecute )
	{
		retVal.mPermissions = VFSMount::Permissions::ReadExecute;
	}
	else
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Invalid permissions" );
		retVal.mType = VFSMount::Type::Invalid;
		retVal.mPermissions = VFSMount::Permissions::Invalid;
		return retVal;
	}

	// Virtual
	rftl::string virtualVal = virtualPoint.substr( prefixLen, virtualPoint.size() - affixesLen );
	retVal.mVirtualPath = kRoot.GetChild( VFSPath::CreatePathFromString( virtualVal ) );
	size_t const numVirtualElements = retVal.mVirtualPath.NumElements();
	for( size_t i = 0; i < numVirtualElements; i++ )
	{
		VFSPath::Element const& element = retVal.mVirtualPath.GetElement( i );

		bool badPathElement = false;
		if( element.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Empty path element in virtual path, parser failure?" );
			badPathElement = true;
		}
		else if( element == kPathAscensionElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in virtual path" );
			badPathElement = true;
		}
		else if( element == kPathCurrentElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path current element in virtual path" );
			badPathElement = true;
		}

		if( badPathElement )
		{
			retVal.mVirtualPath.m_ElementList.clear();
			retVal.mType = VFSMount::Type::Invalid;
			retVal.mPermissions = VFSMount::Permissions::Invalid;
			return retVal;
		}
	}

	// Real
	rftl::string realVal = realPoint.substr( prefixLen, realPoint.size() - affixesLen );
	retVal.mRealMount = VFSPath::CreatePathFromString( realVal );
	bool hasEncounteredDescendToken = false;
	size_t const numRealElements = retVal.mRealMount.NumElements();
	for( size_t i = 0; i < numRealElements; i++ )
	{
		VFSPath::Element const& element = retVal.mRealMount.GetElement( i );

		bool badPathElement = false;
		if( element.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Empty path element in real path, parser failure?" );
			badPathElement = true;
		}
		else if(
			element == kPathAscensionElement &&
			( retVal.mType != VFSMount::Type::ConfigRelative &&
				retVal.mType != VFSMount::Type::UserRelative ) )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in non-relative real path" );
			badPathElement = true;
		}
		else if(
			element == kPathAscensionElement &&
			( retVal.mType == VFSMount::Type::ConfigRelative ||
				retVal.mType == VFSMount::Type::UserRelative ) &&
			hasEncounteredDescendToken )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in relative real path after descending" );
			badPathElement = true;
		}
		else if( element == kPathCurrentElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path current element in real path" );
			badPathElement = true;
		}

		if( badPathElement )
		{
			retVal.mVirtualPath.m_ElementList.clear();
			retVal.mRealMount.m_ElementList.clear();
			retVal.mType = VFSMount::Type::Invalid;
			retVal.mPermissions = VFSMount::Permissions::Invalid;
			return retVal;
		}

		// Normal elements are descensions
		if( element != kPathAscensionElement )
		{
			hasEncounteredDescendToken = true;
		}
	}

	return retVal;
}



VFSPath VFS::GetRealMountPoint( VFSMount const& mount ) const
{
	// The root is the base
	VFSPath const* physRoot;
	switch( mount.mType )
	{
		case VFSMount::Type::Absolute:
			physRoot = &kEmpty;
			break;
		case VFSMount::Type::ConfigRelative:
			physRoot = &mConfigDirectory;
			break;
		case VFSMount::Type::UserRelative:
			physRoot = &mUserDirectory;
			break;
		case VFSMount::Type::Invalid:
		default:
			RFLOG_FATAL( mount.mVirtualPath, RFCAT_VFS, "Unhandled mount type" );
	}
	VFSPath const realMountPoint = physRoot->GetChild( mount.mRealMount );
	return CollapsePath( realMountPoint );
}



rftl::string VFS::AttemptMountMapping( VFSMount const& mount, VFSPath const& collapsedPath, VFSMount::Permissions const& permissions ) const
{
	if( ( static_cast<int>( mount.mPermissions ) & static_cast<int>( permissions ) ) != static_cast<int>( permissions ) )
	{
		// Missing some permissions
		return rftl::string();
	}

	if( collapsedPath == mount.mVirtualPath )
	{
		// At mount, with no overlap
		return GetRealMountPoint( mount ).CreateString();
	}

	if( collapsedPath.IsDescendantOf( mount.mVirtualPath ) == false )
	{
		// Not under this mount point
		return rftl::string();
	}

	// The branch from the virtual point is the append
	bool isBranch = false;
	VFSPath const pathAsBranch = collapsedPath.GetAsBranchOf( mount.mVirtualPath, isBranch );
	RF_ASSERT_MSG( isBranch, "Descendant, but not branch? Logic error?" );

	// Final collapse and serialize
	VFSPath const realMountPoint = GetRealMountPoint( mount );
	VFSPath const physTarget = CollapsePath( realMountPoint.GetChild( pathAsBranch ) );
	RF_ASSERT_MSG( physTarget.IsDescendantOf( kInvalid ) == false, "How? Shouldn't have ascencions in anything pre-collapse" );
	rftl::string const filename = physTarget.CreateString();

	return filename;
}



FileHandlePtr VFS::OpenFile( VFSPath const& uncollapsedPath, VFSMount::Permissions const& permissions, OpenFlags openFlags, bool mustExist ) const
{
	// Chroot for basic safety and sanitization
	VFSPath const path = ChrootCollapse( uncollapsedPath );
	if( path.Empty() )
	{
		RFLOG_ERROR( uncollapsedPath, RFCAT_VFS, "Failed to chroot path" );
		return nullptr;
	}

	// Evaluate each mount point
	using PotentialMapping = rftl::pair<rftl::string, VFSMount const*>;
	rftl::vector<PotentialMapping> potentialMappings;
	for( VFSMount const& mount : mMountTable )
	{
		// Attempt mount
		rftl::string const potentialMapping = AttemptMountMapping( mount, path, permissions );
		if( potentialMapping.empty() )
		{
			// Cannot mount here
			continue;
		}

		if( mustExist )
		{
			bool const exists = rftl::filesystem::exists( potentialMapping );
			if( exists == false )
			{
				// Not here, maybe it's in an overlapping mount point
				continue;
			}
		}

		potentialMappings.emplace_back( potentialMapping, &mount );
	}
	if( potentialMappings.empty() )
	{
		// No valid mappings
		return nullptr;
	}

	// Take the lowest numbered priority
	rftl::stable_sort( potentialMappings.begin(), potentialMappings.end(),
		[]( PotentialMapping const& lhs, PotentialMapping const& rhs ) -> bool
		{
			return lhs.second->mPriority < rhs.second->mPriority;
		} );
	if( potentialMappings.size() > 1 )
	{
		RFLOG_TRACE( path, RFCAT_VFS, "Multiple potential mount points found, taking earliest rule from highest priority" );
	}
	PotentialMapping const& finalMapping = potentialMappings.front();
	rftl::string const finalFilename = finalMapping.first;
	VFSMount const& finalMount = *finalMapping.second;

	// Locked to this mount layer, let's see what happens!
	RFLOG_TRACE( path, RFCAT_VFS, "Open resolved to: %s", finalFilename.c_str() );
	bool parentsCreated = false;
	while( true )
	{
		rftl::filebuf rawFile = {};
		rftl::filebuf const* const errCheck = rawFile.open( finalFilename,
			details::GetOpenModeBitFromEnumValue( openFlags ) );
		if( errCheck == nullptr || rawFile.is_open() == false )
		{
			int32_t const assumedErr = math::integer_cast<int32_t>( errno );

			// Tough luck, no easy way to tell what went wrong
			if( mustExist )
			{
				RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file that was reported to exist, last error code was %i", assumedErr );
				return nullptr;
			}
			else
			{
				VFSPath const immediateParentPath = VFSPath::CreatePathFromString( finalFilename ).GetParent();
				if( rftl::filesystem::exists( immediateParentPath.CreateString() ) == false )
				{
					RFLOG_WARNING( path, RFCAT_VFS, "Failed to open file, perhaps parent is missing?" );
					if( parentsCreated == false )
					{
						// Try to create parents
						{
							// Must be a writeable mount point
							if( ( static_cast<uint8_t>( finalMount.mPermissions ) & VFSMount::kWriteBit ) == 0 )
							{
								RFLOG_WARNING( path, RFCAT_VFS, "Can't create parents, mount rule not writable" );
							}
							else
							{
								// Mount point must already exist
								VFSPath const realMountPoint = GetRealMountPoint( finalMount );
								if( rftl::filesystem::exists( realMountPoint.CreateString() ) == false )
								{
									RFLOG_WARNING( path, RFCAT_VFS, "Can't create parents, mount point doesn't exist" );
								}
								else
								{
									VFSPath const& deepestParent = realMountPoint;
									VFSPath const& shallowestParent = immediateParentPath;
									bool parentSanityCheck = false;
									VFSPath const route = shallowestParent.GetAsBranchOf( deepestParent, parentSanityCheck );
									RFLOG_TEST_AND_FATAL( parentSanityCheck, path, RFCAT_VFS, "Parent calculation for mount point failed internal sanity check. Likely a code bug." );
									RFLOG_TEST_AND_FATAL( route.Empty() == false, path, RFCAT_VFS, "Parent calculation for mount point failed internal route check. Likely a code bug." );
									VFSPath traverse = deepestParent;
									for( VFSPath::Element const& element : route )
									{
										traverse.Append( element );
										RFLOG_INFO( traverse, RFCAT_VFS, "Creating directory" );
										rftl::error_code err = {};
										rftl::filesystem::create_directory( traverse.CreateString(), err );
										if( err )
										{
											RFLOG_ERROR( traverse, RFCAT_VFS, "Filesystem reported an error of '%i' when creating directory", err.value() );
										}
									}
								}
							}
						}
						parentsCreated = true;
						continue;
					}
					else
					{
						RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file, parent seems to be missing" );
						return nullptr;
					}
				}
				else
				{
					RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file that was supposed to be flagged with creation" );
					return nullptr;
				}
			}
		}

		// Sweet! Got it
		RF_ASSERT( rawFile.is_open() );
		return DefaultCreator<FileHandle>::Create( DefaultCreator<rftl::filebuf>::Create( rftl::move( rawFile ) ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

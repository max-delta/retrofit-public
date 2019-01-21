#include "stdafx.h"
#include "VFS.h"

#include "PlatformFilesystem/FileHandle.h"
#include "Logging/Logging.h"

#include "core/macros.h"
#include "core/ptr/default_creator.h"
#include "core_math/math_casts.h"
#include "core_math/math_clamps.h"

#include "rftl/cstdio"
#include "rftl/sstream"
#include "rftl/filesystem"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

VFSPath const VFS::kRoot = VFSPath( "RF:" );
VFSPath const VFS::kInvalid = VFSPath( "INVALID:" );
VFSPath const VFS::kEmpty = VFSPath();

///////////////////////////////////////////////////////////////////////////////

VFS::VFS()
{
	//
}



FileHandlePtr VFS::GetFileForRead( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File read request" );
	return OpenFile( path, VFSMount::Permissions::ReadOnly, "rb", true );
}



FileHandlePtr VFS::GetFileForWrite( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File write request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "wb+", false );
}



FileHandlePtr VFS::GetFileForModify( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File modify request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "rb+", true );
}



FileHandlePtr VFS::GetFileForAppend( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File append request" );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "ab+", false );
}



FileHandlePtr VFS::GetFileForExecute( VFSPath const& path ) const
{
	RFLOG_DEBUG( path, RFCAT_VFS, "File execute request" );
	return OpenFile( path, VFSMount::Permissions::ReadExecute, "rb", true );
}



bool VFS::AttemptInitialMount( rftl::string const& mountTableFile, rftl::string const& userDirectory )
{
	RF_ASSERT( mountTableFile.empty() == false );
	RF_ASSERT( userDirectory.empty() == false );

	rftl::string absoluteSearchDirectory = rftl::filesystem::absolute( "." ).generic_string();
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file search directory: %s", absoluteSearchDirectory.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file param: %s", mountTableFile.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "User directory param: %s", userDirectory.c_str() );

	rftl::string absoluteMountTableFilename = rftl::filesystem::absolute( mountTableFile ).generic_string();
	RF_ASSERT( rftl::filesystem::exists( absoluteMountTableFilename ) );
	mMountTableFile = CollapsePath( CreatePathFromString( absoluteMountTableFilename ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "Mount table file: %s", CreateStringFromPath( mMountTableFile ).c_str() );

	mConfigDirectory = mMountTableFile.GetParent();
	RFLOG_INFO( nullptr, RFCAT_VFS, "Config directory: %s", CreateStringFromPath( mConfigDirectory ).c_str() );

	rftl::string absoluteUserDirectory = rftl::filesystem::absolute( userDirectory ).generic_string();
	RF_ASSERT( rftl::filesystem::exists( absoluteUserDirectory ) );
	mUserDirectory = CollapsePath( CreatePathFromString( absoluteUserDirectory ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "User directory: %s", CreateStringFromPath( mUserDirectory ).c_str() );

	FILE* file;
	rftl::string collapsedMountFilename = CreateStringFromPath( mMountTableFile );
	errno_t const openErr = fopen_s( &file, collapsedMountFilename.c_str(), "r" );
	if( openErr != 0 || file == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_VFS, "Failed to open mount table file" );
		return false;
	}

	rftl::string tokenBuilder;
	rftl::vector<rftl::string> tokenStream;
	enum class ReadMode : uint8_t
	{
		k_HuntingForToken,
		k_DiscardingComment,
		k_ReadingToken,
	} curReadMode = ReadMode::k_HuntingForToken;
	int readCode = 0;
	while( ( readCode = fgetc( file ) ) != EOF )
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



VFSPath VFS::AttemptMapToVFS( rftl::string const& physicalPath, VFSMount::Permissions desiredPermissions ) const
{
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mapping request: <%i> %s", desiredPermissions, physicalPath.c_str() );

	VFSPath const physicalAsVFS = CreatePathFromString( physicalPath );

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
			"  %s %s \"%s\" \"%s\"",
			type,
			permissions,
			CreateStringFromPath( mountRule.mVirtualPath ).c_str(),
			CreateStringFromPath( mountRule.mRealMount ).c_str() );
	}
}



VFSPath VFS::CreatePathFromString( rftl::string const& path )
{
	VFSPath retVal;

	VFSPath::Element elementBuilder;
	elementBuilder.reserve( path.size() );

	for( char const& ch : path )
	{
		if( ch == kPathDelimiter )
		{
			retVal.Append( elementBuilder );
			elementBuilder.clear();
			continue;
		}

		elementBuilder.push_back( ch );
	}
	if( elementBuilder.empty() == false )
	{
		retVal.Append( elementBuilder );
	}

	return retVal;
}



rftl::string VFS::CreateStringFromPath( VFSPath const& path )
{
	rftl::stringstream ss;

	size_t const numElements = path.NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		VFSPath::Element const& element = path.GetElement( i );

		if( i != 0 )
		{
			ss << kPathDelimiter;
		}
		ss << element;
	}

	return ss.str();
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



VFSMount VFS::ProcessMountRule( rftl::string const& type, rftl::string const& permissions, rftl::string const& virtualPoint, rftl::string const& realPoint )
{
	constexpr size_t prefixLen = sizeof( VFSMountTableTokens::kMountTokenAffix );
	constexpr size_t affixesLen = sizeof( VFSMountTableTokens::kMountTokenAffix ) * 2;
	RF_ASSERT( type.size() >= affixesLen );
	RF_ASSERT( permissions.size() >= affixesLen );
	RF_ASSERT( virtualPoint.size() >= affixesLen );
	RF_ASSERT( realPoint.size() >= affixesLen );
	VFSMount retVal;
	retVal.mType = VFSMount::Type::Invalid;
	retVal.mPermissions = VFSMount::Permissions::Invalid;

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
	retVal.mVirtualPath = kRoot.GetChild( CreatePathFromString( virtualVal ) );
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
	retVal.mRealMount = CreatePathFromString( realVal );
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
			element == kPathAscensionElement && (
				retVal.mType != VFSMount::Type::ConfigRelative &&
				retVal.mType != VFSMount::Type::UserRelative ) )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in non-relative real path" );
			badPathElement = true;
		}
		else if(
			element == kPathAscensionElement && (
				retVal.mType == VFSMount::Type::ConfigRelative ||
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



FileHandlePtr VFS::OpenFile( VFSPath const& uncollapsedPath, VFSMount::Permissions const& permissions, char const* openFlags, bool mustExist ) const
{
	// Collapse path first, make sure they aren't trying to trivially jump out
	//  of mount point, also clean it up so we can find the virtual mount point
	// NOTE: Not actually secure, don't assume this is even remotely a chroot
	VFSPath const path = CollapsePath( uncollapsedPath );
	if( path.IsDescendantOf( kRoot ) == false )
	{
		RFLOG_ERROR( path, RFCAT_VFS, "Virtual path doesn't descend from root" );
		return nullptr;
	}

	// Evaluate each mount point in order
	for( VFSMount const& mount : mMountTable )
	{
		if( ( (int)mount.mPermissions & (int)permissions ) != (int)permissions )
		{
			// Missing some permissions
			continue;
		}

		if( path.IsDescendantOf( mount.mVirtualPath ) == false )
		{
			// Not under this mount point
			continue;
		}

		// Where is this actually going to end up?
		rftl::string finalFilename;
		{
			VFSPath physTarget = kInvalid;

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
					RFLOG_ERROR( path, RFCAT_VFS, "Unhandled mount type" );
					return nullptr;
			}
			VFSPath branchRoot = physRoot->GetChild( mount.mRealMount );

			// The branch from the virtual point is the append
			bool isBranch = false;
			VFSPath pathAsBranch = path.GetAsBranchOf( mount.mVirtualPath, isBranch );
			RF_ASSERT_MSG( isBranch, "Descendant, but not branch? Logic error?" );

			// Final collapse and serialize
			physTarget = CollapsePath( branchRoot.GetChild( pathAsBranch ) );
			RF_ASSERT_MSG( physTarget.IsDescendantOf( kInvalid ) == false, "How? Shouldn't have ascencions in anything pre-collapse" );
			finalFilename = CreateStringFromPath( physTarget );
		}

		if( mustExist )
		{
			bool const exists = rftl::filesystem::exists( finalFilename );
			if( exists == false )
			{
				// Not here, maybe it's in an overlapping mount point
				continue;
			}
		}

		// Locked to this mount layer, let's see what happens!
		RFLOG_TRACE( path, RFCAT_VFS, "Open resolved to: %s", finalFilename.c_str() );
		FILE* file = nullptr;
		errno_t const openResult = fopen_s( &file, finalFilename.c_str(), openFlags );
		if( file == nullptr )
		{
			// Tough luck, no easy way to tell what went wrong
			if( mustExist )
			{
				RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file that was reported to exist, error code %i", openResult );
			}
			else
			{
				VFSPath const parentPath = CreatePathFromString( finalFilename ).GetParent();
				rftl::string parentPathStr = CreateStringFromPath( parentPath );
				bool const parentExists = rftl::filesystem::exists( parentPathStr );
				if( parentExists == false )
				{
					RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file, perhaps parent is missing?" );
				}
				else
				{
					RFLOG_ERROR( path, RFCAT_VFS, "Failed to open file that was supposed to be flagged with creation" );
				}
			}
			return nullptr;
		}

		// Sweet! Got it
		return DefaultCreator<FileHandle>::Create( rftl::move( file ) );
	}

	// Couldn't find
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

template<>
void RF::logging::WriteContextString( file::VFSPath const& context, Utf8LogContextBuffer& buffer )
{
	size_t bufferOffset = 0;
	size_t const maxBufferOffset = buffer.size();

	size_t const numElements = context.NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		file::VFSPath::Element const& element = context.GetElement( i );

		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}
		if( i != 0 )
		{
			buffer[bufferOffset] = file::VFS::kPathDelimiter;
			bufferOffset++;
		}
		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}

		size_t const bytesRemaining = maxBufferOffset - bufferOffset;
		size_t const bytesToWrite = math::Min( bytesRemaining, element.size() );
		memcpy( &buffer[bufferOffset], element.data(), bytesToWrite );
		bufferOffset += bytesToWrite;
	}
}

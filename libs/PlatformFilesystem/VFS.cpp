#include "stdafx.h"
#include "VFS.h"

#include "PlatformFilesystem/FileHandle.h"
#include "Logging/Logging.h"

#include "core/macros.h"
#include "core/ptr/entwined_creator.h"
#include "core_math/math_casts.h"

#include <stdio.h>
#include <sstream>
#include <experimental/filesystem>


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

VFSPath const VFS::k_Root = VFSPath( "RF:" );
VFSPath const VFS::k_Invalid = VFSPath( "INVALID:" );
VFSPath const VFS::k_Empty = VFSPath();

///////////////////////////////////////////////////////////////////////////////

VFS::VFS()
{
	//
}



static WeakPtr<VFS> HACK_Instance = nullptr;
VFS* VFS::HACK_GetInstance()
{
	return HACK_Instance;
}
void VFS::HACK_SetInstance( WeakPtr<VFS> instance )
{
	HACK_Instance = std::move( instance );
}



FileHandlePtr VFS::GetFileForRead( VFSPath const & path ) const
{
	RFLOG_TRACE( nullptr, RFCAT_VFS, "File read request: %s", CreateStringFromPath( path ).c_str() );
	return OpenFile( path, VFSMount::Permissions::ReadOnly, "rb", true );
}



FileHandlePtr VFS::GetFileForWrite( VFSPath const & path ) const
{
	RFLOG_TRACE( nullptr, RFCAT_VFS, "File write request: %s", CreateStringFromPath( path ).c_str() );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "wb+", false );
}



FileHandlePtr VFS::GetFileForModify( VFSPath const & path ) const
{
	RFLOG_TRACE( nullptr, RFCAT_VFS, "File modify request: %s", CreateStringFromPath( path ).c_str() );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "rb+", true );
}



FileHandlePtr VFS::GetFileForAppend( VFSPath const & path ) const
{
	RFLOG_TRACE( nullptr, RFCAT_VFS, "File append request: %s", CreateStringFromPath( path ).c_str() );
	return OpenFile( path, VFSMount::Permissions::ReadWrite, "ab+", false );
}



FileHandlePtr VFS::GetFileForExecute( VFSPath const & path ) const
{
	RFLOG_TRACE( nullptr, RFCAT_VFS, "File execute request: %s", CreateStringFromPath( path ).c_str() );
	return OpenFile( path, VFSMount::Permissions::ReadExecute, "rb", true );
}



bool VFS::AttemptInitialMount( std::string const & mountTableFile, std::string const & userDirectory )
{
	RF_ASSERT( mountTableFile.empty() == false );
	RF_ASSERT( userDirectory.empty() == false );

	std::string absoluteSearchDirectory = std::experimental::filesystem::v1::absolute( "." ).generic_string();
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file search directory: %s", absoluteSearchDirectory.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mount table file param: %s", mountTableFile.c_str() );
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "User directory param: %s", userDirectory.c_str() );

	std::string absoluteMountTableFilename = std::experimental::filesystem::v1::absolute( mountTableFile ).generic_string();
	RF_ASSERT( std::experimental::filesystem::v1::exists( absoluteMountTableFilename ) );
	m_MountTableFile = CollapsePath( CreatePathFromString( absoluteMountTableFilename ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "Mount table file: %s", CreateStringFromPath( m_MountTableFile ).c_str() );

	m_ConfigDirectory = m_MountTableFile.GetParent();
	RFLOG_INFO( nullptr, RFCAT_VFS, "Config directory: %s", CreateStringFromPath( m_ConfigDirectory ).c_str() );

	std::string absoluteUserDirectory = std::experimental::filesystem::v1::absolute( userDirectory ).generic_string();
	RF_ASSERT( std::experimental::filesystem::v1::exists( absoluteUserDirectory ) );
	m_UserDirectory = CollapsePath( CreatePathFromString( absoluteUserDirectory ) );
	RFLOG_INFO( nullptr, RFCAT_VFS, "User directory: %s", CreateStringFromPath( m_UserDirectory ).c_str() );

	FILE* file;
	std::string collapsedMountFilename = CreateStringFromPath( m_MountTableFile );
	errno_t const openErr = fopen_s( &file, collapsedMountFilename.c_str(), "r" );
	if( openErr != 0 || file == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_VFS, "Failed to open mount table file" );
		return false;
	}

	std::string tokenBuilder;
	std::vector<std::string> tokenStream;
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
			for( char const& ignoreCh : VFSMountTableTokens::k_MountFillerIgnores )
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

			if( ch == VFSMountTableTokens::k_MountTokenAffix[0] )
			{
				// Token, start reading
				curReadMode = ReadMode::k_ReadingToken;
				if( tokenBuilder.empty() == false )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Internal parser failure" );
					return false;
				}
			}
			else if( ch ==VFSMountTableTokens::k_MountCommentPrefix )
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

			if( ch == VFSMountTableTokens::k_MountCommentSuffix )
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
			RF_ASSERT( tokenBuilder[0] == VFSMountTableTokens::k_MountTokenAffix[0] );

			// Check integrity of initial tokens
			static_assert( sizeof( VFSMountTableTokens::k_MountTokenAffix ) == 2, "Token len changed" );
			if( tokenBuilder.size() == 1 )
			{
				if( tokenBuilder[0] != VFSMountTableTokens::k_MountTokenAffix[0] )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Malformed token prefix at start" );
					return false;
				}
				continue;
			}
			else if( tokenBuilder.size() == 2 )
			{
				if( tokenBuilder[1] != VFSMountTableTokens::k_MountTokenAffix[1] )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Malformed token prefix at start" );
					return false;
				}
				continue;
			}

			// Check for possible termination
			if( tokenBuilder.size() >= 4 && ch == VFSMountTableTokens::k_MountTokenAffix[1] )
			{
				RF_ASSERT( tokenBuilder.size() > sizeof( VFSMountTableTokens::k_MountTokenAffix ) );
				char const& previousToken = *( tokenBuilder.rbegin() + 1 );
				if( previousToken == VFSMountTableTokens::k_MountTokenAffix[0] )
				{
					// Termination! Save off the token
					tokenStream.emplace_back( std::move( tokenBuilder ) );
					RF_ASSERT( tokenBuilder.empty() );

					RF_ASSERT( tokenBuilder.size() <= VFSMountTableTokens::k_NumColumns );
					if( tokenStream.size() == VFSMountTableTokens::k_NumColumns )
					{
						// Form mount rule from tokens
						VFSMount mountRule = ProcessMountRule(
							tokenStream[0],
							tokenStream[1],
							tokenStream[2],
							tokenStream[3] );
						if(
							mountRule.m_Type == VFSMount::Type::Invalid ||
							mountRule.m_Permissions == VFSMount::Permissions::Invalid ||
							mountRule.m_VirtualPath.NumElements() == 0 )
						{
							RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to parse mount rule" );
							return false;
						}
						tokenStream.clear();

						m_MountTable.emplace_back( std::move( mountRule ) );
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
		m_MountTable.clear();
		return false;
	}
	else if( tokenBuilder.empty() == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Unclean token buffer at file end" );
		m_MountTable.clear();
		return false;
	}
	else if( tokenStream.empty() == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Unclean token stream at file end" );
		m_MountTable.clear();
		return false;
	}

	DebugDumpMountTable();
	return true;
}



VFSPath VFS::AttemptMapToVFS( std::string const & physicalPath, VFSMount::Permissions desiredPermissions ) const
{
	RFLOG_DEBUG( nullptr, RFCAT_VFS, "Mapping request: <%i> %s", desiredPermissions, physicalPath.c_str() );

	VFSPath const physicalAsVFS = CreatePathFromString( physicalPath );

	for( VFSMount const& mount : m_MountTable )
	{
		// NOTE: Technically faster to check permissions before path, but
		//  less easy to debug, and not performance-critical at time of writing

		VFSPath const* physRoot;
		switch( mount.m_Type )
		{
			case VFSMount::Type::Absolute:
				physRoot = &k_Empty;
				break;
			case VFSMount::Type::ConfigRelative:
				physRoot = &m_ConfigDirectory;
				break;
			case VFSMount::Type::UserRelative:
				physRoot = &m_UserDirectory;
				break;
			default:
				RFLOG_FATAL( nullptr, RFCAT_VFS, "Unhandled mount type" );
		}
		VFSPath const realMount = CollapsePath( physRoot->GetChild( mount.m_RealMount ) );
		bool const isDescendent = physicalAsVFS.IsDescendantOf( realMount );
		if( isDescendent == false )
		{
			// Unrelated path
			continue;
		}

		uint8_t const applicablePermissions = static_cast<uint8_t>( mount.m_Permissions ) & static_cast<uint8_t>( desiredPermissions );
		if( applicablePermissions != static_cast<uint8_t>( desiredPermissions ) )
		{
			// Not all permissions fulfilled
			continue;
		}

		// Match! Calculate branch
		bool isBranch;
		VFSPath const branchFromMount = physicalAsVFS.GetAsBranchOf( realMount, isBranch );
		RF_ASSERT( isBranch );
		return mount.m_VirtualPath.GetChild( branchFromMount );
	}

	return VFSPath();
}



void VFS::DebugDumpMountTable() const
{
	RFLOG_INFO( nullptr, RFCAT_VFS, "Mount table:" );
	for( VFSMount const& mountRule : m_MountTable )
	{
		char const* type = nullptr;
		switch( mountRule.m_Type )
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
			default:
				type = "INVALID";
				break;
		}
		char const* permissions = nullptr;
		switch( mountRule.m_Permissions )
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
			CreateStringFromPath( mountRule.m_VirtualPath ).c_str(),
			CreateStringFromPath( mountRule.m_RealMount ).c_str() );
	}
}



VFSPath VFS::CreatePathFromString( std::string const & path )
{
	VFSPath retVal;

	VFSPath::Element elementBuilder;
	elementBuilder.reserve( path.size() );

	for( char const& ch : path )
	{
		if( ch == k_PathDelimiter )
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



std::string VFS::CreateStringFromPath( VFSPath const & path )
{
	std::stringstream ss;

	size_t const numElements = path.NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		VFSPath::Element const& element = path.GetElement( i );
		
		if( i != 0 )
		{
			ss << k_PathDelimiter;
		}
		ss << element;
	}

	return ss.str();
}

///////////////////////////////////////////////////////////////////////////////

VFSPath VFS::CollapsePath( VFSPath const & path )
{
	VFSPath retVal;
	for( VFSPath::Element const& element : path )
	{
		// Ascencion
		if( element == k_PathAscensionElement )
		{
			if( retVal.NumElements() == 0 )
			{
				RFLOG_ERROR( nullptr, RFCAT_VFS, "Excessive ascencions found during collapse" );
				return k_Invalid.GetChild( path );
			}

			retVal.GoUp();
			continue;
		}

		// Current
		if( element == k_PathCurrentElement )
		{
			// Do nothing
			continue;
		}

		// Descension
		retVal.Append( element );
	}

	return retVal;
}



VFSMount VFS::ProcessMountRule( std::string const & type, std::string const & permissions, std::string const & virtualPoint, std::string const & realPoint )
{
	constexpr size_t prefixLen = sizeof( VFSMountTableTokens::k_MountTokenAffix );
	constexpr size_t affixesLen = sizeof( VFSMountTableTokens::k_MountTokenAffix ) * 2;
	RF_ASSERT( type.size() >= affixesLen );
	RF_ASSERT( permissions.size() >= affixesLen );
	RF_ASSERT( virtualPoint.size() >= affixesLen );
	RF_ASSERT( realPoint.size() >= affixesLen );
	VFSMount retVal;
	retVal.m_Type = VFSMount::Type::Invalid;
	retVal.m_Permissions = VFSMount::Permissions::Invalid;

	// Type
	std::string typeVal = type.substr( prefixLen, type.size() - affixesLen );
	if( typeVal == VFSMountTableTokens::k_MountTokenAbsolute )
	{
		retVal.m_Type = VFSMount::Type::Absolute;
	}
	else if( typeVal == VFSMountTableTokens::k_MountTokenConfigRelative )
	{
		retVal.m_Type = VFSMount::Type::ConfigRelative;
	}
	else if( typeVal == VFSMountTableTokens::k_MountTokenUserRelative )
	{
		retVal.m_Type = VFSMount::Type::UserRelative;
	}
	else
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Invalid mount type" );
		retVal.m_Type = VFSMount::Type::Invalid;
		return retVal;
	}

	// Permissions
	std::string permissionsVal = permissions.substr( prefixLen, permissions.size() - affixesLen );
	if( permissionsVal == VFSMountTableTokens::k_MountTokenReadOnly )
	{
		retVal.m_Permissions = VFSMount::Permissions::ReadOnly;
	}
	else if( permissionsVal == VFSMountTableTokens::k_MountTokenReadWrite )
	{
		retVal.m_Permissions = VFSMount::Permissions::ReadWrite;
	}
	else if( permissionsVal == VFSMountTableTokens::k_MountTokenReadExecute )
	{
		retVal.m_Permissions = VFSMount::Permissions::ReadExecute;
	}
	else
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Invalid permissions" );
		retVal.m_Type = VFSMount::Type::Invalid;
		retVal.m_Permissions = VFSMount::Permissions::Invalid;
		return retVal;
	}

	// Virtual
	std::string virtualVal = virtualPoint.substr( prefixLen, virtualPoint.size() - affixesLen );
	retVal.m_VirtualPath = k_Root.GetChild( CreatePathFromString( virtualVal ) );
	size_t const numVirtualElements = retVal.m_VirtualPath.NumElements();
	for( size_t i = 0; i < numVirtualElements; i++ )
	{
		VFSPath::Element const& element = retVal.m_VirtualPath.GetElement( i );

		bool badPathElement = false;
		if( element.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Empty path element in virtual path, parser failure?" );
			badPathElement = true;
		}
		else if( element == k_PathAscensionElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in virtual path" );
			badPathElement = true;
		}
		else if( element == k_PathCurrentElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path current element in virtual path" );
			badPathElement = true;
		}

		if( badPathElement )
		{
			retVal.m_VirtualPath.m_ElementList.clear();
			retVal.m_Type = VFSMount::Type::Invalid;
			retVal.m_Permissions = VFSMount::Permissions::Invalid;
			return retVal;
		}
	}

	// Real
	std::string realVal = realPoint.substr( prefixLen, realPoint.size() - affixesLen );
	retVal.m_RealMount = CreatePathFromString( realVal );
	bool hasEncounteredDescendToken = false;
	size_t const numRealElements = retVal.m_RealMount.NumElements();
	for( size_t i = 0; i < numRealElements; i++ )
	{
		VFSPath::Element const& element = retVal.m_RealMount.GetElement( i );

		bool badPathElement = false;
		if( element.empty() )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Empty path element in real path, parser failure?" );
			badPathElement = true;
		}
		else if(
			element == k_PathAscensionElement && (
				retVal.m_Type != VFSMount::Type::ConfigRelative &&
				retVal.m_Type != VFSMount::Type::UserRelative ) )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in non-relative real path" );
			badPathElement = true;
		}
		else if(
			element == k_PathAscensionElement && (
				retVal.m_Type == VFSMount::Type::ConfigRelative ||
				retVal.m_Type == VFSMount::Type::UserRelative ) &&
			hasEncounteredDescendToken )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path ascension element in relative real path after descending" );
			badPathElement = true;
		}
		else if( element == k_PathCurrentElement )
		{
			RFLOG_ERROR( nullptr, RFCAT_VFS, "Unsupported path current element in real path" );
			badPathElement = true;
		}

		if( badPathElement )
		{
			retVal.m_VirtualPath.m_ElementList.clear();
			retVal.m_RealMount.m_ElementList.clear();
			retVal.m_Type = VFSMount::Type::Invalid;
			retVal.m_Permissions = VFSMount::Permissions::Invalid;
			return retVal;
		}

		// Normal elements are descensions
		if( element != k_PathAscensionElement )
		{
			hasEncounteredDescendToken = true;
		}
	}

	return retVal;
}



FileHandlePtr VFS::OpenFile( VFSPath const & uncollapsedPath, VFSMount::Permissions const & permissions, char const * openFlags, bool mustExist ) const
{
	// Collapse path first, make sure they aren't trying to trivially jump out
	//  of mount point, also clean it up so we can find the virtual mount point
	// NOTE: Not actually secure, don't assume this is even remotely a chroot
	VFSPath path = CollapsePath( uncollapsedPath );
	if( path.IsDescendantOf( k_Root ) == false )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Virtual path doesn't descend from root" );
		return nullptr;
	}

	// Evaluate each mount point in order
	for( VFSMount const& mount : m_MountTable )
	{
		if( ( (int)mount.m_Permissions & (int)permissions ) != (int)permissions )
		{
			// Missing some permissions
			continue;
		}

		if( path.IsDescendantOf( mount.m_VirtualPath ) == false )
		{
			// Not under this mount point
			continue;
		}

		// Where is this actually going to end up?
		std::string finalFilename;
		{
			VFSPath physTarget = k_Invalid;

			// The root is the base
			VFSPath const* physRoot;
			switch( mount.m_Type )
			{
				case VFSMount::Type::Absolute:
					physRoot = &k_Empty;
					break;
				case VFSMount::Type::ConfigRelative:
					physRoot = &m_ConfigDirectory;
					break;
				case VFSMount::Type::UserRelative:
					physRoot = &m_UserDirectory;
					break;
				default:
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Unhandled mount type" );
					return nullptr;
			}
			VFSPath branchRoot = physRoot->GetChild( mount.m_RealMount );

			// The branch from the virtual point is the append
			bool isBranch = false;
			VFSPath pathAsBranch = path.GetAsBranchOf( mount.m_VirtualPath, isBranch );
			RF_ASSERT_MSG( isBranch, "Descendant, but not branch? Logic error?" );

			// Final collapse and serialize
			physTarget = CollapsePath( branchRoot.GetChild( pathAsBranch ) );
			RF_ASSERT_MSG( physTarget.IsDescendantOf( k_Invalid ) == false, "How? Shouldn't have ascencions in anything pre-collapse" );
			finalFilename = CreateStringFromPath( physTarget );
		}

		if( mustExist )
		{
			bool const exists = std::experimental::filesystem::v1::exists( finalFilename );
			if( exists == false )
			{
				// Not here, maybe it's in an overlapping mount point
				continue;
			}
		}

		// Locked to this mount layer, let's see what happens!
		RFLOG_TRACE( nullptr, RFCAT_VFS, "Open resolved to: %s", finalFilename.c_str() );
		FILE* file = nullptr;
		errno_t const openResult = fopen_s( &file, finalFilename.c_str(), openFlags );
		if( file == nullptr )
		{
			// Tough luck, no easy way to tell what went wrong
			if( mustExist )
			{
				RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to open file that was reported to exist, error code %i", openResult );
			}
			else
			{
				bool const parentExists
					= std::experimental::filesystem::v1::exists(
						CreateStringFromPath(
							CreatePathFromString( finalFilename ).GetParent() ) );
				if( parentExists == false )
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to open file, perhaps parent is missing?" );
				}
				else
				{
					RFLOG_ERROR( nullptr, RFCAT_VFS, "Failed to open file that was supposed to be flagged with creation" );
				}
			}
			return nullptr;
		}

		// Sweet! Got it
		return EntwinedCreator<FileHandle>::Create( file );
	}

	// Couldn't find
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
}}

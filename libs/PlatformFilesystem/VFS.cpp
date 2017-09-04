#include "stdafx.h"
#include "VFS.h"

#include "core_math/math_casts.h"
#include "core/macros.h"

#include <stdio.h>
#include <sstream>


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

VFSPath const VFS::k_Root = VFSPath( "RF:" );

///////////////////////////////////////////////////////////////////////////////

VFS::VFS()
{
	//
}



bool VFS::AttemptInitialMount( std::string const & mountTableFile, std::string const & userDirectory )
{
	RF_ASSERT( mountTableFile.empty() == false );
	RF_ASSERT( userDirectory.empty() == false );

	FILE* file;
	errno_t openErr = fopen_s( &file, mountTableFile.c_str(), "r" );
	if( openErr != 0 )
	{
		RF_ASSERT_MSG( false, "Failed to open mount table file" );
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
					RF_ASSERT_MSG( false, "Internal parser failure" );
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
				RF_ASSERT_MSG( false, "Unknown character encountered when searching for token" );
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
					RF_ASSERT_MSG( false, "Malformed token prefix at start" );
					return false;
				}
				continue;
			}
			else if( tokenBuilder.size() == 2 )
			{
				if( tokenBuilder[1] != VFSMountTableTokens::k_MountTokenAffix[1] )
				{
					RF_ASSERT_MSG( false, "Malformed token prefix at start" );
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
							RF_ASSERT_MSG( false, "Failed to parse mount rule" );
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

		RF_ASSERT_MSG( false, "Internal parse logic failure" );
		return false;
	}

	if( curReadMode != ReadMode::k_HuntingForToken )
	{
		RF_ASSERT_MSG( false, "Unterminated token at file end" );
		m_MountTable.clear();
		return false;
	}
	else if( tokenBuilder.empty() == false )
	{
		RF_ASSERT_MSG( false, "Unclean token buffer at file end" );
		m_MountTable.clear();
		return false;
	}
	else if( tokenStream.empty() == false )
	{
		RF_ASSERT_MSG( false, "Unclean token stream at file end" );
		m_MountTable.clear();
		return false;
	}

	return true;
}



void VFS::DebugDumpMountTable() const
{
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
		VFSPath virtualMountPoint = k_Root;
		virtualMountPoint.Append( mountRule.m_VirtualPath );
		printf(
			"%s %s \"%s\" \"%s\"\n",
			type,
			permissions,
			CreateStringFromPath( virtualMountPoint ).c_str(),
			CreateStringFromPath( mountRule.m_RealMount ).c_str() );
	}
}

///////////////////////////////////////////////////////////////////////////////

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
		RF_ASSERT_MSG( false, "Invalid mount type" );
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
		RF_ASSERT_MSG( false, "Invalid permissions" );
		retVal.m_Type = VFSMount::Type::Invalid;
		retVal.m_Permissions = VFSMount::Permissions::Invalid;
		return retVal;
	}

	// Virtual
	std::string virtualVal = virtualPoint.substr( prefixLen, virtualPoint.size() - affixesLen );
	retVal.m_VirtualPath = CreatePathFromString( virtualVal );
	size_t const numVirtualElements = retVal.m_VirtualPath.NumElements();
	for( size_t i = 0; i < numVirtualElements; i++ )
	{
		VFSPath::Element const& element = retVal.m_VirtualPath.GetElement( i );

		bool badPathElement = false;
		if( element.empty() )
		{
			RF_ASSERT_MSG( false, "Empty path element in virtual path, parser failure?" );
			badPathElement = true;
		}
		else if( element == k_PathAscensionElement )
		{
			RF_ASSERT_MSG( false, "Unsupported path ascension element in virtual path" );
			badPathElement = true;
		}
		else if( element == k_PathCurrentElement )
		{
			RF_ASSERT_MSG( false, "Unsupported path current element in virtual path" );
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
			RF_ASSERT_MSG( false, "Empty path element in real path, parser failure?" );
			badPathElement = true;
		}
		else if(
			element == k_PathAscensionElement && (
				retVal.m_Type != VFSMount::Type::ConfigRelative &&
				retVal.m_Type != VFSMount::Type::UserRelative ) )
		{
			RF_ASSERT_MSG( false, "Unsupported path ascension element in non-relative real path" );
			badPathElement = true;
		}
		else if(
			element == k_PathAscensionElement && (
				retVal.m_Type == VFSMount::Type::ConfigRelative ||
				retVal.m_Type == VFSMount::Type::UserRelative ) &&
			hasEncounteredDescendToken )
		{
			RF_ASSERT_MSG( false, "Unsupported path ascension element in relative real path after descending" );
			badPathElement = true;
		}
		else if( element == k_PathCurrentElement )
		{
			RF_ASSERT_MSG( false, "Unsupported path current element in real path" );
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

///////////////////////////////////////////////////////////////////////////////
}}

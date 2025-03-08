#include "stdafx.h"
#include "VFSPath.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"
#include "core_math/Hash.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

VFSPath::VFSPath()
	: SegmentedIdentifier()
{
	//
}



VFSPath::VFSPath( SegmentedIdentifier<rftl::string> const& path )
	: SegmentedIdentifier( path )
{
	//
}



rftl::string VFSPath::GetTrailingExtensions() const
{
	if( m_ElementList.empty() )
	{
		return {};
	}

	// Find first dot
	rftl::string const& lastElement = m_ElementList.back();
	size_t const firstDotIndex = lastElement.find_first_of( '.', 0 );
	if( firstDotIndex == rftl::string::npos )
	{
		return {};
	}

	// Substring
	return lastElement.substr( firstDotIndex, rftl::string::npos );
}



VFSPath& VFSPath::RemoveTrailingExtensions()
{
	if( m_ElementList.empty() )
	{
		return *this;
	}

	// Find first dot
	rftl::string& lastElement = m_ElementList.back();
	size_t const firstDotIndex = lastElement.find_first_of( '.', 0 );
	if( firstDotIndex == rftl::string::npos )
	{
		return *this;
	}

	// Shrink
	lastElement.resize( firstDotIndex, '?' );

	return *this;
}



VFSPath VFSPath::CreatePathFromString( rftl::string const& path )
{
	return id::CreateIdentifierFromString<char, kPathDelimiter>( path );
}



rftl::string VFSPath::CreateString() const
{
	return id::CreateStringFromIdentifer<char, kPathDelimiter>( *this );
}

///////////////////////////////////////////////////////////////////////////////
}

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
			buffer[bufferOffset] = file::kPathDelimiter;
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

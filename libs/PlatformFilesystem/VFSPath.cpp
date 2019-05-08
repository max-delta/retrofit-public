#include "stdafx.h"
#include "VFSPath.h"

#include "Logging/Logging.h"

#include "core_math/math_clamps.h"
#include "core_math/hash.h"
#include "core/macros.h"

#include "rftl/sstream"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

VFSPath::VFSPath()
{
	//
}



bool VFSPath::operator==( VFSPath const& rhs ) const
{
	size_t const numElements = this->NumElements();
	if( numElements != rhs.NumElements() )
	{
		return false;
	}

	for( size_t i = 0; i < numElements; i++ )
	{
		if( this->GetElement( i ) != rhs.GetElement( i ) )
		{
			return false;
		}
	}

	return true;
}



VFSPath VFSPath::GetParent() const
{
	VFSPath retVal = *this;
	return retVal.GoUp();
}



VFSPath VFSPath::GetChild( VFSPath const& path ) const
{
	VFSPath retVal = *this;
	return retVal.AppendUnroll( path );
}



VFSPath VFSPath::GetChild( Element const& element ) const
{
	VFSPath retVal = *this;
	return retVal.AppendUnroll( element );
}



VFSPath VFSPath::GetAsBranchOf( VFSPath const& parent, bool& isBranch ) const
{
	if( IsDescendantOf( parent ) == false )
	{
		isBranch = false;
		return *this;
	}

	// Descendant, trim off parent
	VFSPath retVal;
	size_t const parentNumElements = parent.NumElements();
	size_t const numElements = m_ElementList.size();
	RF_ASSERT( parentNumElements < numElements );
	for( size_t i = parentNumElements; i < numElements; i++ )
	{
		retVal.Append( m_ElementList[i] );
	}
	isBranch = true;
	return retVal;
}



VFSPath& VFSPath::GoUp()
{
	m_ElementList.pop_back();
	return *this;
}



VFSPath& VFSPath::GoUp( size_t count )
{
	for( size_t i = 0; i < count; i++ )
	{
		GoUp();
	}
	return *this;
}



VFSPath& VFSPath::Append( VFSPath const& path )
{
	return AppendUnroll( path );
}



VFSPath& VFSPath::Append( Element const& element )
{
	return AppendUnroll( element );
}



bool VFSPath::IsDescendantOf( VFSPath const& closerToRoot ) const
{
	size_t const childNumNodes = NumElements();
	if( childNumNodes == 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Cannot compare using an empty path" );
		return false;
	}

	size_t const parentNumNodes = closerToRoot.NumElements();
	if( parentNumNodes == 0 )
	{
		RFLOG_ERROR( nullptr, RFCAT_VFS, "Cannot compare using an empty path" );
		return false;
	}

	if( childNumNodes <= parentNumNodes )
	{
		// Child must have more nodes, as it must branch from end of parent
		return false;
	}

	for( size_t i = 0; i < parentNumNodes; i++ )
	{
		if( m_ElementList[i] != closerToRoot.m_ElementList[i] )
		{
			// Diverges too early
			return false;
		}
	}

	// Parent path is fully part of the child path
	return true;
}



bool VFSPath::IsImmediateDescendantOf( VFSPath const& immediateParent ) const
{
	if( IsDescendantOf( immediateParent ) == false )
	{
		return false;
	}

	// Descendant, but only by 1 level
	return NumElements() + 1 == immediateParent.NumElements();
}



bool VFSPath::Empty() const
{
	return m_ElementList.empty();
}



size_t VFSPath::NumElements() const
{
	return m_ElementList.size();
}



VFSPath::Element const& VFSPath::GetElement( size_t index ) const
{
	return m_ElementList[index];
}



VFSPath VFSPath::CreatePathFromString( rftl::string const& path )
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



rftl::string VFSPath::CreateString() const
{
	rftl::stringstream ss;

	size_t const numElements = NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		VFSPath::Element const& element = GetElement( i );

		if( i != 0 )
		{
			ss << kPathDelimiter;
		}
		ss << element;
	}

	return ss.str();
}



VFSPath::const_iterator VFSPath::begin() const
{
	return m_ElementList.cbegin();
}



VFSPath::const_iterator VFSPath::end() const
{
	return m_ElementList.cend();
}



VFSPath& VFSPath::AppendUnroll()
{
	// Null case
	return *this;
}



VFSPath& VFSPath::AppendUnroll( VFSPath const& path )
{
	for( Element const& element : path.m_ElementList )
	{
		m_ElementList.emplace_back( element );
	}
	return *this;
}



VFSPath& VFSPath::AppendUnroll( Element const& element )
{
	m_ElementList.emplace_back( element );
	return *this;
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



size_t rftl::hash<RF::file::VFSPath>::operator()( RF::file::VFSPath const& path ) const
{
	return RF::math::SequenceHash<
		RF::file::VFSPath,
		rftl::hash<RF::file::VFSPath::Element>>()( path );
}

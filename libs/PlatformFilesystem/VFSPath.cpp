#include "stdafx.h"
#include "VFSPath.h"

#include "core/macros.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

VFSPath::VFSPath()
{
	//
}



VFSPath VFSPath::GetParent() const
{
	VFSPath retVal = *this;
	return retVal.GoUp();
}



VFSPath VFSPath::GetChild( VFSPath const& path ) const
{
	VFSPath retVal;
	return retVal.AppendUnroll( path );
}



VFSPath VFSPath::GetChild( Element const& element ) const
{
	VFSPath retVal;
	return retVal.AppendUnroll( element );
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
		RF_ASSERT_MSG( false, "Cannot compare using an empty path" );
		return false;
	}

	size_t const parentNumNodes = closerToRoot.NumElements();
	if( parentNumNodes == 0 )
	{
		RF_ASSERT_MSG( false, "Cannot compare using an empty path" );
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



bool VFSPath::IsImmediateDescendantOf( VFSPath const & immediateParent ) const
{
	if( IsDescendantOf( immediateParent ) == false )
	{
		return false;
	}

	// Descendant, but only by 1 level
	return NumElements() + 1 == immediateParent.NumElements();
}



size_t VFSPath::NumElements() const
{
	return m_ElementList.size();
}



VFSPath::Element const& VFSPath::GetElement( size_t index ) const
{
	return m_ElementList[index];
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

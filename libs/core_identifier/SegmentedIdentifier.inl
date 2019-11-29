#pragma once
#include "SegmentedIdentifier.h"

#include "core_math/Hash.h"

#include "rftl/sstream"


namespace RF { namespace id {
///////////////////////////////////////////////////////////////////////////////

template<typename ElementT>
inline SegmentedIdentifier<ElementT>::SegmentedIdentifier()
{
	//
}



template<typename ElementT>
template<typename... Nodes>
inline SegmentedIdentifier<ElementT>::SegmentedIdentifier( Element element, Nodes... elements )
{
	AppendUnroll( element, elements... );
}



template<typename ElementT>
inline bool SegmentedIdentifier<ElementT>::operator==( SegmentedIdentifier const& rhs ) const
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



template<typename ElementT>
inline SegmentedIdentifier<ElementT> SegmentedIdentifier<ElementT>::GetParent() const
{
	// Copy
	SegmentedIdentifier retVal = *this;
	return retVal.GoUp();
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT> SegmentedIdentifier<ElementT>::GetChild( SegmentedIdentifier const& identifier ) const
{
	// Copy
	SegmentedIdentifier retVal = *this;
	return retVal.AppendUnroll( identifier );
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT> SegmentedIdentifier<ElementT>::GetChild( Element const& element ) const
{
	// Copy
	SegmentedIdentifier retVal = *this;
	return retVal.AppendUnroll( element );
}



template<typename ElementT>
template<typename... IdentifiersOrElements>
inline SegmentedIdentifier<ElementT> SegmentedIdentifier<ElementT>::GetChild( IdentifiersOrElements... identifiersOrElements ) const
{
	// Copy
	SegmentedIdentifier retVal = *this;
	return retVal.AppendUnroll( identifiersOrElements... );
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT> SegmentedIdentifier<ElementT>::GetAsBranchOf( SegmentedIdentifier const& parent, bool& isBranch ) const
{
	if( IsDescendantOf( parent ) == false )
	{
		isBranch = false;
		return *this;
	}

	// Descendant, trim off parent
	SegmentedIdentifier retVal;
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



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::GoUp()
{
	m_ElementList.pop_back();
	return *this;
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::GoUp( size_t count )
{
	for( size_t i = 0; i < count; i++ )
	{
		GoUp();
	}
	return *this;
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::Append( SegmentedIdentifier const& identifier )
{
	return AppendUnroll( identifier );
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::Append( Element const& element )
{
	return AppendUnroll( element );
}



template<typename ElementT>
template<typename... IdentifiersOrElements>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::Append( IdentifiersOrElements... identifiersOrElements )
{
	return AppendUnroll( identifiersOrElements... );
}



template<typename ElementT>
inline void SegmentedIdentifier<ElementT>::clear()
{
	m_ElementList.clear();
}



template<typename ElementT>
inline bool SegmentedIdentifier<ElementT>::IsDescendantOf( SegmentedIdentifier const& closerToRoot ) const
{
	size_t const childNumNodes = NumElements();
	if( childNumNodes == 0 )
	{
		RF_DBGFAIL_MSG( "Cannot compare using an empty identifier" );
		return false;
	}

	size_t const parentNumNodes = closerToRoot.NumElements();
	if( parentNumNodes == 0 )
	{
		RF_DBGFAIL_MSG( "Cannot compare using an empty identifier" );
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

	// Parent identifier is fully part of the child identifier
	return true;
}



template<typename ElementT>
inline bool SegmentedIdentifier<ElementT>::IsImmediateDescendantOf( SegmentedIdentifier const& immediateParent ) const
{
	if( IsDescendantOf( immediateParent ) == false )
	{
		return false;
	}

	// Descendant, but only by 1 level
	return NumElements() + 1 == immediateParent.NumElements();
}



template<typename ElementT>
inline bool SegmentedIdentifier<ElementT>::Empty() const
{
	return m_ElementList.empty();
}



template<typename ElementT>
inline size_t SegmentedIdentifier<ElementT>::NumElements() const
{
	return m_ElementList.size();
}



template<typename ElementT>
inline typename SegmentedIdentifier<ElementT>::Element const& SegmentedIdentifier<ElementT>::GetElement( size_t index ) const
{
	return m_ElementList[index];
}



template<typename ElementT>
inline typename SegmentedIdentifier<ElementT>::const_iterator SegmentedIdentifier<ElementT>::begin() const
{
	return m_ElementList.cbegin();
}



template<typename ElementT>
inline typename SegmentedIdentifier<ElementT>::const_iterator SegmentedIdentifier<ElementT>::end() const
{
	return m_ElementList.cend();
}

///////////////////////////////////////////////////////////////////////////////

template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::AppendUnroll()
{
	// Null case
	return *this;
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::AppendUnroll( SegmentedIdentifier const& identifier )
{
	for( Element const& element : identifier.m_ElementList )
	{
		m_ElementList.emplace_back( element );
	}
	return *this;
}



template<typename ElementT>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::AppendUnroll( Element const& element )
{
	m_ElementList.emplace_back( element );
	return *this;
}



template<typename ElementT>
template<typename IdentifierOrElement1, typename IdentifierOrElement2, typename... IdentifiersOrElements>
inline SegmentedIdentifier<ElementT>& SegmentedIdentifier<ElementT>::AppendUnroll( IdentifierOrElement1 identifierOrElement1, IdentifierOrElement2 identifierOrElement2, IdentifiersOrElements... identifiersOrElements )
{
	AppendUnroll( identifierOrElement1 );
	AppendUnroll( identifierOrElement2 );
	return AppendUnroll( identifiersOrElements... );
}

///////////////////////////////////////////////////////////////////////////////

template<typename CharT, CharT DelimiterT>
SegmentedIdentifier<rftl::basic_string<CharT>> CreateIdentifierFromString( rftl::string const& identifier )
{
	SegmentedIdentifier<rftl::basic_string<CharT>> retVal;

	typename SegmentedIdentifier<rftl::basic_string<CharT>>::Element elementBuilder;
	elementBuilder.reserve( identifier.size() );

	for( CharT const& ch : identifier )
	{
		if( ch == DelimiterT )
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



template<typename CharT, CharT DelimiterT>
rftl::string CreateStringFromIdentifer( SegmentedIdentifier<rftl::basic_string<CharT>> const& identifier )
{
	rftl::stringstream ss;

	size_t const numElements = identifier.NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		typename SegmentedIdentifier<rftl::basic_string<CharT>>::Element const& element = identifier.GetElement( i );

		if( i != 0 )
		{
			ss << DelimiterT;
		}
		ss << element;
	}

	return ss.str();
}

///////////////////////////////////////////////////////////////////////////////
}}

template<typename ElementT>
inline size_t rftl::hash<RF::id::SegmentedIdentifier<ElementT>>::operator()( RF::id::SegmentedIdentifier<ElementT> const& identifier ) const
{
	return static_cast<size_t>( RF::math::SequenceHash<
		RF::id::SegmentedIdentifier<ElementT>,
		rftl::hash<typename RF::id::SegmentedIdentifier<ElementT>::Element>>()( identifier ) );
}

#pragma once
#include "SegmentedIdentifier.h"

#include "core/macros.h"

#include "rftl/vector"
#include "rftl/string"
#include "rftl/type_traits"


namespace RF::id {
///////////////////////////////////////////////////////////////////////////////

template<typename ElementT>
class SegmentedIdentifier
{
	//
	// Types
public:
	using Element = ElementT;
	using const_iterator = typename rftl::vector<Element>::const_iterator;

private:
	using ElementList = rftl::vector<Element>;
	static_assert( rftl::is_same<typename ElementList::const_iterator, const_iterator>::value, "Public type differs" );


	//
	// Public methods
public:
	SegmentedIdentifier();
	template<typename... Nodes>
	explicit SegmentedIdentifier( Element element, Nodes... elements );

	bool operator==( SegmentedIdentifier const& rhs ) const;

	// Create new identifier relative to this identifier
	SegmentedIdentifier GetParent() const;
	SegmentedIdentifier GetChild( SegmentedIdentifier const& identifier ) const;
	SegmentedIdentifier GetChild( Element const& element ) const;
	template<typename... IdentifiersOrElements>
	SegmentedIdentifier GetChild( IdentifiersOrElements... identifiersOrElements ) const;
	SegmentedIdentifier GetAsBranchOf( SegmentedIdentifier const& parent, bool& isBranch ) const;

	// Modify this identifier
	SegmentedIdentifier& GoUp();
	SegmentedIdentifier& GoUp( size_t count );
	SegmentedIdentifier& Append( SegmentedIdentifier const& identifier );
	SegmentedIdentifier& Append( Element const& element );
	template<typename... IdentifiersOrElements>
	SegmentedIdentifier& Append( IdentifiersOrElements... identifiersOrElements );
	void clear();

	// Compare and access
	bool IsDescendantOf( SegmentedIdentifier const& closerToRoot ) const;
	bool IsImmediateDescendantOf( SegmentedIdentifier const& immediateParent ) const;
	bool Empty() const;
	size_t NumElements() const;
	Element const& GetElement( size_t index ) const;

	// Range support
	const_iterator begin() const;
	const_iterator end() const;


	//
	// Protected methods
protected:
	SegmentedIdentifier& AppendUnroll();
	SegmentedIdentifier& AppendUnroll( SegmentedIdentifier const& identifier );
	SegmentedIdentifier& AppendUnroll( Element const& element );
	template<typename IdentifierOrElement1, typename IdentifierOrElement2, typename... IdentifiersOrElements>
	SegmentedIdentifier& AppendUnroll( IdentifierOrElement1 identifierOrElement1, IdentifierOrElement2 identifierOrElement2, IdentifiersOrElements... identifiersOrElements );


	//
	// Protected data
protected:
	ElementList m_ElementList;
};

///////////////////////////////////////////////////////////////////////////////

// To/from string
template<typename CharT, CharT DelimiterT>
SegmentedIdentifier<rftl::basic_string<CharT>> CreateIdentifierFromString( rftl::string const& path );
template<typename CharT, CharT DelimiterT>
rftl::string CreateStringFromIdentifer( SegmentedIdentifier<rftl::basic_string<CharT>> const& identifier );

///////////////////////////////////////////////////////////////////////////////
}

template<typename ElementT>
struct rftl::hash<RF::id::SegmentedIdentifier<ElementT>>
{
	size_t operator()( RF::id::SegmentedIdentifier<ElementT> const& identifier ) const;
};

// Explicitly instantiate and alias common types
namespace RF::id {
using SegmentedStringIdentifier = SegmentedIdentifier<rftl::string>;
}
extern template class RF::id::SegmentedIdentifier<rftl::string>;

#include "SegmentedIdentifier.inl"

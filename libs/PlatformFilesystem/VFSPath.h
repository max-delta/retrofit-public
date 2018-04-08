#pragma once
#include "project.h"

#include "rftl/string"
#include "rftl/vector"
#include "rftl/type_traits"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFSPath
{
	//
	// Friends
private:
	friend class VFS;


	//
	// Types
public:
	typedef rftl::string Element;
	typedef rftl::vector<Element>::const_iterator const_iterator;
private:
	typedef rftl::vector<Element> ElementList;
	static_assert( rftl::is_same<ElementList::const_iterator, const_iterator>::value, "Public type differs" );


	//
	// Public methods
public:
	VFSPath();
	template<typename...Nodes>
	explicit VFSPath( Element element, Nodes...elements );

	// Create new path relative to this path
	VFSPath GetParent() const;
	VFSPath GetChild( VFSPath const& path ) const;
	VFSPath GetChild( Element const& element ) const;
	template<typename ...PathsOrElements>
	VFSPath GetChild( PathsOrElements ...pathsOrElements ) const;
	VFSPath GetAsBranchOf( VFSPath const& parent, bool& isBranch ) const;

	// Modify this path
	VFSPath& GoUp();
	VFSPath& GoUp( size_t count );
	VFSPath& Append( VFSPath const& path );
	VFSPath& Append( Element const& element );
	template<typename ...PathsOrElements>
	VFSPath& Append( PathsOrElements ...pathsOrElements );

	// Compare and access
	bool IsDescendantOf( VFSPath const& closerToRoot ) const;
	bool IsImmediateDescendantOf( VFSPath const& immediateParent ) const;
	bool Empty() const;
	size_t NumElements() const;
	Element const& GetElement( size_t index ) const;

	// Range support
	const_iterator begin() const;
	const_iterator end() const;


	//
	// Private methods
private:
	VFSPath& AppendUnroll();
	VFSPath& AppendUnroll( VFSPath const& path );
	VFSPath& AppendUnroll( Element const& element );
	template<typename PathOrElement1, typename PathOrElement2, typename...PathsOrElements>
	VFSPath& AppendUnroll( PathOrElement1 pathOrElement1, PathOrElement2 pathOrElement2, PathsOrElements...pathsOrElements );


	//
	// Private data
private:
	ElementList m_ElementList;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "VFSPath.inl"

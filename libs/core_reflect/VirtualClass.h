#pragma once


// Forwards
namespace RF { namespace reflect {
	struct ClassInfo;
}}

namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

// For specialized cases where the destructor in VirtualClass is undesired
class VirtualClassWithoutDestructor
{
public:
	virtual ClassInfo const* GetVirtualClassInfo() const = 0;
};


// Serves as a base class for reflection that needs to be able to distinguish
//  between derived types
class VirtualClass : public VirtualClassWithoutDestructor
{
	RF_DEFAULT_COPY( VirtualClass );

public:
	VirtualClass() = default;
	virtual ~VirtualClass() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

inline RF::reflect::VirtualClass::~VirtualClass()
{
	// This is an odd work-around to a bug in MSVC and a warning in Clang,
	//  solved using another bug in MSVC.
	// A common expected use case is to derive from VirtualClass, and then
	//  export the derived class from a DLL.
	// If a virtual destructor is not present when a virtual table is made,
	//  Clang reasonably complains. So VirtualClass should have a virtual
	//  destructor in the common case.
	// But the presence of a function on a base class that is not exported, and
	//  then inherited onto a derived class that IS exported, will cause MSVC
	//  to reasonably warn.
	// We can't export/import the VirtualClass, since we don't know how an
	//  engine may want to integrate it, and that would create a major
	//  restriction on how it could be used.
	// We then want to have an inline, non-exported destructor for derived
	//  exported classes to find during linking and chain to. But, MSVC doesn't
	//  detect the inline when evaluating the warning, which seems to be a bug.
	// But, a further bug allows the rule to be worked around, by making the
	//  destructor pure virtual. However, a pure virtual destructor is
	//  nonsensical and not permitted. While not a compilation error, the
	//  pure-ness of the virtual function is not respected, and the linker
	//  requires an implementation to chain through. This is that
	//  implementation.
}

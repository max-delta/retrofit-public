#pragma once


// Forwards
namespace RF { namespace reflect {
	struct ClassInfo;
}}

namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

// Serves as a base class for reflection that needs to be able to distinguish
//  between derived types
class VirtualClass
{
public:
	virtual ClassInfo const* GetVirtualClassInfo() const = 0;
};

///////////////////////////////////////////////////////////////////////////////
}}

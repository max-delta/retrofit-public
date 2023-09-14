#include "stdafx.h"
#include "Example.h"

#include "RFType/CreateClassInfoDefinition.h"


///////////////////////////////////////////////////////////////////////////////

// The core libraries cannot guarantee uniquely addressable code, since they
//  are often statically linked into multiple modules. This means they cannot
//  safely provide the meta info for RF::rftype::GetClassInfo<...>(...).
// The VirtualClass machinery is important for a lot of use-cases, so RFType
//  takes the responsibility of creating the meta info for the related class in
//  its non-core library.
// An important example of the need for this is when there's an attempt to
//  reflect a 'UniquePtr<VirtualClass>', which is a highly flexible pointer
//  type that can still reliably get reflection info for casting to a derived
//  non-abstract class.

RFTYPE_CREATE_META( RF::reflect::VirtualClassWithoutDestructor )
{
	//
}

RFTYPE_CREATE_META( RF::reflect::VirtualClass )
{
	//
}

///////////////////////////////////////////////////////////////////////////////

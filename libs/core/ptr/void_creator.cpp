#include "stdafx.h"
#include "void_creator.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

CreationPayload<void> VoidCreator::Create( void* ptr )
{
	CreationPayload<void> retVal(
		ptr,
		new PtrRef( &Delete, nullptr ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void VoidCreator::Delete( void const* target, PtrRef* ref, void* userData )
{
	(void)target;
	(void)userData;
	if( ref != nullptr )
	{
		delete ref;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

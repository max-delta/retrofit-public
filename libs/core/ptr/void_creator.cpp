#include "stdafx.h"
#include "void_creator.h"

#include "core/rf_assert.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

CreationPayload<void> VoidCreator::Create()
{
	CreationPayload<void> retVal(
		const_cast<void*>( compiler::kInvalidNonNullPointer ),
		new PtrRef( &Delete, nullptr ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

void VoidCreator::Delete( void const* target, PtrRef* ref, void* userData )
{
	RF_ASSERT( userData == nullptr );
	if( target != nullptr )
	{
		RF_ASSERT( target == compiler::kInvalidNonNullPointer );
	}
	if( ref != nullptr )
	{
		delete ref;
	}
}

///////////////////////////////////////////////////////////////////////////////
}

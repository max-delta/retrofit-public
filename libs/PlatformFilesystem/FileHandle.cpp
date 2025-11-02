#include "stdafx.h"
#include "FileHandle.h"

#include "core_math/math_casts.h"

#include "rftl/ios"
#include "rftl/istream"
#include "rftl/ostream"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( UniquePtr<rftl::streambuf>&& streamBuf )
	: SeekHandle( rftl::move( streamBuf ) )
{
	//
}



FileHandle::~FileHandle()
{
	RF_ASSERT( mStreamBuf != nullptr );
	// NOTE: Would like to check for errors when closing a file, but there is
	//  no exposed way to do that unless we know the concrete type
	// NOTE: Maybe have an optional WeakPtr option for the concrete type, that
	//  we could use here if available?
	RF_TODO_ANNOTATION( "Come up with a way to check for errors on close" );
}

///////////////////////////////////////////////////////////////////////////////
}

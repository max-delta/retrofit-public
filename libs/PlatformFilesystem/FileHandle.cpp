#include "stdafx.h"
#include "FileHandle.h"

#include "core_math/math_casts.h"

#include "rftl/fstream"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( UniquePtr<rftl::filebuf>&& fileBuf, VFSPath const& path )
	: FileHandle( fileBuf, rftl::move( fileBuf ), path )
{
	//
}



FileHandle::~FileHandle()
{
	RF_ASSERT( mStreamBuf != nullptr );
	RF_ASSERT( mConcreteType != nullptr );

	if( mConcreteType->is_open() == false )
	{
		RFLOGF_ERROR( mPath, RFCAT_VFS, "File was already closed prior to destruction" );
		RF_DBGFAIL();
	}
	else
	{
		rftl::filebuf const* const errCheck = mConcreteType->close();
		if( errCheck == nullptr || mConcreteType->is_open() )
		{
			int32_t const assumedErr = math::integer_cast<int32_t>( errno );

			// Tough luck, no easy way to tell what went wrong
			RFLOGF_ERROR( mPath, RFCAT_VFS, "Failed to close file, last error code was {}", assumedErr );
			RF_DBGFAIL();
		}
	}

	mConcreteType = nullptr;
}

///////////////////////////////////////////////////////////////////////////////

FileHandle::FileHandle( WeakPtr<rftl::filebuf> fileBuf, UniquePtr<rftl::filebuf>&& streamBuf, VFSPath const& path )
	: SeekHandle( rftl::move( streamBuf ) )
	, mConcreteType( fileBuf )
	, mPath( path )
{
	RF_ASSERT( mConcreteType != nullptr );
}

///////////////////////////////////////////////////////////////////////////////
}

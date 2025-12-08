#pragma once
#include "core/macros.h"

#include "rftl/shared_mutex"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

// Basic helper to tie a mutex to some data
template<typename DataT>
struct LockedData final
{
	RF_NO_COPY( LockedData );

	//
	// Types and constants
private:
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;

	//
	// Public methods
public:
	LockedData() = default;
	~LockedData() = default;

	LockedData( DataT const& data )
		: mData( data )
	{
		//
	}

	LockedData( DataT&& data )
		: mData( rftl::move( data ) )
	{
		//
	}

	// Thread-safe
	template<typename CallableT>
	void Read( CallableT& callable ) const
	{
		ReaderLock const lock( mMutex );
		DataT const& readOnly = mData;
		callable( readOnly );
	}
	template<typename CallableT>
	void Read( CallableT&& callable ) const
	{
		Read( callable );
	}

	// Thread-safe
	template<typename CallableT>
	void Write( CallableT& callable )
	{
		WriterLock const lock( mMutex );
		DataT& writeable = mData;
		callable( writeable );
	}
	template<typename CallableT>
	void Write( CallableT&& callable )
	{
		Write( callable );
	}

	// Thread-safe
	[[nodiscard]] DataT Copy() const
	{
		ReaderLock const lock( mMutex );
		DataT copy = mData;
		return copy;
	}

	// Thread-safe
	[[nodiscard]] DataT Extract()
	{
		WriterLock const lock( mMutex );
		DataT extracted = rftl::move( mData );
		return extracted;
	}

	// Thread-safe
	void Emplace( DataT&& data )
	{
		WriterLock const lock( mMutex );
		mData = rftl::move( data );
	}

	// Thread-safe
	void Push( DataT const& data )
	{
		WriterLock const lock( mMutex );
		mData = data;
	}


	//
	// Private data
private:
	mutable ReaderWriterMutex mMutex;
	DataT mData = {};
};


///////////////////////////////////////////////////////////////////////////////
}

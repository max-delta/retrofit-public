#pragma once
#include "StateTree.h"

#include "core_allocate/DefaultAllocCreator.h"


namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType> StateTree<ValueT, MaxChangesT>::GetOrCreateStream( VariableIdentifier const& identifier, alloc::Allocator& allocator )
{
	// Initial lock assumes we can return without mutation
	{
		ReaderLock lock( mMultiReaderSingleWriterLock );

		typename Tree::const_iterator const iter = mTree.find( identifier );
		if( iter != mTree.end() )
		{
			return iter->second;
		}
	}

	// Elevate to writer lock and try again
	{
		WriterLock lock( mMultiReaderSingleWriterLock );

		// NOTE: Have to re-search since we briefly dropped the lock
		typename Tree::const_iterator const iter = mTree.find( identifier );
		if( iter == mTree.end() )
		{
			UniquePtr<StreamType> newStream = alloc::DefaultAllocCreator<StreamType>::Create( allocator );
			typename Tree::iterator const newIter = mTree.emplace( identifier, rftl::move( newStream ) ).first;
			WeakPtr<StreamType> const retVal = newIter->second;
			return retVal;
		}
		return iter->second;
	}
}



template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType const> StateTree<ValueT, MaxChangesT>::GetStream( VariableIdentifier const& identifier ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	typename Tree::const_iterator const iter = mTree.find( identifier );
	if( iter == mTree.end() )
	{
		return nullptr;
	}
	return iter->second;
}



template<typename ValueT, size_t MaxChangesT>
WeakPtr<typename StateTree<ValueT, MaxChangesT>::StreamType> StateTree<ValueT, MaxChangesT>::GetMutableStream( VariableIdentifier const& identifier )
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	typename Tree::const_iterator const iter = mTree.find( identifier );
	if( iter == mTree.end() )
	{
		return nullptr;
	}
	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
}}

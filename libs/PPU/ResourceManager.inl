#pragma once
#include "ResourceManager.h"

#include "Logging/Logging.h"

#include "rftl/extension/enum_format.h"
#include "rftl/extension/transparent_lookup.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceManager()
	: mNextResourceID( IncrementID( kInvalidResourceID ) )
{
	//
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::~ResourceManager()
{
	// NOTE: Shutdown should've been called before this
	RF_ASSERT( mResourceIDs.empty() );
	RF_ASSERT( mResources.empty() );
	RF_ASSERT( mFileBackedResources.empty() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline WeakPtr<Resource const> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const
{
	return GetMutableResourceFromManagedResourceID( managedResourceID );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource const> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetResourceFromResourceName( Filename const& filename ) const
{
	return GetResourceFromResourceName( filename.CreateString() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource const> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetResourceFromResourceName( ResourceNameView resourceName ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );

	typename ResourceIDsByName::const_iterator IDIter = mResourceIDs.find( resourceName );
	if( IDIter == mResourceIDs.end() )
	{
		RFLOG_ERROR( resourceName, RFCAT_PPU, "Could not find resource by name" );
		return nullptr;
	}

	typename ResourcesByManagedID::const_iterator resourceIter = mResources.find( IDIter->second );
	if( resourceIter == mResources.end() )
	{
		RFLOG_NOTIFY( resourceName, RFCAT_PPU, "Lookups desynced?" );
		return nullptr;
	}

	return resourceIter->second;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetManagedResourceIDFromResourceName( Filename const& filename ) const
{
	return GetManagedResourceIDFromResourceName( filename.CreateString() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetManagedResourceIDFromResourceName( ResourceNameView resourceName ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );

	typename ResourceIDsByName::const_iterator IDIter = mResourceIDs.find( resourceName );
	if( IDIter == mResourceIDs.end() )
	{
		RFLOG_ERROR( resourceName, RFCAT_PPU, "Could not find resource by name" );
		return kInvalidResourceID;
	}

	return IDIter->second;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReserveNullResource( Filename const& filename )
{
	return ReserveNullResource( filename.CreateString() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReserveNullResource( ResourceNameView resourceName )
{
	return ReserveNullResourceInternal( resourceName );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadResult
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResource( Filename const& filename )
{
	return LoadNewResourceInternal( filename.CreateString(), filename );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadResult
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResource( ResourceNameView resourceName, Filename const& filename )
{
	return LoadNewResourceInternal( resourceName, filename );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadResult
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResource( ResourceNameView resourceName, UniquePtr<Resource>&& resource )
{
	return LoadNewResourceInternal( resourceName, rftl::move( resource ) );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::UpdateExistingResource( Filename const& filename )
{
	return UpdateExistingResource( filename.CreateString(), filename );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::UpdateExistingResource( ResourceNameView resourceName, Filename const& filename )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );
	return UpdateExistingResourceWithoutLock( resourceName, filename );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReloadExistingResource( Filename const& filename )
{
	return ReloadExistingResource( filename.CreateString() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReloadExistingResource( ResourceNameView resourceName )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );

	typename ResourcesByFilename::const_iterator fileIter = mFileBackedResources.find( resourceName );
	if( fileIter == mFileBackedResources.end() )
	{
		RFLOG_ERROR( resourceName, RFCAT_PPU, "Resource not found, or not file backed" );
		return false;
	}

	return UpdateExistingResourceWithoutLock( resourceName, fileIter->second );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DestroyResource( ResourceNameView resourceName )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );

	typename ResourceIDsByName::const_iterator IDIter = mResourceIDs.find( resourceName );
	if( IDIter == mResourceIDs.end() )
	{
		RFLOG_NOTIFY( resourceName, RFCAT_PPU, "Resource ID not found to destroy" );
		return false;
	}
	ManagedResourceID const managedResourceID = IDIter->second;

	typename ResourcesByManagedID::const_iterator resourceIter = mResources.find( managedResourceID );
	if( resourceIter == mResources.end() )
	{
		RFLOG_NOTIFY( resourceName, RFCAT_PPU, "Resource not found to destroy" );
		return false;
	}

	UniquePtr<Resource> const& resource = resourceIter->second;
	RF_ASSERT( resource != nullptr );

	bool const success = PreDestroy( resourceName, *resource );
	RF_ASSERT( success );

	RF_TODO_ANNOTATION(
		"Record information to know how many frames a resource was active for,"
		" to be able to spot unnecessary churn in unexpected cases" );

	rftl::transparent_erase( mFileBackedResources, resourceName );
	mResourceIDs.erase( IDIter );
	mResources.erase( resourceIter );

	RFLOG_INFO( resourceName, RFCAT_PPU, "Resource destroyed at {}", managedResourceID );
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceName ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForResourceNameByResourceID( ManagedResourceID managedResourceID ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	for( typename ResourceIDsByName::value_type const& resourcePair : mResourceIDs )
	{
		ManagedResourceID const& id = resourcePair.second;
		if( id == managedResourceID )
		{
			ResourceName const& name = resourcePair.first;
			return name;
		}
	}

	return ResourceName();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::Filename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForFilenameByResourceName( ResourceNameView resourceName ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );

	for( ResourcesByFilename::value_type const& resourcePair : mFileBackedResources )
	{
		ResourceName const& name = resourcePair.first;
		if( name == resourceName )
		{
			Filename const& file = resourcePair.second;
			return file;
		}
	}

	return Filename();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::Filename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForFilenameByResourceID( ManagedResourceID managedResourceID ) const
{
	return SearchForFilenameByResourceName( SearchForResourceNameByResourceID( managedResourceID ) );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
rftl::vector<typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceName> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DebugSearchForResourcesByFilename( Filename const& filename ) const
{
	RF_TODO_BREAK();
	return rftl::vector<ResourceName>();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DebugLockResourceForDirectModification( ManagedResourceID managedResourceID )
{
	RF_TODO_ANNOTATION( "Resource lock, and return a lock object, block normal access" );
	return GetMutableResourceFromManagedResourceID( managedResourceID );
}

///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::PostLoadFromFile( ResourceType& resource, Filename filename )
{
	(void)resource;
	(void)filename;
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::PostLoadFromMemory( ResourceType& resource )
{
	(void)resource;
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::PreDestroy( ResourceNameView resourceName, ResourceType& resource )
{
	(void)resource;
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline void ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::InternalShutdown()
{
	// NOTE: No lock taken, expect this to be a terminal action taken on only
	//  one thread, deep into complete app shutdown

	while( mResourceIDs.empty() == false )
	{
		// NOTE: Subtle, copy resource name so we don't pass by reference and
		//  have it deleted mid-function
		ResourceName const resourceNameCopy = mResourceIDs.begin()->first;
		DestroyResource( resourceNameCopy );
	}
	RF_ASSERT( mResources.empty() );
	RF_ASSERT( mFileBackedResources.empty() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline size_t ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetNumResources() const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	return mResources.size();
}

///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::FileBackedResourceRange ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForResourcesByFilenameInternal( Filename const& filename ) const
{
	RF_TODO_BREAK();
	return FileBackedResourceRange();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::IncrementID( ManagedResourceID id )
{
	static_assert(
		rftl::is_enum<ManagedResourceID>::value,
		"ID types are assumed to be enums to prevent accidentally mixing ID"
		" types between each other by accident" );
	return math::enum_bitcast<ManagedResourceID>( math::enum_bitcast( id ) + 1u );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GenerateNewManagedID()
{
	// A write lock should have already been taken
	RF_ASSERT( mMultiReaderSingleWriterLock.try_lock() == false );

	// Make naive increment attempt
	ManagedResourceID retVal = mNextResourceID;
	mNextResourceID = IncrementID( mNextResourceID );

	// Check that it hasn't rolled over
	if( retVal == kInvalidResourceID )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_PPU,
			"Resource IDs have rolled over, and will now be more expensive and"
			" more likely to expose ID collision bugs" );
		mResourceIDsHaveRolledOver = true;
	}

	if( mResourceIDsHaveRolledOver == false )
	{
		// Have yet to rollover, can assume generation is safe
		RF_ASSERT( retVal != kInvalidResourceID );
		RF_ASSERT( mResources.contains( retVal ) == false );
		return retVal;
	}

	// Spin and try to find something that is valid and also not currently in
	//  use already
	ManagedResourceIDType const originalAttempt = retVal;
	while( mResources.contains( retVal ) || retVal == kInvalidResourceID )
	{
		// Collision, try again
		retVal = mNextResourceID;
		mNextResourceID = IncrementID( mNextResourceID );

		if( retVal == originalAttempt )
		{
			// Total exhaustion
			RFLOG_FATAL( nullptr, RFCAT_PPU,
				"Resource IDs have not only rolled over, but have also been"
				" completely exhausted, and even the fallback logic has fallen"
				" into an infinite loop!" );
		}
	}

	// Have found something valid
	RF_ASSERT( retVal != kInvalidResourceID );
	RF_ASSERT( mResources.contains( retVal ) == false );
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetMutableResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const
{
	ReaderLock const lock( mMultiReaderSingleWriterLock );

	typename ResourcesByManagedID::const_iterator resourceIter = mResources.find( managedResourceID );
	if( resourceIter == mResources.end() )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Could not find resource by ID {}", managedResourceID );
		return nullptr;
	}

	WeakPtr<Resource> const& retVal = resourceIter->second;
	if( retVal == nullptr )
	{
		RFLOG_WARNING( nullptr, RFCAT_PPU,
			"Found resource {}, but it is null, which should indicate a"
			" reservation",
			managedResourceID );
		RF_DBGFAIL_MSG(
			"Seem to be trying to fetch a reserved resource that isn't loaded"
			" yet. Should this have been force-fetched? Is there data in it"
			" that is needed outside the render passes? Render passes should"
			" be able to survive missing resources while they're loading." );
	}
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReserveNullResourceInternal( ResourceNameView resourceName )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );

	typename ResourceIDsByName::const_iterator const existing = mResourceIDs.find( resourceName );
	if( mResourceIDs.count( resourceName ) != 0 )
	{
		// Already exists
		ManagedResourceID const existingID = existing->second;
		RF_ASSERT( existingID != kInvalidResourceID );
		return existingID;
	}

	ManagedResourceID const managedResourceID = GenerateNewManagedID();
	RF_ASSERT( managedResourceID != kInvalidResourceID );

	RegisterResource( managedResourceID, nullptr );
	RegisterResourceID( resourceName, managedResourceID );

	RFLOG_INFO( resourceName, RFCAT_PPU, "Null resource reserved at {}", managedResourceID );
	return managedResourceID;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadResult
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceInternal( ResourceNameView resourceName, Filename const& filename )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( filename.Empty() == false );

	// Check if it is already loaded
	{
		ResourcesByFilename::const_iterator const fileIter = mFileBackedResources.find( resourceName );
		if( fileIter != mFileBackedResources.end() )
		{
			Filename const& existingFilename = fileIter->second;
			if( filename == existingFilename )
			{
				// Already loaded
				RFLOG_TRACE( filename, RFCAT_PPU, "Resource already loaded, reusing" );
				RF_ASSERT( mResourceIDs.count( resourceName ) == 1 );
				ManagedResourceID const id = rftl::transparent_at( mResourceIDs, resourceName );
				RF_ASSERT( mResources.count( id ) == 1 );
				return LoadResult{ .mID = id, .mHandle = mResources.at( id ) };
			}
			else
			{
				RF_DBGFAIL_MSG( "Resource already loaded with different file" );
				return LoadResult{};
			}
		}
	}

	ManagedResourceID const managedResourceID = GenerateNewManagedID();

	RF_ASSERT( mResourceIDs.count( resourceName ) == 0 );
	RF_ASSERT( mResources.count( managedResourceID ) == 0 );
	RF_ASSERT( mFileBackedResources.count( resourceName ) == 0 );
	Resource* res;
	WeakPtr<Resource> handle;
	{
		UniquePtr<Resource> newResource = AllocateResourceFromFile( resourceName, filename );
		if( newResource == nullptr )
		{
			RFLOG_NOTIFY( filename, RFCAT_PPU, "Failed to allocate resource from file" );
			return LoadResult{};
		}
		handle = newResource;
		res = newResource;
		RegisterResource( managedResourceID, rftl::move( newResource ) );
		RegisterResourceID( resourceName, managedResourceID );
		RegisterFileBackedResource( resourceName, filename );
	}
	RF_ASSERT( handle != nullptr );

	bool const success = PostLoadFromFile( *res, filename );
	RF_ASSERT( success );
	RFLOG_INFO( filename, RFCAT_PPU, "Resource loaded from file at {}", managedResourceID );
	return LoadResult{ .mID = managedResourceID, .mHandle = handle };
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadResult
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceInternal( ResourceNameView resourceName, UniquePtr<Resource>&& resource )
{
	WriterLock const lock( mMultiReaderSingleWriterLock );

	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( resource != nullptr );
	ManagedResourceID const managedResourceID = GenerateNewManagedID();

	Resource* res;
	WeakPtr<Resource> handle;
	{
		handle = resource;
		res = resource;
		RegisterResource( managedResourceID, rftl::move( resource ) );
		RegisterResourceID( resourceName, managedResourceID );
	}
	RF_ASSERT( handle != nullptr );

	bool const success = PostLoadFromMemory( *res );
	RF_ASSERT( success );
	RFLOG_INFO( resourceName, RFCAT_PPU, "Resource loaded from memory at {}", managedResourceID );
	return LoadResult{ .mID = managedResourceID, .mHandle = handle };
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::UpdateExistingResourceWithoutLock( ResourceNameView resourceName, Filename const& filename )
{
	RF_ASSERT( resourceName.empty() == false );

	typename ResourceIDsByName::const_iterator IDIter = mResourceIDs.find( resourceName );
	if( IDIter == mResourceIDs.end() )
	{
		RFLOG_ERROR( resourceName, RFCAT_PPU, "Resource ID not found" );
		return false;
	}

	typename ResourcesByManagedID::iterator resourceIter = mResources.find( IDIter->second );
	if( resourceIter == mResources.end() )
	{
		RFLOG_ERROR( resourceName, RFCAT_PPU, "Resource not found" );
		return false;
	}

	UniquePtr<Resource>& resourceRef = resourceIter->second;
	if( resourceRef )
	{
		RFLOG_TRACE( resourceName, RFCAT_PPU, "Updating null resource, assumed to have been a reserve" );
	}

	// Attempt to load new
	UniquePtr<Resource> newResource = AllocateResourceFromFile( resourceName, filename );
	if( newResource == nullptr )
	{
		RFLOG_NOTIFY( filename, RFCAT_PPU, "Failed to allocate resource from file" );
		return false;
	}

	// Swap out old
	if( resourceRef != nullptr )
	{
		bool const preDestroySuccess = PreDestroy( resourceName, *resourceRef );
		RF_ASSERT( preDestroySuccess );
		resourceRef = nullptr;

		ResourcesByFilename::const_iterator const fileIter = mFileBackedResources.find( resourceName );
		if( fileIter != mFileBackedResources.end() )
		{
			if( fileIter->second == filename )
			{
				RFLOG_TRACE( resourceName, RFCAT_PPU, "Updating file-backed resource to same filename, assuming a reload" );
			}
			else
			{
				RFLOG_TRACE( resourceName, RFCAT_PPU, "Updating file-backed resource to different filename, assuming a modify" );
			}

			// Clear out file reference
			mFileBackedResources.erase( fileIter );
		}
		else
		{
			RFLOG_TRACE( resourceName, RFCAT_PPU, "Updating a non-file-backed resource to be file-backed" );
		}
	}
	RF_ASSERT( mFileBackedResources.count( resourceName ) == 0 );

	// Swap in new
	Resource* const res = newResource;
	resourceRef = rftl::move( newResource );
	RegisterFileBackedResource( resourceName, filename );
	bool const postLoadSuccess = PostLoadFromFile( *res, filename );
	RF_ASSERT( postLoadSuccess );
	RFLOG_INFO( filename, RFCAT_PPU, "Resource loaded from file" );

	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline void ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::RegisterResource( ManagedResourceID managedResourceID, UniquePtr<Resource>&& resource )
{
	RF_ASSERT( managedResourceID != kInvalidResourceID );
	// NOTE: Resource is allowed to be null, used when reserving null resources
	RF_ASSERT( mResources.count( managedResourceID ) == 0 );
	mResources.emplace( managedResourceID, rftl::move( resource ) );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline void ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::RegisterResourceID( ResourceNameView resourceName, ManagedResourceID managedResourceID )
{
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( managedResourceID != kInvalidResourceID );
	RF_ASSERT( mResourceIDs.count( resourceName ) == 0 );
	mResourceIDs.emplace( resourceName, managedResourceID );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline void ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::RegisterFileBackedResource( ResourceNameView resourceName, Filename const& filename )
{
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( filename.Empty() == false );
	RF_ASSERT( mFileBackedResources.count( resourceName ) == 0 );
	for( ResourcesByFilename::value_type const& entry : mFileBackedResources )
	{
		if( entry.second == filename )
		{
			// At time of writing, the resource manager permits duplicate
			//  resources that are both backed by the same file, but this is
			//  still potentially undesirable or wasteful, so it's called out
			//  as a warning here to make sure it's intentional
			RFLOG_WARNING( filename, RFCAT_PPU,
				"Resource '{}' is registering file already registered by resource '{}'",
				resourceName,
				entry.first );
		}
	}
	mFileBackedResources.emplace( resourceName, filename );
}

///////////////////////////////////////////////////////////////////////////////
}

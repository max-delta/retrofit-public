#pragma once
#include "ResourceManager.h"

#include "Logging/Logging.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceManager()
	: m_NextResourceID( k_InvalidResourceID + 1 )
{
	//
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::~ResourceManager()
{
	// NOTE: Shutdown should've been called before this
	RF_ASSERT( m_ResourceIDs.empty() );
	RF_ASSERT( m_Resources.empty() );
	RF_ASSERT( m_FileBackedResources.empty() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const
{
	ResourcesByManagedID::const_iterator resourceIter = m_Resources.find( managedResourceID );
	if( resourceIter == m_Resources.end() )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Could not find ID" );
		return nullptr;
	}

	return resourceIter->second;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetResourceFromResourceName( ResourceName const& resourceName ) const
{
	ResourceIDsByName::const_iterator IDIter = m_ResourceIDs.find( resourceName );
	if( IDIter == m_ResourceIDs.end() )
	{
		return nullptr;
	}

	ResourcesByManagedID::const_iterator resourceIter = m_Resources.find( IDIter->second );
	if( resourceIter == m_Resources.end() )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_PPU, "Lookups desynced?" );
		return nullptr;
	}

	return resourceIter->second;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResource( ResourceName const& resourceName, Filename const& filename )
{
	WeakPtr<Resource> handle = LoadNewResourceGetHandle( resourceName, filename );
	return handle != nullptr;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResource( ResourceName const & resourceName, UniquePtr<Resource>&& resource )
{
	RF_DBGFAIL_MSG( "TODO" );
	return false;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceGetID( ResourceName const& resourceName, Filename const& filename )
{
	ManagedResourceID retVal;
	LoadNewResourceInternal( resourceName, filename, retVal );
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceGetID( ResourceName const& resourceName, UniquePtr<Resource> && resource )
{
	ManagedResourceID retVal;
	LoadNewResourceInternal( resourceName, rftl::move( resource ), retVal );
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceGetHandle( ResourceName const& resourceName, Filename const& filename )
{
	ManagedResourceID unused;
	return LoadNewResourceInternal( resourceName, filename, unused );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceGetHandle( ResourceName const& resourceName, UniquePtr<Resource> && resource )
{
	ManagedResourceID unused;
	return LoadNewResourceInternal( resourceName, rftl::move( resource ), unused );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::UpdateExistingResource( ResourceName const & resourceName, Filename const & filename )
{
	RF_DBGFAIL_MSG( "TODO" );
	return false;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ReloadExistingResource( ResourceName const & resourceName )
{
	RF_DBGFAIL_MSG( "TODO" );
	return false;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DestroyResource( ResourceName const& resourceName )
{
	ResourceIDsByName::const_iterator IDIter = m_ResourceIDs.find( resourceName );
	if( IDIter == m_ResourceIDs.end() )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Resource ID not found" );
		return false;
	}

	ResourcesByManagedID::const_iterator resourceIter = m_Resources.find( IDIter->second );
	if( resourceIter == m_Resources.end() )
	{
		RFLOG_ERROR( nullptr, RFCAT_PPU, "Resource not found" );
		return false;
	}

	UniquePtr<Resource> const& resource = resourceIter->second;
	RF_ASSERT( resource != nullptr );

	bool const success = PreDestroy( *resource );
	RF_ASSERT( success );

	m_FileBackedResources.erase( resourceName );
	m_ResourceIDs.erase( IDIter );
	m_Resources.erase( resourceIter );
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceName ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForResourceNameByResourceID( ManagedResourceID managedResourceID ) const
{
	for( ResourceIDsByName::value_type const& resourcePair : m_ResourceIDs )
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
typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::Filename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForFilenameByResourceName( ResourceName const & resourceName ) const
{
	for( ResourcesByFilename::value_type const& resourcePair : m_FileBackedResources )
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
rftl::vector<typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::ResourceName> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DebugSearchForResourcesByFilename( Filename const & filename ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return rftl::vector<ResourceName>();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::DebugLockResourceForDirectModification( ManagedResourceID managedResourceID )
{
	// TODO: Resource lock, and return a lock object, block normal access
	return GetResourceFromManagedResourceID( managedResourceID );
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
bool ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::PreDestroy( ResourceType & resource )
{
	(void)resource;
	return true;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline void ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::InternalShutdown()
{
	while( m_ResourceIDs.empty() == false )
	{
		DestroyResource( m_ResourceIDs.begin()->first );
	}
	RF_ASSERT( m_Resources.empty() );
	RF_ASSERT( m_FileBackedResources.empty() );
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
inline size_t ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GetNumResources() const
{
	return m_Resources.size();
}

///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
typename ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::FileBackedResourceRange ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::SearchForResourcesByFilenameInternal( Filename const & filename ) const
{
	RF_DBGFAIL_MSG( "TODO" );
	return FileBackedResourceRange();
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
ManagedResourceID ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::GenerateNewManagedID()
{
	ManagedResourceID retVal = m_NextResourceID++;
	if( m_NextResourceID == k_InvalidResourceID )
	{
		RF_DBGFAIL_MSG( "TODO: Safe generation" );
	}
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceInternal( ResourceName const & resourceName, Filename const & filename, ManagedResourceID& managedResourceID )
{
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( filename.Empty() == false );
	managedResourceID = InvalidResourceID;

	// Check if it is already loaded
	{
		ResourcesByFilename::const_iterator const fileIter = m_FileBackedResources.find( resourceName );
		if( fileIter != m_FileBackedResources.end() )
		{
			Filename const& existingFilename = fileIter->second;
			if( filename == existingFilename )
			{
				// Already loaded
				RFLOG_TRACE( filename, RFCAT_PPU, "Resource already loaded, reusing" );
				RF_ASSERT( m_ResourceIDs.count( resourceName ) == 1 );
				ManagedResourceID const id = m_ResourceIDs.at( resourceName );
				RF_ASSERT( m_Resources.count( id ) == 1 );
				managedResourceID = id;
				return m_Resources.at( id );
			}
			else
			{
				RF_DBGFAIL_MSG( "Resource already loaded with different file" );
				managedResourceID = k_InvalidResourceID;
				return nullptr;
			}
		}
	}

	ManagedResourceID tempID = GenerateNewManagedID();

	RF_ASSERT( m_ResourceIDs.count( resourceName ) == 0 );
	RF_ASSERT( m_Resources.count( managedResourceID ) == 0 );
	RF_ASSERT( m_FileBackedResources.count( resourceName ) == 0 );
	Resource* res;
	WeakPtr<Resource> retVal;
	{
		UniquePtr<Resource> newResource = AllocateResourceFromFile( filename );
		if( newResource == nullptr )
		{
			RFLOG_NOTIFY( filename, RFCAT_PPU, "Failed to allocate resource from file" );
			return nullptr;
		}
		retVal = newResource;
		res = newResource;
		managedResourceID = tempID;
		m_Resources.emplace( managedResourceID, rftl::move( newResource ) );
		m_ResourceIDs.emplace( resourceName, managedResourceID );
		m_FileBackedResources.emplace( resourceName, filename );
	}
	RF_ASSERT( retVal != nullptr );

	bool const success = PostLoadFromFile( *res, filename );
	RF_ASSERT( success );
	RFLOG_INFO( filename, RFCAT_PPU, "Resource loaded from file" );
	return retVal;
}



template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
WeakPtr<Resource> ResourceManager<Resource, ManagedResourceID, InvalidResourceID>::LoadNewResourceInternal( ResourceName const & resourceName, UniquePtr<Resource>&& resource, ManagedResourceID& managedResourceID )
{
	RF_ASSERT( resourceName.empty() == false );
	RF_ASSERT( resource != nullptr );
	managedResourceID = GenerateNewManagedID();

	RF_ASSERT( m_ResourceIDs.count( resourceName ) == 0 );
	RF_ASSERT( m_Resources.count( managedResourceID ) == 0 );
	Resource* res;
	WeakPtr<Resource> retVal;
	{
		retVal = resource;
		res = resource;
		m_Resources.emplace( managedResourceID, rftl::move( resource ) );
		m_ResourceIDs.emplace( resourceName, managedResourceID );
	}
	RF_ASSERT( retVal != nullptr );

	bool const success = PostLoadFromMemory( *res );
	RF_ASSERT( success );
	RFLOG_INFO( nullptr, RFCAT_PPU, "Resource loaded from memory" );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

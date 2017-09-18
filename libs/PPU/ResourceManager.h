#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"

#include <string>
#include <unordered_map>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
class ResourceManager
{
	RF_NO_COPY( ResourceManager );

	//
	// Types
public:
	typedef Resource ResourceType;
	typedef ManagedResourceID ManagedResourceIDType;
	typedef file::VFSPath Filename;
	typedef std::string ResourceName;
	typedef ResourceManager<Resource, ManagedResourceID, InvalidResourceID> ResourceManagerType;

	typedef std::unordered_map<ManagedResourceID, UniquePtr<Resource>> ResourcesByManagedID;
	typedef std::unordered_map<ResourceName, ManagedResourceID> ResourceIDsByName;
	typedef std::unordered_map<ResourceName, Filename> ResourcesByFilename;
private:
	typedef std::pair<ResourcesByFilename::const_iterator, ResourcesByFilename::const_iterator> FileBackedResourceRange;


	//
	// Constants
public:
	static constexpr ManagedResourceID k_InvalidResourceID = InvalidResourceID;


	//
	// Public methods
public:
	ResourceManager();
	virtual ~ResourceManager();

	WeakPtr<Resource> GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const;
	WeakPtr<Resource> GetResourceFromResourceName( ResourceName const& resourceName ) const;

	Filename GetFilenameFromResourceName( ResourceName const& resourceName ) const;

	bool LoadNewResource( ResourceName const& resourceName, Filename const& filename );
	bool LoadNewResource( ResourceName const& resourceName, UniquePtr<Resource> && resource );
	ManagedResourceID LoadNewResourceGetID( ResourceName const& resourceName, Filename const& filename );
	ManagedResourceID LoadNewResourceGetID( ResourceName const& resourceName, UniquePtr<Resource> && resource );
	WeakPtr<Resource> LoadNewResourceGetHandle( ResourceName const& resourceName, Filename const& filename );
	WeakPtr<Resource> LoadNewResourceGetHandle( ResourceName const& resourceName, UniquePtr<Resource> && resource );

	bool UpdateExistingResource( ResourceName const& resourceName, Filename const& filename );
	bool ReloadExistingResource( ResourceName const& resourceName );
	bool DestroyResource( ResourceName const& resourceName );

	std::vector<ResourceName> DebugSearchForResourcesByFilename( Filename const& filename ) const;
	WeakPtr<Resource> DebugLockResourceForDirectModification( ManagedResourceID managedResourceID );


	//
	// Protected methods
protected:
	virtual UniquePtr<Resource> AllocateResourceFromFile( Filename const & filename ) = 0;
	virtual bool PostLoadFromFile( ResourceType& resource, Filename filename );
	virtual bool PostLoadFromMemory( ResourceType& resource );
	virtual bool PreDestroy( ResourceType& resource );

	// IMPORTANT: Derived classes must call this during deconstructor
	void InternalShutdown();

	size_t GetNumResources() const;


	//
	// Private methods
private:
	FileBackedResourceRange SearchForResourcesByFilenameInternal( Filename const& filename ) const;
	ManagedResourceID GenerateNewManagedID();

	WeakPtr<Resource> LoadNewResourceInternal( ResourceName const& resourceName, Filename const& filename, ManagedResourceID& managedResourceID );
	WeakPtr<Resource> LoadNewResourceInternal( ResourceName const& resourceName, UniquePtr<Resource> && resource, ManagedResourceID& managedResourceID );


	//
	// Private data
private:
	ManagedResourceID m_NextResourceID;
	ResourcesByManagedID m_Resources;
	ResourceIDsByName m_ResourceIDs;
	ResourcesByFilename m_FileBackedResources;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "ResourceManager.inl"

#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_map"
#include "rftl/shared_mutex"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

class ResourceManagerBase
{
	RF_NO_COPY( ResourceManagerBase );

	//
	// Types and constants
public:
	using Filename = file::VFSPath;
	using ResourceName = rftl::string;

protected:
	using ResourcesByFilename = rftl::unordered_map<ResourceName, Filename>;
	using FileBackedResourceRange = rftl::pair<ResourcesByFilename::const_iterator, ResourcesByFilename::const_iterator>;
	using ReaderWriterMutex = rftl::shared_mutex;
	using ReaderLock = rftl::shared_lock<rftl::shared_mutex>;
	using WriterLock = rftl::unique_lock<rftl::shared_mutex>;


	//
	// Protected methods
protected:
	ResourceManagerBase() = default;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Resource, typename ManagedResourceID, ManagedResourceID InvalidResourceID>
class ResourceManager : public ResourceManagerBase
{
	RF_NO_COPY( ResourceManager );

	//
	// Types and constants
public:
	static constexpr ManagedResourceID kInvalidResourceID = InvalidResourceID;

protected:
	using ResourceType = Resource;
	using ManagedResourceIDType = ManagedResourceID;
	using ResourceManagerType = ResourceManager<Resource, ManagedResourceID, InvalidResourceID>;
	using ResourcesByManagedID = rftl::unordered_map<ManagedResourceID, UniquePtr<Resource>>;
	using ResourceIDsByName = rftl::unordered_map<ResourceName, ManagedResourceID>;


	//
	// Public methods
public:
	ResourceManager();
	virtual ~ResourceManager();

	WeakPtr<Resource> GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const;
	WeakPtr<Resource> GetResourceFromResourceName( Filename const& filename ) const;
	WeakPtr<Resource> GetResourceFromResourceName( ResourceName const& resourceName ) const;
	ManagedResourceID GetManagedResourceIDFromResourceName( Filename const& filename ) const;
	ManagedResourceID GetManagedResourceIDFromResourceName( ResourceName const& resourceName ) const;

	bool LoadNewResource( Filename const& filename );
	bool LoadNewResource( ResourceName const& resourceName, Filename const& filename );
	bool LoadNewResource( ResourceName const& resourceName, UniquePtr<Resource>&& resource );
	ManagedResourceID LoadNewResourceGetID( Filename const& filename );
	ManagedResourceID LoadNewResourceGetID( ResourceName const& resourceName, Filename const& filename );
	ManagedResourceID LoadNewResourceGetID( ResourceName const& resourceName, UniquePtr<Resource>&& resource );
	WeakPtr<Resource> LoadNewResourceGetHandle( Filename const& filename );
	WeakPtr<Resource> LoadNewResourceGetHandle( ResourceName const& resourceName, Filename const& filename );
	WeakPtr<Resource> LoadNewResourceGetHandle( ResourceName const& resourceName, UniquePtr<Resource>&& resource );

	bool UpdateExistingResource( ResourceName const& resourceName, Filename const& filename );
	bool ReloadExistingResource( ResourceName const& resourceName );
	bool DestroyResource( ResourceName const& resourceName );

	ResourceName SearchForResourceNameByResourceID( ManagedResourceID managedResourceID ) const;
	Filename SearchForFilenameByResourceName( ResourceName const& resourceName ) const;
	Filename SearchForFilenameByResourceID( ManagedResourceID managedResourceID ) const;
	rftl::vector<ResourceName> DebugSearchForResourcesByFilename( Filename const& filename ) const;
	WeakPtr<Resource> DebugLockResourceForDirectModification( ManagedResourceID managedResourceID );


	//
	// Protected methods
protected:
	virtual UniquePtr<Resource> AllocateResourceFromFile( Filename const& filename ) = 0;
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
	WeakPtr<Resource> LoadNewResourceInternal( ResourceName const& resourceName, UniquePtr<Resource>&& resource, ManagedResourceID& managedResourceID );
	bool UpdateExistingResourceWithoutLock( ResourceName const& resourceName, Filename const& filename );


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;

	ManagedResourceID mNextResourceID;
	ResourcesByManagedID mResources;
	ResourceIDsByName mResourceIDs;
	ResourcesByFilename mFileBackedResources;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "ResourceManager.inl"

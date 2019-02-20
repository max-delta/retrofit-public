#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/string"
#include "rftl/unordered_map"
#include "rftl/shared_mutex"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class ResourceManagerBase
{
	RF_NO_COPY( ResourceManagerBase );

	//
	// Types
public:
	typedef file::VFSPath Filename;
	typedef rftl::string ResourceName;
	typedef rftl::unordered_map<ResourceName, Filename> ResourcesByFilename;
protected:
	typedef rftl::pair<ResourcesByFilename::const_iterator, ResourcesByFilename::const_iterator> FileBackedResourceRange;
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
	// Types
public:
	typedef Resource ResourceType;
	typedef ManagedResourceID ManagedResourceIDType;
	typedef ResourceManager<Resource, ManagedResourceID, InvalidResourceID> ResourceManagerType;
	typedef rftl::unordered_map<ManagedResourceID, UniquePtr<Resource>> ResourcesByManagedID;
	typedef rftl::unordered_map<ResourceName, ManagedResourceID> ResourceIDsByName;


	//
	// Constants
public:
	static constexpr ManagedResourceID kInvalidResourceID = InvalidResourceID;


	//
	// Public methods
public:
	ResourceManager();
	virtual ~ResourceManager();

	WeakPtr<Resource> GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const;
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
}}

#include "ResourceManager.inl"

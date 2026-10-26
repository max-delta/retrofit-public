#pragma once
#include "project.h"

#include "PPU/ResourceLoadResults.h"

#include "PlatformFilesystem/VFSPath.h"

#include "core/ptr/unique_ptr.h"

#include "rftl/extension/string_hash.h"
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
	using ResourceNameView = rftl::string_view;

protected:
	using ResourcesByFilename = rftl::unordered_map<
		ResourceName,
		Filename,
		rftl::string_hash,
		rftl::equal_to<>>;
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
	using LoadResult = ResourceLoadResult<Resource, ManagedResourceID, kInvalidResourceID>;

protected:
	using ResourceType = Resource;
	using ManagedResourceIDType = ManagedResourceID;
	using ResourceManagerType = ResourceManager<Resource, ManagedResourceID, kInvalidResourceID>;
	using ResourcesByManagedID = rftl::unordered_map<ManagedResourceID, UniquePtr<Resource>>;
	using ResourceIDsByName = rftl::unordered_map<ResourceName, ManagedResourceID, rftl::string_hash, rftl::equal_to<>>;


	//
	// Public methods
public:
	ResourceManager();
	virtual ~ResourceManager();

	WeakPtr<Resource const> GetResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const;
	WeakPtr<Resource const> GetResourceFromResourceName( Filename const& filename ) const;
	WeakPtr<Resource const> GetResourceFromResourceName( ResourceNameView resourceName ) const;
	ManagedResourceID GetManagedResourceIDFromResourceName( Filename const& filename ) const;
	ManagedResourceID GetManagedResourceIDFromResourceName( ResourceNameView resourceName ) const;

	ManagedResourceID ReserveNullResource( Filename const& filename );
	ManagedResourceID ReserveNullResource( ResourceNameView resourceName );

	LoadResult LoadNewResource( Filename const& filename );
	LoadResult LoadNewResource( ResourceNameView resourceName, Filename const& filename );
	LoadResult LoadNewResource( ResourceNameView resourceName, UniquePtr<Resource>&& resource );

	bool UpdateExistingResource( Filename const& filename );
	bool UpdateExistingResource( ResourceNameView resourceName, Filename const& filename );
	bool ReloadExistingResource( Filename const& filename );
	bool ReloadExistingResource( ResourceNameView resourceName );
	bool DestroyResource( ResourceNameView resourceName );

	ResourceName SearchForResourceNameByResourceID( ManagedResourceID managedResourceID ) const;
	Filename SearchForFilenameByResourceName( ResourceNameView resourceName ) const;
	Filename SearchForFilenameByResourceID( ManagedResourceID managedResourceID ) const;
	rftl::vector<ResourceName> DebugSearchForResourcesByFilename( Filename const& filename ) const;
	WeakPtr<Resource> DebugLockResourceForDirectModification( ManagedResourceID managedResourceID );


	//
	// Protected methods
protected:
	virtual UniquePtr<Resource> AllocateResourceFromFile( ResourceNameView resourceName, Filename const& filename ) = 0;
	virtual bool PostLoadFromFile( ResourceType& resource, Filename filename );
	virtual bool PostLoadFromMemory( ResourceType& resource );
	virtual bool PreDestroy( ResourceNameView resourceName, ResourceType& resource );

	// IMPORTANT: Derived classes must call this during destructor
	void InternalShutdown();

	size_t GetNumResources() const;


	//
	// Private methods
private:
	FileBackedResourceRange SearchForResourcesByFilenameInternal( Filename const& filename ) const;

	static ManagedResourceID IncrementID( ManagedResourceID id );
	ManagedResourceID GenerateNewManagedID();

	WeakPtr<Resource> GetMutableResourceFromManagedResourceID( ManagedResourceID managedResourceID ) const;
	ManagedResourceID ReserveNullResourceInternal( ResourceNameView resourceName );
	LoadResult LoadNewResourceInternal( ResourceNameView resourceName, Filename const& filename );
	LoadResult LoadNewResourceInternal( ResourceNameView resourceName, UniquePtr<Resource>&& resource );
	bool UpdateExistingResourceWithoutLock( ResourceNameView resourceName, Filename const& filename );

	void RegisterResource( ManagedResourceID managedResourceID, UniquePtr<Resource>&& resource );
	void RegisterResourceID( ResourceNameView resourceName, ManagedResourceID managedResourceID );
	void RegisterFileBackedResource( ResourceNameView resourceName, Filename const& filename );


	//
	// Private data
private:
	mutable ReaderWriterMutex mMultiReaderSingleWriterLock;

	ManagedResourceID mNextResourceID;
	bool mResourceIDsHaveRolledOver = false;

	ResourcesByManagedID mResources;
	ResourceIDsByName mResourceIDs;
	ResourcesByFilename mFileBackedResources;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "ResourceManager.inl"

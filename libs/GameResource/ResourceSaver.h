#pragma once
#include "project.h"

#include "GameResource/ResourceFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core_reflect/ReflectFwd.h"

#include "rftl/string"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

class GAMERESOURCE_API ResourceSaver
{
	RF_NO_COPY( ResourceSaver );

	//
	// Types and constants
private:
	static constexpr char kRootVariableName[] = "resource";


	//
	// Public methods
public:
	ResourceSaver(
		WeakPtr<file::VFS const> vfs,
		WeakPtr<ResourceTypeRegistry const> typeRegistry );
	~ResourceSaver();

	// Save class (typed)
	template<typename ReflectedClass>
	bool SaveClassToFile(
		ResourceTypeIdentifier typeID,
		ReflectedClass const& classInstance,
		file::VFSPath const& path );
	template<typename ReflectedClass>
	bool SaveClassToBuffer(
		ResourceTypeIdentifier typeID,
		ReflectedClass const& classInstance,
		rftl::string& buffer );

	// Save class (typeless)
	bool SaveClassToFile(
		reflect::ClassInfo const& classInfo,
		void const* classInstance,
		ResourceTypeIdentifier typeID,
		file::VFSPath const& path );
	bool SaveClassToBuffer(
		reflect::ClassInfo const& classInfo,
		void const* classInstance,
		ResourceTypeIdentifier typeID,
		rftl::string& buffer );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;
	WeakPtr<ResourceTypeRegistry const> mTypeRegistry;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "ResourceSaver.inl"

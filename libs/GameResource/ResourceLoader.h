#pragma once
#include "project.h"

#include "GameResource/ResourceFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/string"
#include "rftl/string_view"
#include "rftl/unordered_map"
#include "rftl/unordered_set"


namespace RF::script {
class OOLoader;
}

namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

class GAMERESOURCE_API ResourceLoader
{
	RF_NO_COPY( ResourceLoader );

	//
	// Types and constants
private:
	static constexpr char kRootVariableName[] = "resource";
	using RFTypeNames = rftl::unordered_set<rftl::string>;
	using ResourceClasses = rftl::unordered_map<ResourceTypeIdentifier, RFTypeNames>;

	//
	// Public methods
public:
	ResourceLoader( WeakPtr<file::VFS const> vfs );
	~ResourceLoader();

	// Only explicitly injected types can be loaded, other types will cause
	//  load errors when referenced
	// NOTE: Uses type name as registered with RFType
	void AddResourceClass(
		ResourceTypeIdentifier typeID,
		char const* typeName );

	// Load class
	template<typename ReflectedClass, template<typename> typename Creator>
	UniquePtr<ReflectedClass> LoadClassFromFile(
		ResourceTypeIdentifier typeID,
		file::VFSPath const& path );
	template<typename ReflectedClass, template<typename> typename Creator>
	UniquePtr<ReflectedClass> LoadClassFromBuffer(
		ResourceTypeIdentifier typeID,
		rftl::string_view buffer );

	// Populate class
	template<typename ReflectedClass>
	bool PopulateClassFromFile(
		ResourceTypeIdentifier typeID,
		ReflectedClass& classInstance,
		file::VFSPath const& path );
	template<typename ReflectedClass>
	bool PopulateClassFromBuffer(
		ResourceTypeIdentifier typeID,
		ReflectedClass& classInstance,
		rftl::string_view buffer );


	//
	// Private methods
private:
	void InjectTypes(
		script::OOLoader& loader,
		ResourceTypeIdentifier typeID );
	bool AddSource(
		script::OOLoader& loader,
		file::VFSPath const& path );
	bool AddSource(
		script::OOLoader& loader,
		rftl::string_view buffer );

	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;

	ResourceClasses mResourceClasses;
};

///////////////////////////////////////////////////////////////////////////////
}

#include "ResourceLoader.inl"

#include "stdafx.h"
#include "ResourceLoad.h"

#include "cc3o3/Common.h"
#include "cc3o3/overworld/OverworldDesc.h"
#include "cc3o3/save/SaveBlob.h"

#include "GameResource/ResourceLoader.h"

#include "core/ptr/default_creator.h"


namespace RF::cc::resource {
///////////////////////////////////////////////////////////////////////////////
namespace details {

namespace type {
enum : ResourceTypeIdentifier
{
	Invalid = kInvalidResourceTypeIdentifier,
	SaveBlob,
	OverworldDesc
};
}



template<typename T>
UniquePtr<T> LoadFromFile( ResourceTypeIdentifier typeID, file::VFSPath const& path )
{
	return gResourceLoader->LoadClassFromFile<T, DefaultCreator>( typeID, path );
}

}
///////////////////////////////////////////////////////////////////////////////

void InitializeLoader()
{
	ResourceLoader& loader = *gResourceLoader;

	{
		using namespace details::type;
		loader.AddResourceClass( SaveBlob, "SaveBlob" );
		loader.AddResourceClass( SaveBlob, "SaveFileBlob" );
		loader.AddResourceClass( OverworldDesc, "OverworldDesc" );
	}
}

///////////////////////////////////////////////////////////////////////////////

#define RF_LOADER( TYPE_ID, TYPE ) \
	template<> \
	UniquePtr<TYPE> LoadFromFile<TYPE>( file::VFSPath const& path ) \
	{ \
		return details::LoadFromFile<TYPE>( TYPE_ID, path ); \
	} \
	template<> \
	UniquePtr<TYPE const> LoadFromFile<TYPE const>( file::VFSPath const& path ) \
	{ \
		return LoadFromFile<TYPE>( path ); \
	}

RF_LOADER( details::type::SaveBlob, save::SaveBlob );
RF_LOADER( details::type::OverworldDesc, overworld::OverworldDesc );

#undef RF_LOADER

///////////////////////////////////////////////////////////////////////////////
}

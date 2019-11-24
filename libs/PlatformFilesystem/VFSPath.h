#pragma once
#include "project.h"

#include "VFSFwd.h"

#include "Logging/Logging.h"

#include "core_identifier/SegmentedIdentifier.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class VFSPath : public id::SegmentedIdentifier<rftl::string>
{
	//
	// Friends
private:
	friend class VFS;

	//
	// Public methods
public:
	PLATFORMFILESYSTEM_API VFSPath();
	PLATFORMFILESYSTEM_API VFSPath( SegmentedIdentifier<rftl::string> const& path );
	template<typename... Nodes>
	explicit VFSPath( Element element, Nodes... elements );

	// To/from string
	PLATFORMFILESYSTEM_API static VFSPath CreatePathFromString( rftl::string const& path );
	PLATFORMFILESYSTEM_API rftl::string CreateString() const;
};

///////////////////////////////////////////////////////////////////////////////
}}

namespace RF { namespace logging {
template<>
void PLATFORMFILESYSTEM_API WriteContextString( file::VFSPath const& context, Utf8LogContextBuffer& buffer );
}}

template<>
struct PLATFORMFILESYSTEM_API rftl::hash<RF::file::VFSPath> : public rftl::hash<RF::id::SegmentedIdentifier<rftl::string>>
{
	//
};

#include "VFSPath.inl"

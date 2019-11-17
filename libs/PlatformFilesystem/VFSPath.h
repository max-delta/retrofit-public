#pragma once
#include "project.h"

#include "VFSFwd.h"

#include "Logging/Logging.h"

#include "core_identifier/SegmentedIdentifier.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFSPath : public id::SegmentedIdentifier<rftl::string>
{
	//
	// Friends
private:
	friend class VFS;

	//
	// Public methods
public:
	VFSPath();
	VFSPath( SegmentedIdentifier<rftl::string> const& path );
	template<typename... Nodes>
	explicit VFSPath( Element element, Nodes... elements );

	// To/from string
	static VFSPath CreatePathFromString( rftl::string const& path );
	rftl::string CreateString() const;
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

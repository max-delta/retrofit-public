#pragma once
#include "project.h"

#include "core/macros.h"
#include "core/meta/ConstructorOverload.h"

#include "rftl/vector"


// Forwards
namespace RF { namespace file {
	class FileHandle;
}}

namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API FileBuffer
{
	RF_NO_COPY( FileBuffer );

	//
	// Public methods
public:
	FileBuffer() = delete;
	explicit FileBuffer( ExplicitDefaultConstruct );
	explicit FileBuffer( FileHandle const& file, bool addTerminatingNull );
	FileBuffer( FileBuffer&& rhs );
	~FileBuffer() = default;
	FileBuffer& operator=( FileBuffer&& rhs );

	void const* GetData() const;
	size_t GetSize() const;


	//
	// Private data
private:
	rftl::vector<uint8_t> mBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}}

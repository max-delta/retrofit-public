#pragma once
#include "project.h"

#include "core/macros.h"
#include "core/meta/ConstructorOverload.h"

#include "rftl/extension/byte_view.h"
#include "rftl/string_view"
#include "rftl/vector"


// Forwards
namespace RF::file {
class SeekHandle;
}

namespace RF::file {
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API FileBuffer
{
	RF_NO_COPY( FileBuffer );

	//
	// Public methods
public:
	FileBuffer() = delete;
	explicit FileBuffer( ExplicitDefaultConstruct );
	explicit FileBuffer( SeekHandle& file, bool addTerminatingNull );
	explicit FileBuffer( SeekHandle& file, size_t maxBytes );
	FileBuffer( FileBuffer&& rhs );
	~FileBuffer() = default;
	FileBuffer& operator=( FileBuffer&& rhs );

	bool IsEmpty() const;
	size_t GetSize() const;

	rftl::byte_view GetBytes() const;
	rftl::string_view GetChars() const;


	//
	// Private data
private:
	rftl::vector<uint8_t> mBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}

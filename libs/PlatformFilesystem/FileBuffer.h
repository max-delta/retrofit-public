#pragma once
#include "project.h"

#include "core/macros.h"

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
	RF_NO_MOVE( FileBuffer );

	//
	// Public methods
public:
	FileBuffer() = delete;
	explicit FileBuffer( FileHandle const& file, bool addTerminatingNull );
	~FileBuffer() = default;

	void const* GetData() const;
	size_t GetSize() const;


	//
	// Private data
private:
	rftl::vector<uint8_t> mBuffer;
};

///////////////////////////////////////////////////////////////////////////////
}}

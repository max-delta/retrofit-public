#pragma once
#include "project.h"

#include "Serialization/Importer.h"

#include "core/macros.h"
#include "core/ptr/unique_ptr.h"

#include "rftl/string_view"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API AutoImporter final : public Importer
{
	RF_NO_COPY( AutoImporter );

public:
	// The longest byte sequence needed to peek for all supported types
	static constexpr size_t kLongestPeekMagicBytes = 5; // '<?xml'


public:
	AutoImporter();
	~AutoImporter() override;

	// Check initial bytes to see if it looks like it would be supported
	static bool LooksLikeSupportedType( rftl::string_view const& string );

	bool ReadFromString( rftl::string_view const& string );

	virtual bool ImportAndFinalize( Callbacks const& callbacks ) override;


private:
	UniquePtr<Importer> mUnderlying;
};

///////////////////////////////////////////////////////////////////////////////
}

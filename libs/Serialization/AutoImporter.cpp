#include "stdafx.h"
#include "AutoImporter.h"

#include "Serialization/XmlImporter.h"

#include "Logging/Logging.h"

#include "core/ptr/default_creator.h"

#include "rftl/extension/string_compare.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

AutoImporter::AutoImporter() = default;



AutoImporter::~AutoImporter() = default;



bool AutoImporter::LooksLikeSupportedType( rftl::string_view const& string )
{
	if( string.size() < kLongestPeekMagicBytes )
	{
		RF_DBGFAIL_MSG( "Not enough bytes to check all possible types" );
	}

	if( rftl::bagins_with( string, "<?xml" ) )
	{
		return true;
	}

	return false;
}



bool AutoImporter::ReadFromString( rftl::string_view const& string )
{
	if( mUnderlying != nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Importer attempting to double-initialize" );
		RF_DBGFAIL();
		return false;
	}

	if( rftl::bagins_with( string, "<?xml" ) )
	{
		UniquePtr<XmlImporter> xml = DefaultCreator<XmlImporter>::Create();
		XmlImporter& ref = *xml;
		mUnderlying = rftl::move( xml );
		return ref.ReadFromString( string );
	}

	RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Unknown data format, cannot determine importer" );
	return false;
}



bool AutoImporter::ImportAndFinalize( Callbacks const& callbacks )
{
	if( mUnderlying == nullptr )
	{
		RFLOG_ERROR( nullptr, RFCAT_SERIALIZATION, "Uninitialized importer, cannot import" );
		RF_DBGFAIL();
		return false;
	}

	return mUnderlying->ImportAndFinalize( callbacks );
}

///////////////////////////////////////////////////////////////////////////////
}

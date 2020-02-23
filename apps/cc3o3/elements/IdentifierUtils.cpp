#include "stdafx.h"
#include "IdentifierUtils.h"

#include "core_math/math_clamps.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

ElementIdentifier MakeElementIdentifier( ElementString& identifier )
{
	ElementIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), sizeof( retVal ) );
	return retVal;
}



ElementIdentifier MakeElementIdentifier( rftl::string const& identifier )
{
	RF_ASSERT( identifier.size() <= sizeof( identifier ) );
	ElementIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), math::Min( identifier.size(), sizeof( retVal ) ) );
	return retVal;
}



ElementString GetElementString( ElementIdentifier identifier )
{
	ElementString retVal = {};
	memcpy( retVal.data(), &identifier, sizeof( identifier ) );
	return retVal;
}



ElementName GetElementName( ElementIdentifier identifier )
{
	ElementName retVal = {};
	retVal.at( 0 ) = '$';
	memcpy( retVal.data() + 1, &identifier, sizeof( identifier ) );
	retVal.at( 9 ) = 'N';
	return retVal;
}



ElementSynopsis GetElementSynopsis( ElementIdentifier identifier )
{
	ElementSynopsis retVal = {};
	retVal.at( 0 ) = '$';
	memcpy( retVal.data() + 1, &identifier, sizeof( identifier ) );
	retVal.at( 9 ) = 'S';
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

InnateIdentifier MakeInnateIdentifier( InnateString& identifier )
{
	InnateIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), sizeof( retVal ) );
	return retVal;
}



InnateIdentifier MakeInnateIdentifier( rftl::string const& identifier )
{
	RF_ASSERT( identifier.size() <= sizeof( identifier ) );
	InnateIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), math::Min( identifier.size(), sizeof( retVal ) ) );
	return retVal;
}



InnateString GetInnateString( InnateIdentifier identifier )
{
	InnateString retVal = {};
	memcpy( retVal.data(), &identifier, sizeof( identifier ) );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

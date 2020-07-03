#include "stdafx.h"
#include "IdentifierUtils.h"

#include "core_math/math_clamps.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

ElementIdentifier MakeElementIdentifier( ElementString const& identifier )
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
	retVal.push_back( '$' );
	memcpy( retVal.data() + 1, &identifier, sizeof( identifier ) );
	retVal.push_back( 'N' );
	return retVal;
}



ElementSynopsis GetElementSynopsis( ElementIdentifier identifier )
{
	ElementSynopsis retVal( ElementSynopsis::fixed_capacity, '_' );
	retVal.push_back( '$' );
	memcpy( retVal.data() + 1, &identifier, sizeof( identifier ) );
	retVal.push_back( 'S' );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

InnateIdentifier MakeInnateIdentifier( InnateString const& identifier )
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

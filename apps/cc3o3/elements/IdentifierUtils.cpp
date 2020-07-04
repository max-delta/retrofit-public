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

bool operator==( InnateIdentifier const& lhs, InnateString const& rhs )
{
	return lhs == MakeInnateIdentifier( rhs );
}



bool SortPredicateInnate( InnateIdentifier const& lhs, InnateIdentifier const& rhs )
{
	// Sort special known types before everything else
	static constexpr auto coerce = []( InnateIdentifier& innate ) -> void //
	{
		if( innate <= 6 )
			innate += 6;
		else if( innate == InnateString{ 'r', 'e', 'd' } )
			innate = 0;
		else if( innate == InnateString{ 'b', 'l', 'u' } )
			innate = 1;
		else if( innate == InnateString{ 'y', 'e', 'l' } )
			innate = 2;
		else if( innate == InnateString{ 'g', 'r', 'n' } )
			innate = 3;
		else if( innate == InnateString{ 'w', 'h', 't' } )
			innate = 4;
		else if( innate == InnateString{ 'b', 'l', 'k' } )
			innate = 5;
		else if( innate == InnateString{ 't', 'e', 'k' } )
			innate = 6;
	};

	InnateIdentifier l = lhs;
	InnateIdentifier r = rhs;
	coerce( l );
	coerce( r );
	return l < r;
}

///////////////////////////////////////////////////////////////////////////////
}

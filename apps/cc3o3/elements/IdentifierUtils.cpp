#include "stdafx.h"
#include "IdentifierUtils.h"

#include "core_math/math_clamps.h"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

ElementIdentifier MakeElementIdentifier( ElementBytes const& identifier )
{
	static_assert( sizeof( ElementBytes ) == sizeof( ElementIdentifier ) );
	ElementIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), sizeof( retVal ) );
	return retVal;
}



ElementIdentifier MakeElementIdentifier( ElementString const& identifier )
{
	static_assert( ElementString::fixed_capacity == sizeof( ElementIdentifier ) );
	return identifier.as_integer<ElementIdentifier>();
}



ElementIdentifier MakeElementIdentifier( rftl::string_view const& identifier )
{
	RF_ASSERT( identifier.size() <= ElementString::fixed_capacity );
	return ElementString( identifier ).as_integer<ElementIdentifier>();
}



ElementBytes GetElementBytes( ElementIdentifier identifier )
{
	static_assert( sizeof( ElementBytes ) == sizeof( ElementIdentifier ) );
	ElementBytes retVal = {};
	memcpy( retVal.data(), &identifier, sizeof( identifier ) );
	return retVal;
}



ElementString GetElementString( ElementIdentifier identifier )
{
	static_assert( ElementString::fixed_capacity == sizeof( ElementIdentifier ) );
	return ElementString( identifier );
}



ElementName GetElementName( ElementIdentifier identifier )
{
	ElementName retVal = {};
	for( char const& ch : kElementLocalizationPrefix )
	{
		retVal.push_back( ch );
	}
	memcpy( retVal.data() + sizeof( kElementLocalizationPrefix ), &identifier, sizeof( identifier ) );
	retVal.push_back( 'N' );
	return retVal;
}



ElementSynopsis GetElementSynopsis( ElementIdentifier identifier )
{
	ElementSynopsis retVal = {};
	for( char const& ch : kElementLocalizationPrefix )
	{
		retVal.push_back( ch );
	}
	memcpy( retVal.data() + sizeof( kElementLocalizationPrefix ), &identifier, sizeof( identifier ) );
	retVal.push_back( 'S' );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////

InnateIdentifier MakeInnateIdentifier( InnateBytes const& identifier )
{
	static_assert( sizeof( InnateBytes ) == sizeof( InnateIdentifier ) );
	InnateIdentifier retVal = {};
	memcpy( &retVal, identifier.data(), sizeof( retVal ) );
	return retVal;
}



InnateIdentifier MakeInnateIdentifier( InnateString const& identifier )
{
	static_assert( InnateString::fixed_capacity == sizeof( InnateIdentifier ) );
	return identifier.as_integer<InnateIdentifier>();
}



InnateIdentifier MakeInnateIdentifier( rftl::string_view const& identifier )
{
	RF_ASSERT( identifier.size() <= InnateString::fixed_capacity );
	return InnateString( identifier ).as_integer<InnateIdentifier>();
}



InnateBytes GetInnateBytes( InnateIdentifier identifier )
{
	static_assert( sizeof( InnateBytes ) == sizeof( InnateIdentifier ) );
	InnateBytes retVal = {};
	memcpy( retVal.data(), &identifier, sizeof( identifier ) );
	return retVal;
}



InnateString GetInnateString( InnateIdentifier identifier )
{
	static_assert( InnateString::fixed_capacity == sizeof( InnateIdentifier ) );
	return InnateString( identifier );
}

///////////////////////////////////////////////////////////////////////////////

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

bool operator==( RF::cc::element::InnateIdentifier const& lhs, RF::cc::element::InnateString const& rhs )
{
	return lhs == RF::cc::element::MakeInnateIdentifier( rhs );
}

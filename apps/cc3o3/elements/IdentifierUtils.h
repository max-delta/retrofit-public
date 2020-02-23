#pragma once
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/extension/static_string.h"
#include "rftl/string"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

// Element identifiers are intended to be valid ASCII strings
using ElementString = rftl::static_string<sizeof( ElementIdentifier )>;
using ElementName = rftl::static_string<sizeof( "$" ) + sizeof( ElementIdentifier ) + sizeof( 'N' )>;
using ElementSynopsis = rftl::static_string<sizeof( "$" ) + sizeof( ElementIdentifier ) + sizeof( 'S' )>;

// Innate identifiers are intended to be valid ASCII strings
using InnateString = rftl::static_string<sizeof( InnateIdentifier )>;

///////////////////////////////////////////////////////////////////////////////

ElementIdentifier MakeElementIdentifier( ElementString& identifier );
ElementIdentifier MakeElementIdentifier( rftl::string const& identifier );
ElementString GetElementString( ElementIdentifier identifier );
ElementName GetElementName( ElementIdentifier identifier );
ElementSynopsis GetElementSynopsis( ElementIdentifier identifier );

///////////////////////////////////////////////////////////////////////////////

InnateIdentifier MakeInnateIdentifier( InnateString& identifier );
InnateIdentifier MakeInnateIdentifier( rftl::string const& identifier );
InnateString GetInnateString( InnateIdentifier identifier );

///////////////////////////////////////////////////////////////////////////////
}

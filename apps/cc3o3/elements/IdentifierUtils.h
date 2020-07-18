#pragma once
#include "cc3o3/elements/ElementFwd.h"

#include "rftl/extension/static_string.h"
#include "rftl/string"
#include "rftl/array"


namespace RF::cc::element {
///////////////////////////////////////////////////////////////////////////////

// Element identifiers are intended to be valid ASCII strings
using ElementBytes = rftl::array<uint8_t, sizeof( ElementIdentifier )>;
using ElementString = rftl::static_string<sizeof( ElementIdentifier )>;
static constexpr char const kElementLocalizationPrefix[] = { '$', 'e', 'l', 'e', 'm', 'e', 'n', 't', '_' };
using ElementName = rftl::static_string<sizeof( kElementLocalizationPrefix ) + sizeof( ElementIdentifier ) + sizeof( 'N' )>;
using ElementSynopsis = rftl::static_string<sizeof( kElementLocalizationPrefix ) + sizeof( ElementIdentifier ) + sizeof( 'S' )>;

// Innate identifiers are intended to be valid ASCII strings
using InnateBytes = rftl::array<uint8_t, sizeof( InnateIdentifier )>;
using InnateString = rftl::static_string<sizeof( InnateIdentifier )>;

///////////////////////////////////////////////////////////////////////////////

ElementIdentifier MakeElementIdentifier( ElementBytes const& identifier );
ElementIdentifier MakeElementIdentifier( ElementString const& identifier );
ElementIdentifier MakeElementIdentifier( rftl::string const& identifier );
ElementBytes GetElementBytes( ElementIdentifier identifier );
ElementString GetElementString( ElementIdentifier identifier );
ElementName GetElementName( ElementIdentifier identifier );
ElementSynopsis GetElementSynopsis( ElementIdentifier identifier );

///////////////////////////////////////////////////////////////////////////////

InnateIdentifier MakeInnateIdentifier( InnateBytes const& identifier );
InnateIdentifier MakeInnateIdentifier( InnateString const& identifier );
InnateIdentifier MakeInnateIdentifier( rftl::string const& identifier );
InnateBytes GetInnateBytes( InnateIdentifier identifier );
InnateString GetInnateString( InnateIdentifier identifier );

///////////////////////////////////////////////////////////////////////////////

bool SortPredicateInnate( InnateIdentifier const& lhs, InnateIdentifier const& rhs );

///////////////////////////////////////////////////////////////////////////////
}

bool operator==( RF::cc::element::InnateIdentifier const& lhs, RF::cc::element::InnateString const& rhs );

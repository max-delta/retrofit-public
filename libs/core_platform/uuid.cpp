#include "stdafx.h"
#include "uuid.h"

#include "rftl/cstring"


namespace RF { namespace platform {
///////////////////////////////////////////////////////////////////////////////

Uuid::Uuid()
	: Uuid( kInvalid )
{
	//
}



Uuid::Uuid( OctetSequence const & sequence )
	: mOctetSequence{
		sequence[0], sequence[1], sequence[2], sequence[3],
		sequence[4], sequence[5], sequence[6], sequence[7],
		sequence[8], sequence[9], sequence[10], sequence[11],
		sequence[12], sequence[13], sequence[14], sequence[15] }
{
	//
}



bool Uuid::operator==( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult == 0;
}



bool Uuid::operator!=( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult != 0;
}



bool Uuid::operator<( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult < 0;
}



bool Uuid::operator>( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult > 0;
}



bool Uuid::operator<=( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult < 0 || compareResult == 0;
}



bool Uuid::operator>=( Uuid const & rhs ) const
{
	int const compareResult = std::memcmp( &this->mOctetSequence[0], &rhs.mOctetSequence[0], kNumOctets );
	return compareResult > 0 || compareResult == 0;
}



bool Uuid::IsValid() const
{
	return *this != kInvalid;
}



bool Uuid::HasClearlyDefinedCollisionAvoidance() const
{
	uint8_t const version = GetVersion();
	// Hey man, you either get reliablility or security, you can't have it both
	//  ways, sorry...
	// NOTE: Version 2 may be well-defined too, but I don't have anything lying
	//  around that emits version 2 to dig through the internals of, and I
	//  think it may be uncommon
	return version == 1;
}



bool Uuid::ExposesComputerInformation() const
{
	uint8_t const version = GetVersion();
	// 3 and 5 MAY expose computer information, but it's not knowable. Version
	//  5 is much less likely to expose information than version 3 though.
	constexpr bool kParanoid = true;
	return
		version == 1 ||
		version == 2 ||
		( kParanoid && version == 3 ) ||
		( kParanoid && version == 5 );
}



uint8_t Uuid::GetVersion() const
{
	// 0 = Invalid?
	// 1 = Time and MAC
	// 2 = DCE Security
	// 3 = MD5 hash
	// 4 = RNG and pray
	// 5 = SHA-1 hash
	Octet const& versionOctet = mOctetSequence[6];
	uint8_t const version = ( versionOctet & 0xf0u ) >> 4;
	return version;
}



uint8_t Uuid::GetVariant() const
{
	// 0 - 3 = NCS compat
	// 4 - 5 = Standard
	// 6 = MS compat
	// 7 = Reserved
	Octet const& variantOctet = mOctetSequence[6];
	uint8_t const variant = ( variantOctet & 0xe0u ) >> 5;
	return variant;
}



Uuid::OctetSequence const & Uuid::GetSequence() const
{
	return mOctetSequence;
}



rftl::string Uuid::GetDebugString() const
{
	char octetBuf[3] = { '0','0','\0' };
	constexpr char kExample[] = "{11223344-1122-1122-1122-112233445566}(v1!$,1)";
	rftl::string retVal;
	retVal.reserve( sizeof( kExample ) );

	retVal.push_back( '{' );
	for( size_t i = 0; i < kNumOctets; i++ )
	{
		Octet const& octet = mOctetSequence[i];
		int numParsed = snprintf( &octetBuf[0], 3, "%02x", octet );
		if( numParsed != 2 )
		{
			octetBuf[0] = '?';
			octetBuf[1] = '?';
		}
		retVal.push_back( octetBuf[0] );
		retVal.push_back( octetBuf[1] );
		if( i == 3 || i == 5 || i == 7 || i == 9 )
		{
			retVal.push_back( '-' );
		}
	}
	retVal.push_back( '}' );
	retVal.push_back( '(' );
	retVal.push_back( 'v' );
	{
		char const versionChar = '0' + static_cast<char>( GetVersion() );
		retVal.push_back( versionChar );
		if( ExposesComputerInformation() )
		{
			retVal.push_back( '!' );
		}
		if( HasClearlyDefinedCollisionAvoidance() )
		{
			retVal.push_back( '#' );
		}
	}
	retVal.push_back( ',' );
	{
		char const variantChar = '0' + static_cast<char>( GetVariant() );
		retVal.push_back( variantChar );
	}
	retVal.push_back( ')' );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}}

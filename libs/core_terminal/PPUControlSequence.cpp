#include "stdafx.h"
#include "PPUControlSequence.h"

#include "core/rf_assert.h"


namespace RF::term::ppu {
///////////////////////////////////////////////////////////////////////////////
namespace details {

rftl::string_view DiscardEscapeCharacter( rftl::string_view readHead )
{
	RF_ASSERT( readHead.empty() == false );
	RF_ASSERT( readHead.front() == kEscapeCharacter );
	readHead.remove_prefix( 1 );
	return readHead;
}



size_t CheckDirectiveBytes( rftl::string_view readHead )
{
	RF_ASSERT( readHead.empty() == false );
	char const firstDirectiveByte = readHead.front();
	RF_ASSERT( IsValidDirectiveByte( firstDirectiveByte ) );
	size_t const numBytes = NumBytesExpectedInDirective( firstDirectiveByte );
	RF_ASSERT( numBytes > 0 );
	if constexpr( RF_IS_ALLOWED( RF_CONFIG_ASSERTS ) )
	{
		for( size_t i = 0; i < numBytes; i++ )
		{
			RF_ASSERT( IsValidDirectiveByte( readHead.at( i ) ) );
		}
	}

	return numBytes;
}



void Process1ByteDirective( PPUParseState& state, rftl::string_view directive )
{
	RF_ASSERT( directive.size() == 1 );
	uint8_t const firstByte = static_cast<uint8_t>( directive.at( 0 ) );

	// 01xxxxxx
	//   ^^     Signifier (0/1/2/3)
	//     ^^^^ Pallete index (when signifier == 0)
	//     ^^^^ Reserved (when signifier == 1)
	//     ^^^^ Reserved (when signifier == 2)
	//     0000 Clear all state (when signifier == 3)
	//     1    Clear [reserved] (when signifier == 3)
	//      1   Clear [reserved sig 2] (when signifier == 3)
	//       1  Clear [reserved sig 1] (when signifier == 3)
	//        1 Clear pallete index (when signifier == 3)

	uint8_t const signifier = ( firstByte & 0b00110000u ) >> 4u;
	uint8_t const payload = firstByte & 0b00001111u;

	switch( signifier )
	{
		case 0:
			// Pallete index
			state.mPalleteIndex = payload;
			return;
		case 1:
			// Reserved
			return;
		case 2:
			// Reserved
			return;
		case 3:
			// Clear
			if( payload == 0b0000 )
			{
				// All (special case)
				state = {};
				return;
			}
			if( payload & 0b0001 )
			{
				// Pallete index
				state.mPalleteIndex.reset();
			}
			if( payload & 0b0010 )
			{
				// Reserved sig 1
			}
			if( payload & 0b0100 )
			{
				// Reserved sig 2
			}
			if( payload & 0b1000 )
			{
				// Reserved
			}
			return;
		default:
			RF_DBGFAIL();
			return;
	}
}



void Process2ByteDirective( PPUParseState& state, rftl::string_view directive )
{
	RF_ASSERT( directive.size() == 2 );
	uint8_t const firstByte = static_cast<uint8_t>( directive.at( 0 ) );
	uint8_t const secondByte = static_cast<uint8_t>( directive.at( 1 ) );

	// 001xxxxx 01xxxxxx
	//    ^^^^^   ^^^^^^ Reserved

	( (void)firstByte );
	( (void)secondByte );
}



void Process3ByteDirective( PPUParseState& state, rftl::string_view directive )
{
	RF_ASSERT( directive.size() == 3 );
	uint8_t const firstByte = static_cast<uint8_t>( directive.at( 0 ) );
	uint8_t const secondByte = static_cast<uint8_t>( directive.at( 1 ) );
	uint8_t const thirdByte = static_cast<uint8_t>( directive.at( 2 ) );

	// 0001xxxx 01xxxxxx 01xxxxxx
	//     ^^^^   ^^^^^^   ^^^^^^ Reserved

	( (void)firstByte );
	( (void)secondByte );
	( (void)thirdByte );
}



void Process4ByteDirective( PPUParseState& state, rftl::string_view directive )
{
	RF_ASSERT( directive.size() == 4 );
	uint8_t const firstByte = static_cast<uint8_t>( directive.at( 0 ) );
	uint8_t const secondByte = static_cast<uint8_t>( directive.at( 1 ) );
	uint8_t const thirdByte = static_cast<uint8_t>( directive.at( 2 ) );
	uint8_t const fourthByte = static_cast<uint8_t>( directive.at( 3 ) );

	// 00001xxx 01xxxxxx 01xxxxxx 01xxxxxx
	//      ^^^   ^^^^^^   ^^^^^^   ^^^^^^ Reserved

	( (void)firstByte );
	( (void)secondByte );
	( (void)thirdByte );
	( (void)fourthByte );
}

}
///////////////////////////////////////////////////////////////////////////////

bool IsValidDirectiveByte( char byte )
{
	bool const isNonNull = byte != '\0';
	bool const isNonUtf8 = ( byte & 0b1000'0000 ) == 0b0000'0000;
	return isNonNull && isNonUtf8;
}



size_t NumBytesExpectedInDirective( char firstByte )
{
	if( ( firstByte & 0b1100'0000 ) == 0b0100'0000 )
	{
		// 01xxxxxx
		return 1;
	}
	else if( ( firstByte & 0b1110'0000 ) == 0b0010'0000 )
	{
		// 001xxxxx 01xxxxxx
		return 2;
	}
	else if( ( firstByte & 0b1111'0000 ) == 0b0001'0000 )
	{
		// 0001xxxx 01xxxxxx 01xxxxxx
		return 3;
	}
	else if( ( firstByte & 0b1111'1000 ) == 0b0000'1000 )
	{
		// 00001xxx 01xxxxxx 01xxxxxx 01xxxxxx
		return 4;
	}
	else
	{
		// Invalid
		return 0;
	}
}



rftl::string_view ConsumeControlSequence( PPUParseState& state, rftl::string_view readHead )
{
	// Escape
	readHead = details::DiscardEscapeCharacter( readHead );

	// Directive
	size_t const numBytes = details::CheckDirectiveBytes( readHead );
	rftl::string_view const directive = readHead.substr( 0, numBytes );
	readHead.remove_prefix( numBytes );

	if( numBytes == 0 )
	{
		RF_DBGFAIL_MSG( "Invalid control sequence" );
	}
	else if( numBytes == 1 )
	{
		details::Process1ByteDirective( state, directive );
	}
	else if( numBytes == 2 )
	{
		details::Process2ByteDirective( state, directive );
	}
	else if( numBytes == 3 )
	{
		details::Process3ByteDirective( state, directive );
	}
	else if( numBytes == 4 )
	{
		details::Process4ByteDirective( state, directive );
	}
	return readHead;
}



rftl::string_view DiscardControlSequence( rftl::string_view readHead )
{
	// Escape
	readHead = details::DiscardEscapeCharacter( readHead );

	// Directive
	size_t const numBytes = details::CheckDirectiveBytes( readHead );
	readHead.remove_prefix( numBytes );

	return readHead;
}

///////////////////////////////////////////////////////////////////////////////
}

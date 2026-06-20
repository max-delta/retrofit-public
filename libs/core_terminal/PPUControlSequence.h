#pragma once

#include "rftl/array"
#include "rftl/cstdint"
#include "rftl/optional"
#include "rftl/string_view"


namespace RF::term::ppu {
///////////////////////////////////////////////////////////////////////////////

// PPU control directives are used to alter the behavior of the PPU while it is
//  in the middle of processing text for rendering
struct PPUParseState
{
	bool operator==( PPUParseState const& ) const = default;

	// If set, the color of the text switches to a pallete-based color lookup,
	//  and if not set, it reverts back to the color that text would normally
	//  have (presumably the color of the string's command)
	using PalleteIndex = uint8_t;
	rftl::optional<PalleteIndex> mPalleteIndex = rftl::nullopt;

	// Reserved for more state, currently only used for unit tests until more
	//  state is available
	rftl::optional<bool> mReserved = rftl::nullopt;
};

// The PPU control sequence is a single escape character, followed by a series
//  of bytes that form the directive
// NOTE: The escape character is 'Enquiry' (ENQ / Ctrl-E), chosen because it is
//  within the lowest 8 ASCII characters, doesn't have a reasonable modern
//  use-case, and its historical usages are vaguely like commands already
// NOTE: [Ctrl-E] or [Alt+005] are the canonical ways to enter this into a text
//  editor that supports ASCII control codes
inline constexpr char kEscapeCharacter = '\x05';

// Each byte in the directive requires the top bit be unset, in order to ensure
//  that no byte could ever be misinterpreted as multi-byte UTF-8, but also
//  must have atleast one bit set so that no byte could ever look like the
//  null-terminator
bool IsValidDirectiveByte( char byte );

// The control sequence is variable-byte encoded, based on UTF-8, but without
//  the hacks that come from needing to support legacy ASCII characters as a
//  subset of the Unicode standard, and also with better bit-packing
size_t NumBytesExpectedInDirective( char firstByte );

// Consume a full control sequence, modifying the current parse state
// NOTE: Expects the first character to be the escape character
// NOTE: Returns the advanced read head
rftl::string_view ConsumeControlSequence( PPUParseState& state, rftl::string_view readHead );

// Discard a full control sequence, ignoring its effects
// NOTE: Expects the first character to be the escape character
// NOTE: Returns the advanced read head
rftl::string_view DiscardControlSequence( rftl::string_view readHead );

///////////////////////////////////////////////////////////////////////////////
namespace sequence {

inline constexpr rftl::array<char, 2> ClearAllState()
{
	return {
		kEscapeCharacter,
		static_cast<char>( 0b0111'0000u ) };
}



inline constexpr rftl::array<char, 2> SetPallete( PPUParseState::PalleteIndex index )
{
	return {
		kEscapeCharacter,
		static_cast<char>( 0b0100'0000u | ( index & 0b1111u ) ) };
}

inline constexpr rftl::array<char, 2> ClearPallete()
{
	return {
		kEscapeCharacter,
		static_cast<char>( 0b0111'0001u ) };
}

}
///////////////////////////////////////////////////////////////////////////////
}

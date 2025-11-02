#pragma once
#include "project.h"

#include "rftl/cstdint"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

enum class RawEntryType : uint8_t
{
	Invalid = 0,

	// Syntactically valid but contains no meaningful content
	Whitespace,

	// FORMAT: define TYPE KEY = VALUE
	// EXAMPLE: define char n = "narrator"
	Alias,

	// FORMAT: layout VALUE
	// EXAMPLE: layout "bottom_portrait_box"
	Layout,

	// FORMAT: command COMMAND [EXPR] KEY:VALUE ...
	// EXAMPLE: command "debug" echo : "true"
	Command,

	// FORMAT: scene ID KEY : VALUE...
	// EXAMPLE: scene "meadow" pos : "bg"
	Scene,

	// FORMAT: LOC_ID [CHAR] [EXPR] TEXT
	// EXAMPLE: 10 n xpr:"na" "Dialogue test"
	Speech,

	// FORMAT: label LABEL
	// EXAMPLE: label "pos_jump_target"
	Label,

	// FORMAT: jump TARGET
	// EXAMPLE: jump "always_jump_target"
	Jump,

	// FORMAT: jumpif TARGET KEY:VALUE
	// EXAMPLE: jumpif "pos_jump_target" test:"true"
	JumpIf,

	// FORMAT: jumpunless TARGET KEY:VALUE
	// EXAMPLE: jumpunless "pos_jump_target" test:"false"
	JumpUnless,
};

// Aliases are strongly-typed, they are not generic text replacement
enum class AliasType : uint8_t
{
	Invalid = 0,
	Char,
};

// This is like BASIC-style line numbers, meaning that the user is expected to
//  provide them, they must be unique within the translation unit, and they
//  must be ascending
using LocIDLineNum = uint16_t;
static constexpr LocIDLineNum kInvalidLocIDLineNum = 0;

class RawDialogueEntry;

///////////////////////////////////////////////////////////////////////////////
}

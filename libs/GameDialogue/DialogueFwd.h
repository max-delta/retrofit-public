#pragma once
#include "project.h"

#include "rftl/cstdint"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

// Raw entries are pretty close to the plain text data
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
	// EXAMPLE: label "pos_cond_target"
	Label,

	// FORMAT: cond_if TARGET KEY:VALUE
	// EXAMPLE: cond_if "pos_cond_target" test:"true"
	CondIf,

	// FORMAT: cond_unless TARGET KEY:VALUE
	// EXAMPLE: cond_unless "pos_cond_target" test:"false"
	CondUnless,

	// FORMAT: cond_else TARGET
	// EXAMPLE: cond_else "else_cond_target"
	CondElse,
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

// Processed entries contain much less intermediate data
enum class EntryType : uint8_t
{
	Invalid = 0,

	// Execute a command
	Command,

	// Switch the scene
	Scene,

	// Make a character speak
	Speech,
};

class RawDialogueEntry;
class DialogueLoader;
class DialogueSequence;

///////////////////////////////////////////////////////////////////////////////
}

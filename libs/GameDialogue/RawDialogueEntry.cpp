#include "stdafx.h"
#include "RawDialogueEntry.h"

#include "rftl/extension/string_parse.h"
#include "rftl/optional"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr char kInlineWhitespaceCharset[] = { ' ', '\t' };

using Token = rftl::string_view;
using Line = rftl::string_view;
using KVPair = rftl::pair<Token, Token>;
using KVMap = rftl::unordered_map<Token, Token>;



static rftl::string_view TrimFrontWhitespace( rftl::string_view str )
{
	return rftl::trim_prefix_chars( str, kInlineWhitespaceCharset );
}

static rftl::string_view TrimBackWhitespace( rftl::string_view str )
{
	return rftl::trim_suffix_chars( str, kInlineWhitespaceCharset );
}



static Token ReadToken( Line& line )
{
	line = TrimFrontWhitespace( line );
	Token const token = rftl::strtok_view( line, kInlineWhitespaceCharset );
	line = TrimFrontWhitespace( line );
	return token;
}



static Token PeekToken( Line line )
{
	return ReadToken( line );
}



static Token ReadQuotedString( Line& line )
{
	line = TrimFrontWhitespace( line );

	Line temp = line;
	if( temp.starts_with( '"' ) == false )
	{
		return {};
	}
	temp.remove_prefix( 1 );
	if( temp.find( '"' ) == temp.npos )
	{
		return {};
	}

	Token const everythingBeforeSecondQuotationMarks = rftl::strtok_view( temp, '"' );
	line = temp;
	return everythingBeforeSecondQuotationMarks;
}



static KVPair ReadKVPair( Line& line )
{
	line = TrimFrontWhitespace( line );

	Line temp = line;
	if( temp.empty() )
	{
		return {};
	}

	// Key
	Token key = rftl::strtok_view( temp, ':' );
	key = TrimFrontWhitespace( key );
	key = TrimBackWhitespace( key );
	if( key.empty() )
	{
		// No key
		return {};
	}
	if( key.find_first_of( kInlineWhitespaceCharset ) != key.npos )
	{
		// Whitespace inside the key
		return {};
	}

	// Value
	temp = TrimFrontWhitespace( temp );
	if( temp.starts_with( '"' ) == false )
	{
		// No quotation mark
		return {};
	}
	temp.remove_prefix( 1 );
	if( temp.find( '"' ) == temp.npos )
	{
		// No matching quotation mark
		return {};
	}
	Token value = rftl::strtok_view( temp, '"' );

	line = temp;
	return KVPair( key, value );
}



static KVMap ReadKVPairsUntilEnd( Line& line )
{
	KVMap retVal = {};

	Line temp = line;

	while( true )
	{
		temp = TrimFrontWhitespace( temp );
		if( temp.empty() )
		{
			break;
		}

		KVPair const param = ReadKVPair( temp );
		if( param.first.empty() )
		{
			return {};
		}

		retVal[param.first] = param.second;
	}

	line = temp;
	RF_ASSERT( line.empty() );
	return retVal;
}



static bool HasUnparsedCruft( Line line )
{
	return TrimFrontWhitespace( line ).empty() == false;
}



static void InterpretAsAlias( RawDialogueEntry& entry, Line line )
{
	// Type
	Token const type = ReadToken( line );
	if( type == "char" )
	{
		entry.mAliasType = AliasType::Char;
	}
	else
	{
		return;
	}

	// Shorthand
	entry.mPrimary = ReadToken( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// =
	Token const equals = ReadToken( line );
	if( equals != "=" )
	{
		return;
	}

	// Replacement value
	entry.mSecondary = ReadQuotedString( line );
	if( entry.mSecondary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Alias;
	return;
}



static void InterpretAsLayout( RawDialogueEntry& entry, Line line )
{
	// LayoutID
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Layout;
	return;
}



static void InterpretAsCommand( RawDialogueEntry& entry, Line line )
{
	// CommandID
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// Params
	entry.mKVPairs = ReadKVPairsUntilEnd( line );
	if( line.empty() == false )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Command;
	return;
}



static void InterpretAsScene( RawDialogueEntry& entry, Line line )
{
	// SceneID
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// Params
	entry.mKVPairs = ReadKVPairsUntilEnd( line );
	if( line.empty() == false )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Scene;
	return;
}



static void InterpretAsSpeech( RawDialogueEntry& entry, Token firstToken, Line line )
{
	// LocID
	bool const parsedInt = rftl::parse_int( entry.mLocIDLineNumber, firstToken );
	if( parsedInt == false )
	{
		return;
	}

	line = TrimFrontWhitespace( line );
	if( line.empty() )
	{
		// Invalid, missing dialogue text
		return;
	}

	// CharacterID? ExpressionID? dialogue text?
	Token tentativeToken = PeekToken( line );
	bool hasCharacter = true;
	bool hasExpression = true;
	if( tentativeToken.starts_with( '"' ) )
	{
		// Skips to dialogue text
		hasCharacter = false;
		hasExpression = false;
	}
	else if( tentativeToken.find( ':' ) != tentativeToken.npos )
	{
		// Skips to expression
		hasCharacter = false;
	}

	// CharacterID
	if( hasCharacter )
	{
		entry.mPrimary = ReadToken( line );
		if( entry.mPrimary.empty() )
		{
			RF_DBGFAIL_MSG( "Expected this to already be ensured by earlier parse" );
			return;
		}

		// Re-peek
		tentativeToken = PeekToken( line );
		if( tentativeToken.starts_with( '"' ) )
		{
			// Skips to dialogue text
			hasExpression = false;
		}
	}

	// ExpressionID
	if( hasExpression )
	{
		KVPair const expression = ReadKVPair( line );
		if( expression.first != "xpr" )
		{
			return;
		}
		entry.mSecondary = expression.second;
	}

	// Done peeking
	tentativeToken = {};

	// Dialogue Text
	entry.mTertiary = ReadQuotedString( line );
	if( entry.mTertiary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Speech;
	return;
}



static void InterpretAsLabel( RawDialogueEntry& entry, Line line )
{
	// Label
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::Label;
	return;
}



static void InterpretAsCondIf( RawDialogueEntry& entry, Line line )
{
	// Target
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	KVPair const condition = ReadKVPair( line );

	// Condition key
	entry.mSecondary = condition.first;
	if( entry.mSecondary.empty() )
	{
		return;
	}

	// Condition value
	entry.mTertiary = condition.second;
	if( entry.mTertiary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::CondIf;
	return;
}



static void InterpretAsCondUnless( RawDialogueEntry& entry, Line line )
{
	// Identical to condif, just negated
	InterpretAsCondIf( entry, line );
	if( entry.mEntryType == RawEntryType::CondIf )
	{
		entry.mEntryType = RawEntryType::CondUnless;
	}
}



static void InterpretAsCondElse( RawDialogueEntry& entry, Line line )
{
	// Target
	entry.mPrimary = ReadQuotedString( line );
	if( entry.mPrimary.empty() )
	{
		return;
	}

	// Cruft?
	if( HasUnparsedCruft( line ) )
	{
		return;
	}

	// Success!
	entry.mEntryType = RawEntryType::CondElse;
	return;
}

}
///////////////////////////////////////////////////////////////////////////////

RawDialogueEntry::RawDialogueEntry() = default;



RawDialogueEntry RawDialogueEntry::FromLine( size_t lineNumber, rftl::string_view str )
{
	RawDialogueEntry retVal = {};
	retVal.mTrueLineNumber = lineNumber;

	// Remove trivial whitespace from front
	str = details::TrimFrontWhitespace( str );

	// Remove any comment
	// NOTE: A side-effect of this naive logic is that '#' is not valid to use
	//  in any non-comment context, so it will need to be specified using some
	//  kind of escape sequence
	str = rftl::trim_after_find_first_inclusive( str, '#' );

	// Remove trivial whitespace from back
	str = details::TrimBackWhitespace( str );

	// Trivial
	if( str.empty() )
	{
		retVal.mEntryType = RawEntryType::Whitespace;
		return retVal;
	}

	// Extract the first token
	details::Token const firstToken = details::ReadToken( str );
	RF_ASSERT( firstToken.empty() == false );

	// Speech
	{
		char const chFirstCh = firstToken.front();
		if( chFirstCh >= '0' && chFirstCh <= '9' )
		{
			details::InterpretAsSpeech( retVal, firstToken, str );
			return retVal;
		}
	}

#define RF_FIRST_TOKEN_BRANCH( TEXT, FUNC ) \
	if( firstToken == "" TEXT "" ) \
	{ \
		FUNC( retVal, str ); \
		return retVal; \
	}

	RF_FIRST_TOKEN_BRANCH( "define", details::InterpretAsAlias );
	RF_FIRST_TOKEN_BRANCH( "layout", details::InterpretAsLayout );
	RF_FIRST_TOKEN_BRANCH( "command", details::InterpretAsCommand );
	RF_FIRST_TOKEN_BRANCH( "scene", details::InterpretAsScene );
	RF_FIRST_TOKEN_BRANCH( "label", details::InterpretAsLabel );
	RF_FIRST_TOKEN_BRANCH( "cond_if", details::InterpretAsCondIf );
	RF_FIRST_TOKEN_BRANCH( "cond_unless", details::InterpretAsCondUnless );
	RF_FIRST_TOKEN_BRANCH( "cond_else", details::InterpretAsCondElse );

#undef RF_FIRST_TOKEN_BRANCH

	// Not valid
	RF_ASSERT( retVal.mEntryType == RawEntryType::Invalid );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}

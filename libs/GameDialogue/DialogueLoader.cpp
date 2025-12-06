#include "stdafx.h"
#include "DialogueLoader.h"

#include "GameDialogue/DialogueSequence.h"
#include "GameDialogue/RawDialogueEntry.h"

#include "Logging/Logging.h"

#include "core_math/math_casts.h"

#include "rftl/extension/string_parse.h"
#include "rftl/unordered_set"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// Helpers for common validation
#define RF_MUST_BE_EMPTY( VAR ) \
	if( VAR.empty() == false ) \
	{ \
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Expected '" #VAR "' to be empty, but it is '{}'", VAR ); \
		RF_DBGFAIL(); \
		return false; \
	}
#define RF_MUST_BE_VALID( VAR ) \
	if( VAR.empty() ) \
	{ \
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Expected '" #VAR "' to be valid, but it is empty" ); \
		RF_DBGFAIL(); \
		return false; \
	}
#define RF_MUST_BE_UNIQUE( CONTAINER, KEY ) \
	if( CONTAINER.contains( KEY ) ) \
	{ \
		auto const& MACRO_existing = CONTAINER.at( KEY ); \
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Expected '" #CONTAINER "' to be unique, but encountered duplicate '{}' with value '{}'", KEY, MACRO_existing ); \
		RF_DBGFAIL(); \
		return false; \
	}
#define RF_NO_DUPE_SCOPE( CONTAINER, NAME ) \
	for( Scope const& scope : CONTAINER ) \
	{ \
		if( scope.mName == NAME ) \
		{ \
			RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Already have an open scope for '{}', cannot nest another with the same name", NAME ); \
			RF_DBGFAIL(); \
			return false; \
		} \
	}



using AliasTable = rftl::unordered_map<rftl::string_view, rftl::string_view>;
using ExpressionTable = rftl::unordered_map<rftl::string_view, rftl::string_view>;

using LineNumber = size_t;
using LabelLocations = rftl::unordered_map<rftl::string_view, LineNumber>;

struct Scope
{
	rftl::string_view mName;
	rftl::string_view mCondKey;
	rftl::string_view mCondVal;
	bool mIsNegated = false;
};
using ScopeStack = rftl::deque<Scope>;

struct State
{
	RF_NO_COPY( State );
	State() = default;

	// Text-replacement aliases to apply
	AliasTable mCharAliases;

	// Some speech entries re-use previous state
	rftl::string_view mLastChar;
	ExpressionTable mLastExpressionPerChar;

	// If present, we're slapping conditions onto applicable entries
	ScopeStack mScopeStack;

	// Can't have labels re-used
	// NOTE: May revisit this, but probably not, it's useful for debuggability
	LabelLocations mAlreadyUsedLabels;

	// The return value we're building up
	DialogueSequence mSequence;
};



[[nodiscard]] static rftl::string_view ApplyAlias( AliasTable const& aliases, rftl::string_view token )
{
	RF_ASSERT( token.empty() == false );

	AliasTable::const_iterator const iter = aliases.find( token );
	if( iter != aliases.end() )
	{
		// Has an alias
		rftl::string_view const& aliased = iter->second;
		RF_ASSERT( aliased.empty() == false );
		return aliased;
	}

	// No alias, leave unmodified
	return token;
}



static rftl::string_view MakeStorage( DialogueSequence::Strings& cache, rftl::string_view fromBuffer )
{
	// Check for exising
	for( rftl::string const& str : cache )
	{
		if( str == fromBuffer )
		{
			// Already cached
			return str;
		}
	}

	// New, must cache
	return cache.emplace_back( fromBuffer );
}



static void PushConditionsOntoEntry(
	State const& state,
	DialogueSequence& sequence,
	DialogueSequence::Entry& entry )
{
	// For each scope...
	for( Scope const& scope : state.mScopeStack )
	{
		rftl::string_view const key = MakeStorage( sequence.mRequiredConditionKeys, scope.mCondKey );
		rftl::string_view const val = MakeStorage( sequence.mRequiredConditionVals, scope.mCondVal );

		if( scope.mIsNegated )
		{
			// Negative condition
			entry.mForbiddenConditions.emplace( key, val );
		}
		else
		{
			// Positive condition
			entry.mNecessaryConditions.emplace( key, val );
		}
	}
}



static bool IngestAlias( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Alias );
	rftl::string_view const& shorthand = rawEntry.mPrimary;
	rftl::string_view const& replacement = rawEntry.mSecondary;
	RF_MUST_BE_VALID( shorthand );
	RF_MUST_BE_VALID( replacement );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	switch( rawEntry.mAliasType )
	{
		case AliasType::Char:
			RF_MUST_BE_UNIQUE( state.mCharAliases, shorthand );
			state.mCharAliases[shorthand] = replacement;
			return true;

		case AliasType::Invalid:
		default:
			RF_DBGFAIL();
			return false;
	}
}


static bool IngestLayout( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Layout );
	rftl::string_view const& layout = rawEntry.mPrimary;
	RF_MUST_BE_VALID( layout );
	RF_MUST_BE_EMPTY( rawEntry.mSecondary );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	// Only one
	rftl::string& storage = state.mSequence.mLayout;
	if( storage.empty() == false )
	{
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Layout already was set to '{}', cannot have multiple", storage );
		RF_DBGFAIL();
		return false;
	}

	// Store
	storage = layout;

	return true;
}



static bool IngestCommand( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Command );
	rftl::string_view const& command = rawEntry.mPrimary;
	RF_MUST_BE_VALID( command );
	RF_MUST_BE_EMPTY( rawEntry.mSecondary );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	// Can't give commands without a layout
	RF_MUST_BE_VALID( state.mSequence.mLayout );

	// Store
	DialogueSequence::Entry& newEntry = state.mSequence.mEntries.emplace_back();
	newEntry.mEntryType = EntryType::Command;
	newEntry.mPrimary = MakeStorage( state.mSequence.mRequiredCommands, command );
	for( RawDialogueEntry::KVPairs::value_type const& pair : rawEntry.mKVPairs )
	{
		RF_MUST_BE_VALID( pair.first );
		RF_MUST_BE_VALID( pair.second );
		newEntry.mParams[MakeStorage( state.mSequence.mRequiredCommandParamKeys, pair.first )] =
			MakeStorage( state.mSequence.mRequiredCommandParamVals, pair.second );
	}
	PushConditionsOntoEntry( state, state.mSequence, newEntry );

	return true;
}



static bool IngestScene( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Scene );
	rftl::string_view const& scene = rawEntry.mPrimary;
	RF_MUST_BE_VALID( scene );
	RF_MUST_BE_EMPTY( rawEntry.mSecondary );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	// Can't use scenes without a layout
	RF_MUST_BE_VALID( state.mSequence.mLayout );

	// Store
	DialogueSequence::Entry& newEntry = state.mSequence.mEntries.emplace_back();
	newEntry.mEntryType = EntryType::Scene;
	newEntry.mPrimary = MakeStorage( state.mSequence.mRequiredScenes, scene );
	for( RawDialogueEntry::KVPairs::value_type const& pair : rawEntry.mKVPairs )
	{
		RF_MUST_BE_VALID( pair.first );
		RF_MUST_BE_VALID( pair.second );
		newEntry.mParams[MakeStorage( state.mSequence.mRequiredSceneParamKeys, pair.first )] =
			MakeStorage( state.mSequence.mRequiredSceneParamVals, pair.second );
	}
	PushConditionsOntoEntry( state, state.mSequence, newEntry );

	return true;
}



static bool IngestSpeech( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Speech );
	rftl::string_view character = rawEntry.mPrimary;
	rftl::string_view expression = rawEntry.mSecondary;
	rftl::string_view const& text = rawEntry.mTertiary;
	RF_MUST_BE_VALID( text );

	// Character
	rftl::string_view& lastCharacter = state.mLastChar;
	if( character.empty() )
	{
		// Re-use last character
		character = lastCharacter;
	}
	RF_MUST_BE_VALID( character );
	character = ApplyAlias( state.mCharAliases, character );
	lastCharacter = character;

	// Expression
	rftl::string_view& lastExpression = state.mLastExpressionPerChar[character];
	if( expression.empty() )
	{
		// Re-use character's last expression
		expression = lastExpression;
	}
	RF_MUST_BE_VALID( expression );
	lastExpression = expression;

	// Store
	DialogueSequence::Entry& newEntry = state.mSequence.mEntries.emplace_back();
	newEntry.mEntryType = EntryType::Speech;
	newEntry.mLocIDLineNumber = rawEntry.mLocIDLineNumber;
	newEntry.mPrimary = MakeStorage( state.mSequence.mRequiredCharacters, character );
	newEntry.mExpression = MakeStorage( state.mSequence.mRequiredExpressions, expression );
	newEntry.mFallbackText = MakeStorage( state.mSequence.mSpeechStorage, text );
	PushConditionsOntoEntry( state, state.mSequence, newEntry );

	return true;
}



static bool IngestLabel( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::Label );
	rftl::string_view const& label = rawEntry.mPrimary;
	RF_MUST_BE_VALID( label );
	RF_MUST_BE_EMPTY( rawEntry.mSecondary );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	// No dupe labels
	if( state.mAlreadyUsedLabels.contains( label ) )
	{
		LineNumber const& existing = state.mAlreadyUsedLabels.at( label );
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Already encountered a label for '{}' at line #{}", label, existing );
		RF_DBGFAIL();
		return false;
	}
	state.mAlreadyUsedLabels.emplace( label, rawEntry.mTrueLineNumber );

	// Dubious, might be a mismatch, dangling scope, or just commented out
	if( state.mScopeStack.empty() )
	{
		RFLOGF_WARNING( nullptr, RFCAT_GAMEDIALOGUE, "Label for '{}' is not closing a scope, is it commented out?", label );
		return true;
	}
	Scope const& latest = state.mScopeStack.back();
	if( latest.mName != label )
	{
		RFLOGF_WARNING( nullptr, RFCAT_GAMEDIALOGUE, "Label for '{}' is not closing the current scope '{}', is it commented out?", label, latest.mName );
		return true;
	}

	// Pop
	state.mScopeStack.pop_back();

	return true;
}



static bool IngestCondIf( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::CondIf );
	rftl::string_view const& target = rawEntry.mPrimary;
	rftl::string_view const& key = rawEntry.mSecondary;
	rftl::string_view const& value = rawEntry.mTertiary;
	RF_MUST_BE_VALID( target );
	RF_MUST_BE_VALID( key );
	RF_MUST_BE_VALID( value );

	// No nesting with dupes
	RF_NO_DUPE_SCOPE( state.mScopeStack, target );

	// Temp stack
	state.mScopeStack.emplace_back(
		Scope{
			.mName = target,
			.mCondKey = key,
			.mCondVal = value,
			.mIsNegated = false } );

	return true;
}



static bool IngestCondUnless( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::CondUnless );
	rftl::string_view const& target = rawEntry.mPrimary;
	rftl::string_view const& key = rawEntry.mSecondary;
	rftl::string_view const& value = rawEntry.mTertiary;
	RF_MUST_BE_VALID( target );
	RF_MUST_BE_VALID( key );
	RF_MUST_BE_VALID( value );

	// No nesting with dupes
	RF_NO_DUPE_SCOPE( state.mScopeStack, target );

	// Temp stack
	state.mScopeStack.emplace_back(
		Scope{
			.mName = target,
			.mCondKey = key,
			.mCondVal = value,
			.mIsNegated = true } );

	return true;
}



static bool IngestCondElse( State& state, RawDialogueEntry const& rawEntry )
{
	RF_ASSERT( rawEntry.mEntryType == RawEntryType::CondElse );
	rftl::string_view const& target = rawEntry.mPrimary;
	RF_MUST_BE_VALID( target );
	RF_MUST_BE_EMPTY( rawEntry.mSecondary );
	RF_MUST_BE_EMPTY( rawEntry.mTertiary );

	// Any scopes?
	if( state.mScopeStack.empty() )
	{
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "No open scopes, cannot do an else for '{}'", target );
		RF_DBGFAIL();
		return false;
	}
	Scope& currentScope = state.mScopeStack.back();

	// Expect to operate on current
	if( currentScope.mName != target )
	{
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Current scope '{}' doesn't match else's '{}'", currentScope.mName, target );
		RF_DBGFAIL();
		return false;
	}

	// Invert
	currentScope.mIsNegated = !currentScope.mIsNegated;

	return true;
}



static bool Ingest( State& state, RawDialogueEntry const& rawEntry )
{
#define RF_ENTRY_TYPE( TYPE ) \
	case RawEntryType::TYPE: \
		return Ingest##TYPE( state, rawEntry );

	switch( rawEntry.mEntryType )
	{
		RF_ENTRY_TYPE( Alias );
		RF_ENTRY_TYPE( Label );
		RF_ENTRY_TYPE( Layout );
		RF_ENTRY_TYPE( Command );
		RF_ENTRY_TYPE( Scene );
		RF_ENTRY_TYPE( Speech );
		RF_ENTRY_TYPE( CondIf );
		RF_ENTRY_TYPE( CondUnless );
		RF_ENTRY_TYPE( CondElse );

		case RawEntryType::Invalid:
		case RawEntryType::Whitespace:
		default:
			RF_DBGFAIL();
			return false;
	}

#undef RF_ENTRY_TYPE
}

#undef RF_MUST_BE_EMPTY
#undef RF_MUST_BE_VALID
#undef RF_MUST_BE_UNIQUE
#undef RF_NO_DUPE_SCOPE

}
///////////////////////////////////////////////////////////////////////////////

DialogueLoader::RawEntries DialogueLoader::FirstPassParse( rftl::string_view rawText )
{
	RawEntries retVal;

	size_t lineNumber = 0;
	rftl::string_view parser = rawText;
	while( parser.empty() == false )
	{
		// NOTE: One-indexed
		lineNumber++;
		RF_ASSERT_MSG( lineNumber > 0, "Rollover" );

		// Extract next line
		rftl::string_view const line = rftl::strtok_view( parser, '\n' );

		// Parse
		dialogue::RawDialogueEntry entry = dialogue::RawDialogueEntry::FromLine( lineNumber, line );
		RFLOGF_TRACE( nullptr, RFCAT_GAMEDIALOGUE, "Raw-parsed dialogue line #{}: {}", lineNumber, line );
		RF_ASSERT( entry.mTrueLineNumber == lineNumber );

		if( entry.mEntryType == dialogue::RawEntryType::Invalid )
		{
			// Error
			RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Failed to parse dialogue line #{}: {}", lineNumber, line );
			RF_DBGFAIL();
			return {};
		}

		if( entry.mEntryType == dialogue::RawEntryType::Whitespace )
		{
			// Comment, or otherwise empty
			continue;
		}

		// Store
		retVal.emplace_back( rftl::move( entry ) );
	}

	return retVal;
}



DialogueSequence DialogueLoader::SecondPassParse( RawEntries const& rawEntries )
{
	details::State state = {};

	// For each entry...
	for( RawDialogueEntry const& rawEntry : rawEntries )
	{
		RF_ASSERT( rawEntry.mEntryType != RawEntryType::Invalid );
		RF_ASSERT( rawEntry.mEntryType != RawEntryType::Whitespace );

		size_t const& lineNumber = rawEntry.mTrueLineNumber;
		RF_ASSERT( lineNumber > 0 );

		// Ingest next entry
		bool const success = details::Ingest( state, rawEntry );
		RFLOGF_TRACE( nullptr, RFCAT_GAMEDIALOGUE, "Ingested dialogue line #{}", lineNumber );

		if( success == false )
		{
			// Error
			RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "Failed to ingest dialogue line #{}", lineNumber );
			RF_DBGFAIL();
			return {};
		}
	}

	// No unclosed scopes
	if( state.mScopeStack.empty() == false )
	{
		details::Scope const& latest = state.mScopeStack.back();
		RFLOGF_ERROR( nullptr, RFCAT_GAMEDIALOGUE, "One or more scopes are unclosed, including '{}', did you mess up your labels?", latest.mName );
		RF_DBGFAIL();
		return {};
	}

	// Success!
	return rftl::move( state.mSequence );
}



DialogueSequence DialogueLoader::Parse( rftl::string_view rawText )
{
	return SecondPassParse( FirstPassParse( rawText ) );
}

///////////////////////////////////////////////////////////////////////////////
}

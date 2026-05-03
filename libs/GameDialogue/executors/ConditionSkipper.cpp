#include "stdafx.h"
#include "ConditionSkipper.h"

#include "GameDialogue/DialogueSequence.h"

#include "Logging/Logging.h"


namespace RF::dialogue {
///////////////////////////////////////////////////////////////////////////////

void ConditionSkipper::SkipUnmetConditions( size_t& currentIndex, DialogueSequence const& sequence, const KVLookupFunc& kvLookup )
{
	size_t const numEntries = sequence.mEntries.size();
	if( numEntries <= currentIndex )
	{
		// At or beyond end of sequence
		return;
	}

	while( currentIndex < numEntries )
	{
		DialogueEntry const& entry = sequence.mEntries.at( currentIndex );

		using Key = rftl::string_view;
		using Value = rftl::string_view;

		// Forbidden check
		bool forbidden = false;
		for( DialogueEntry::KVPair const& condition : entry.mForbiddenConditions )
		{
			Key const& key = condition.first;
			Value const& checkVal = condition.second;
			rftl::optional<Value> const foundVal = kvLookup( key );
			if( foundVal.has_value() == false )
			{
				// Not found, this is okay as the forbidden is not present
				continue;
			}

			if( foundVal != checkVal )
			{
				// Not matched, this is okay as the forbidden is not present
				continue;
			}

			// A forbidden condition was matched
			forbidden = true;
			RFLOG_TRACE( nullptr, RFCAT_GAMEDIALOGUE,
				"Skip @ {}, forbidden condition: '{}'->'{}'",
				currentIndex, key, checkVal );
			break;
		}
		if( forbidden )
		{
			// Unmet, skip
			currentIndex++;
			continue;
		}

		// Necessary check
		bool unmetNecessary = false;
		for( DialogueEntry::KVPair const& condition : entry.mNecessaryConditions )
		{
			Key const& key = condition.first;
			Value const& checkVal = condition.second;
			rftl::optional<Value> const foundVal = kvLookup( key );
			if( foundVal.has_value() == false )
			{
				// Not found, this is bad as the necessary is not present
				RFLOG_TRACE( nullptr, RFCAT_GAMEDIALOGUE,
					"Skip @ {}, missing necessary condition: '{}'",
					currentIndex, key );
				unmetNecessary = true;
				break;
			}

			if( foundVal != checkVal )
			{
				// Not matched, this is bad as the necessary is not present
				RFLOG_TRACE( nullptr, RFCAT_GAMEDIALOGUE,
					"Skip @ {}, mismatched necessary condition: '{}'->'{}' encountered '{}'",
					currentIndex, key, checkVal, *foundVal );
				unmetNecessary = true;
				break;
			}

			// A necessary condition was matched
			continue;
		}
		if( unmetNecessary )
		{
			// Unmet, skip
			currentIndex++;
			continue;
		}

		// Met conditions, stop here
		RF_ASSERT( currentIndex < numEntries );
		return;
	}

	RF_ASSERT( currentIndex == numEntries );
}

///////////////////////////////////////////////////////////////////////////////
}

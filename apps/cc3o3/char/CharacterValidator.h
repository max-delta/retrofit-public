#pragma once
#include "project.h"

#include "cc3o3/char/CharFwd.h"
#include "cc3o3/char/Stats.h"
#include "cc3o3/company/CompanyFwd.h"

#include "GameSprite/SpriteFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"
#include "rftl/deque"
#include "rftl/array"
#include "rftl/string"


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

struct GeneticsEntry
{
	rftl::string mSpecies = {};
	rftl::string mGender = {};
	bool mPlayable = false;
	rftl::string m9Char = {};
};

///////////////////////////////////////////////////////////////////////////////

class CharacterValidator
{
	RF_NO_COPY( CharacterValidator );

	//
	// Types and constants
public:
	using GeneticsID = rftl::string;
	using SpeciesID = rftl::string;
	static constexpr int8_t kMaxTotalStatPoints = 16;
	static constexpr int8_t kMaxPointsPerStat = 5;

private:
	using GeneticsStorage = rftl::unordered_map<GeneticsID, GeneticsEntry>;
	using StatBonusesStorage = rftl::unordered_map<SpeciesID, Stats>;


	//
	// Public methods
public:
	CharacterValidator( WeakPtr<file::VFS const> vfs, WeakPtr<sprite::CharacterCreator const> characterCreator );

	bool LoadGeneticsTable( file::VFSPath const& geneticsTablePath );
	rftl::unordered_set<GeneticsID> GetGeneticsIDs() const;
	GeneticsEntry const& GetGeneticEntry( GeneticsID const& id ) const;
	GeneticsID GetGeneticsID( rftl::string const& species, rftl::string const& gender ) const;

	bool LoadStatBonusesTable( file::VFSPath const& statBonusesTablePath );
	Stats const& GetStatBonuses( SpeciesID const& id ) const;
	static int8_t CalculateTotalPoints( Stats const& stats );

	size_t GetMinimumTotalSlots( company::StoryTier storyTier ) const;
	SlotsPerElemLevel GetMinimumSlotDistribution( company::StoryTier storyTier ) const;
	size_t CalculateTotalSlots( Stats::StatModifier elemPower, company::StoryTier storyTier ) const;
	SlotsPerElemLevel CalculateSlotDistribution( Stats::StatModifier elemPower, Stats::GridShape gridShape, company::StoryTier storyTier ) const;

	void SanitizeForCharacterCreation( Character& character ) const;
	void SanitizeForGameplay( Character& character ) const;


	//
	// Private methods
private:
	rftl::deque<rftl::deque<rftl::string>> LoadCSV( file::VFSPath const& path );


	//
	// Private data
private:
	WeakPtr<file::VFS const> mVfs;

	// TODO: Only using this for composition logic, it should be seperated and
	//  then re-used by the various systems that need it
	WeakPtr<sprite::CharacterCreator const> mCharacterCreator;

	// TODO: This should probably be seperated and then re-used by the various
	//  systems that need it
	GeneticsStorage mGeneticsStorage;

	// TODO: This should probably be seperated and then re-used by the various
	//  systems that need it
	StatBonusesStorage mStatBonusesStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}}

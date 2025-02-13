#pragma once
#include "GameInput/GameController.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/vector"


namespace RF::input {
///////////////////////////////////////////////////////////////////////////////

// A game controller implementation for combining multiple game controllers
//  together, such as when parallel processing passes are done on the same
//  input stream, or multiple input streams need to be combined
class GAMEINPUT_API MergeController final : public GameController
{
	RF_NO_COPY( MergeController );

	//
	// Types and constants
public:
	using Source = WeakPtr<GameController>;
	using Sources = rftl::vector<WeakPtr<GameController>>;


	//
	// Public methods
public:
	MergeController();

	// NOTE: Order matters when multiple inputs have the same timestamp, as the
	//  earlier sources in the list will push their inputs to the combined
	//  stream before the later sources
	// NOTE: In general, this is assumed to not be a significant concern for
	//  the caller
	Sources GetSources() const;
	void AddSource( Source const& source );

	virtual void GetGameCommandStream( rftl::virtual_iterator<GameCommand>& parser, size_t maxCommands ) const override;
	virtual void TruncateBuffers( time::CommonClock::time_point earliestTime, time::CommonClock::time_point latestTime ) override;

	//
	// Private data
private:
	Sources mSources = {};
};

///////////////////////////////////////////////////////////////////////////////
}

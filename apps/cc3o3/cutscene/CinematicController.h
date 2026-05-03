#pragma once
#include "project.h"

#include "cc3o3/cutscene/CutsceneFwd.h"

#include "GameDialogue/DialogueSequence.h"

#include "PlatformFilesystem/VFSFwd.h"


namespace RF::cc::cutscene {
///////////////////////////////////////////////////////////////////////////////

// TODO
class CinematicController
{
	RF_NO_COPY( CinematicController );

	//
	// Public methods
public:
	CinematicController();

	bool LoadDialogueSequence( file::VFSPath const& filePath );

	dialogue::DialogueSequence const& GetDialogueRef() const;


	//
	// Private data
private:
	dialogue::DialogueSequence mDialog = {};
};

///////////////////////////////////////////////////////////////////////////////
}

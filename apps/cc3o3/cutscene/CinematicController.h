#pragma once
#include "project.h"

#include "cc3o3/cutscene/CutsceneFwd.h"

#include "GameDialogue/DialogueFwd.h"
#include "GameNovel/NovelFwd.h"

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

	dialogue::DialogueSequence const& GetDialogue() const;

	novel::CinematicDriver const& GetDriver() const;
	novel::CinematicDriver& GetMutableDriver();


	//
	// Private data
private:
	UniquePtr<dialogue::DialogueSequence const> const mFallbackDialogue = {};
	UniquePtr<dialogue::DialogueSequence const> mDialogue = {};
	UniquePtr<novel::CinematicDriver> const mDriver = {};
};

///////////////////////////////////////////////////////////////////////////////
}

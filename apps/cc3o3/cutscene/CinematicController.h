#pragma once
#include "project.h"

#include "cc3o3/cutscene/CutsceneFwd.h"

#include "GameDialogue/DialogueFwd.h"
#include "GameNovel/NovelFwd.h"

#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/deque"
#include "rftl/string"


namespace RF::cc::cutscene {
///////////////////////////////////////////////////////////////////////////////

// Operates the cinematic driver, supplying the necessary data and the relevant
//  hookpoints for the game to run the novel engine
class CinematicController
{
	RF_NO_COPY( CinematicController );

	//
	// Public methods
public:
	CinematicController();

	bool SetCharacterData( file::VFSPath const& characterRoot );
	bool SetSceneData( file::VFSPath const& sceneRoot );

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

	rftl::deque<rftl::string> mLoadedExpressionFramePackResourceNames;
};

///////////////////////////////////////////////////////////////////////////////
}

#pragma once

#include "PPU/Object.h"
#include "PlatformFilesystem/VFSFwd.h"

#include <stdint.h>


namespace RF {
///////////////////////////////////////////////////////////////////////////////

class FramePackEditor
{
	//
	// Enums
private:
	enum class MasterMode : uint8_t
	{
		Meta = 0,
		Texture,
		Colliders
	};


	//
	// Public methods
public:
	void Init();
	void Process();
	void Render();

	void Command_ChangePreviewSpeed( bool faster );
	void Command_ChangeEditingFrame( bool increase );
	void Command_OpenFramePack();


	//
	// Private methods
private:
	void OpenFramePack( file::VFSPath const& path );


	//
	// Private data
private:
	MasterMode m_MasterMode;
	gfx::ManagedFramePackID m_FramePackID;
	uint8_t m_EditingFrame;
	gfx::TimeSlowdownRate m_PreviewSlowdownRate;
	gfx::Object m_PreviewObject;
};

///////////////////////////////////////////////////////////////////////////////
}
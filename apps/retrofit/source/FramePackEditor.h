#pragma once

#include "PPU/Object.h"
#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/cstdint"


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
	void Command_ChangeSustainCount( bool increase );
	void Command_ChangeEditingFrame( bool increase );

	void Command_Meta_ChangeDataSpeed( bool faster );
	void Command_Meta_OpenFramePack();

	void Command_Texture_InsertBefore();
	void Command_Texture_InsertAfter();
	void Command_Texture_ChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y );


	//
	// Private methods
private:
	void OpenFramePack( file::VFSPath const& path );
	void InsertTimeSlotBefore( size_t slotIndex );
	void ChangeTexture( size_t slotIndex );


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
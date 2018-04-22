#pragma once

#include "PPU/Object.h"
#include "PlatformFilesystem/VFSFwd.h"

#include "rftl/cstdint"
#include "rftl/string"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

class FramePackEditor
{
	RF_NO_COPY( FramePackEditor );

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
	// Types and constants
private:
	static constexpr char kFramePackName[] = "EDITFPACK";
	static constexpr char kInitialTextureName[] = "EDITFPACK_DEFAULTTEXTURE";


	//
	// Public methods
public:
	FramePackEditor( WeakPtr<file::VFS> const& vfs );

	void Init();
	void Process();
	void Render();

	void Command_ChangePreviewSpeed( bool faster );
	void Command_ChangeSustainCount( bool increase );
	void Command_ChangeEditingFrame( bool increase );
	void Command_ReloadFramePack();

	void Command_Meta_ChangeDataSpeed( bool faster );
	void Command_Meta_CreateFramePack();
	void Command_Meta_OpenFramePack();
	void Command_Meta_DeleteFrame();

	void Command_Texture_InsertBefore();
	void Command_Texture_InsertAfter();
	void Command_Texture_ChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y );
	void Command_Texture_BatchChangeOffset( gfx::PPUCoordElem x, gfx::PPUCoordElem y );


	//
	// Private methods
private:
	void OpenFramePack( rftl::string const& rawPath );
	void OpenFramePack( file::VFSPath const& path );
	void InsertTimeSlotBefore( size_t slotIndex );
	void RemoveTimeSlotAt( size_t slotIndex );
	void ChangeTexture( size_t slotIndex );


	//
	// Private data
private:
	WeakPtr<file::VFS> const m_Vfs;
	MasterMode m_MasterMode;
	gfx::ManagedFramePackID m_FramePackID;
	uint8_t m_EditingFrame;
	gfx::TimeSlowdownRate m_PreviewSlowdownRate;
	gfx::Object m_PreviewObject;
};

///////////////////////////////////////////////////////////////////////////////
}
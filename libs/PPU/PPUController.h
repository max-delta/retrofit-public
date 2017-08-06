#pragma once
#include "project.h"

#include "PPUFwd.h"

#include "core/ptr/unique_ptr.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API PPUController
{
	RF_NO_COPY( PPUController );
	RF_NO_MOVE( PPUController );

	//
	// Public methods
public:
	PPUController() = delete;
	explicit PPUController( UniquePtr<gfx::DeviceInterface> && deviceInterface );
	~PPUController();

	bool Initialize( uint16_t width, uint16_t height );

	bool BeginFrame();
	bool SubmitToRender();
	bool WaitForRender();
	bool EndFrame();


	//
	// Private data
private:
	UniquePtr<gfx::DeviceInterface> m_DeviceInterface;
	UniquePtr<gfx::TextureManager> m_TextureManager;
};

///////////////////////////////////////////////////////////////////////////////
}}

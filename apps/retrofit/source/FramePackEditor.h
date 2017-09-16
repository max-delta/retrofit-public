#pragma once
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


	//
	// Private data
private:
	MasterMode m_MasterMode;
};

///////////////////////////////////////////////////////////////////////////////
}
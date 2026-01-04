#pragma once
#include "project.h"

#include "core_platform/shim/win_shim.h"
#include "core_math/AABB4.h"

#include "rftl/cstdint"
#include "rftl/string_view"


namespace RF::platform::windowing {
///////////////////////////////////////////////////////////////////////////////

enum class WindowStyle : uint8_t
{
	Invalid = 0,

	// Resizable with title bar but no maximize button
	Legacy,

	// More standard windowing style with maximize button
	Standard,

	// Borderless 'full-screen' maximized
	Borderless,
};

// NOTE: Null wndProc will result in the default wndProc being used instead
PLATFORMUTILS_API shim::HWND CreateNewWindow(
	WindowStyle windowStyle,
	rftl::string_view windowTitle,
	uint32_t width,
	uint32_t height,
	shim::WNDPROC wndProc );

PLATFORMUTILS_API math::AABB4i32 GetWindowShape( shim::HWND hWnd );

// Positive: Number of messages processed
// Zero: No processing needed
// Negative: Should quit the application
PLATFORMUTILS_API int32_t ProcessAllMessages();
PLATFORMUTILS_API int32_t ProcessSingleMessage();

///////////////////////////////////////////////////////////////////////////////
}

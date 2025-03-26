#include "stdafx.h"
#include "Debugging.h"

#include "core_platform/inc/windows_inc.h"


namespace RF::platform::debugging {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API void OutputToDebugger( char const* string )
{
	win32::OutputDebugStringA( string );
}



PLATFORMUTILS_API bool IsUserModeDebuggerObviouslyAttached()
{
	// Behind the scenes, this is looking into the PEB, so it's trivially easy
	//  to spoof by just scribbling over that memory
	// NOTE: Functionally this is just a couple fast memory offsets in the
	//  process's address space, and maybe a DLL import jump instruction
	// SEE: TIB @ GS -> GS:[60h] -> PEB -> [PEB+2]
	return win32::IsDebuggerPresent() == win32::TRUE;
}



PLATFORMUTILS_API bool IsKernelModeDebuggerObviouslyAttached()
{
	// Always at the same location, used to exfiltrate data from the kernel to
	//  user-mode code, such as QPC clock sync stuff and whatnot
	static constexpr ptrdiff_t kKernelUserSharedPageOffset = 0x7FFE0000;

	// SEE: https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-kuser_shared_data
	static constexpr ptrdiff_t kDebuggerByteOffset = 0x2D4;

	// Documentation doesn't indicate it as any more than a full bool, but the
	//  conensus is that there's a few specific bits that might get set, so
	//  it's probably better to err on the side of caution, in case the other
	//  bits get reserved for other uses
	static constexpr size_t kDebuggerBits = 0x3;

	// This is where we're going
	uint8_t const* addr =
		reinterpret_cast<uint8_t const*>( 0 ) +
		kKernelUserSharedPageOffset +
		kDebuggerByteOffset;

	// Using structured exception handling just to read a byte... Yep...
	// NOTE: The presence of this safety-check brings in a non-trivial amount
	//  of extra weight to the function, since some extra stack-safety checks
	//  get run to make sure we haven't borked stuff up
	// SEE: _RTC_CheckStackVars (From /RTCs compiler flag? Probably?)
	uint8_t byteOfInterest = 0;
	__try
	{
		byteOfInterest = *addr;
	}
	// NOTE: Produces a warning about an extremely broad exception filter,
	//  which is what we want, because even though it should only be an access
	//  violation at worst, this is all super-sketch, so better to be cautious
	RF_MSVC_INLINE_SUPPRESS( 6320 )
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// Just because accessing random hard-coded memory addresses is dubious
		//  in even the best of cases
		// NOTE: In practice, if this fails, probably every Windows program
		//  under the sun will fail, since tons of random MS-provided library
		//  calls rely on this memory being here, and a lot of those are linked
		//  into shipped code
		RF_DBGFAIL_MSG( "Well then, I guess we WON'T try to look for the kernel debugger..." );
	}

	// Checking for any of the bits to be set
	bool const isPresent = ( byteOfInterest & kDebuggerBits ) != 0;

	return isPresent;
}

///////////////////////////////////////////////////////////////////////////////
}

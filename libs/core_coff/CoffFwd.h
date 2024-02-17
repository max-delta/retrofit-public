#pragma once

#include "rftl/cstdint"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

enum class MachineType : uint16_t
{
	Unknown = 0x0,

	Intel386 = 0x14c,
	IntelItanium = 0x200,

	AMD64 = 0x8664,

	ARM = 0x1c0,
	ARM64 = 0xaa64,
	ARMThumb = 0x1c2,
	ARMThumb2 = 0x1c4,

	PowerPC = 0x1f0,
	PowerPCFpu = 0x1f1,

	MatsuhitaAM33 = 0x1d3,

	AlphaAXP32 = 0x184,
	AlphaAXP64 = 0x284,

	MIPSR4000 = 0x166,
	MIPS16 = 0x266,
	MIPSFpu = 0x366,
	MIPS16Fpu = 0x466,

	EBC = 0xebc,

	Loong32 = 0x6232,
	Loong64 = 0x6264,

	MitsubishiM32R = 0x9041,

	RISCV32 = 0x5032,
	RISCV64 = 0x5064,
	RISCV128 = 0x5128,

	HitachiSH3 = 0x1a2,
	HitachiSH3DSP = 0x1a3,
	HitachiSH4 = 0x1a6,
	HitachiSH5 = 0x1a8,

	MIPSWCE2 = 0x169,
};

enum class OptionalHeaderType : uint16_t
{
	Invalid = 0x0,

	PE32 = 0x10b,
	PE32Plus = 0x20b,
};

enum class WindowsSubsystemType : uint16_t
{
	Unknown = 0,

	NativeNT = 1,
	WindowsNTGUI = 2,
	WindowsNTCUI = 3,
	OS2CUI = 5,
	POSIXCUI = 7,
	NativeWin = 8,
	WindowsCEGUI = 9,
	EFIApplication = 10,
	EFIBootDriver = 11,
	EFIRuntimeDriver = 12,
	EFIROM = 13,
	XBox = 14,
	WindowsBoot = 15,
};

struct CoffHeader;
struct OptionalHeaderCommon;
struct OptionalHeaderWindows;
struct SectionHeader;

///////////////////////////////////////////////////////////////////////////////
}

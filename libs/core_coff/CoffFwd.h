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

enum class SymbolTypeUpper : uint8_t
{
	Scalar = 0,
	Pointer = 1,
	Function = 2,
	Array = 3,
};

enum class SymbolTypeLower : uint8_t
{
	Unknown = 0,
	Void = 1,
	Char = 2,
	Short = 3,
	Int = 4,
	Long = 5,
	Float = 6,
	Double = 7,
	Struct = 8,
	Union = 9,
	Enum = 10,
	EnumMember = 11,
	UChar = 12,
	UShort = 13,
	UInt = 14,
	ULong = 15,
};

enum class SymbolStorageClass : uint8_t
{
	Null = 0,
	Stack = 1, // Value = stack offset
	External = 2, // Value = section == 0 ? size : offset into section
	Static = 3,
	Register = 4, // Value = register number
	ExternalDefinition = 5,
	Label = 6, // Value = offset into section
	UndefinedLabel = 7,
	MemberOfStruct = 8, // Value = index into members
	FunctionArgument = 9, // Value = indext into params
	StructTag = 10,
	MemberOfUnion = 11, // Value = index into members
	UnionTag = 12,
	TypeDef = 13,
	UndefinedStatic = 14,
	EnumTag = 15,
	MemberOfEnum = 16, // Value = index into members
	RegisterParam = 17,
	BitField = 18, // Value = index into bits
	Block = 100, // Value = relocatable address
	Function = 101, // Value = (lf ? number of sourc lines, rf ? size of code)
	EndOfStruct = 102,
	File = 103, // Expect an aux record with name of file
	WeakExternal = 105,
	CLRToken = 107,
};

struct CoffHeader;
struct OptionalHeaderCommon;
struct OptionalHeaderWindows;
struct SectionHeader;

///////////////////////////////////////////////////////////////////////////////
}

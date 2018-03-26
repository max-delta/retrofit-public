#pragma once

namespace RF { namespace compiler {
///////////////////////////////////////////////////////////////////////////////

enum class Compiler
{
	Invalid = 0,
	MSVC
};

enum class Architecture
{
	Invalid = 0,
	x86_32,
	x86_64
};

enum class MemoryModel
{
	Invalid = 0,
	Strong,
	Weak
};

enum class Endianness
{
	Invalid = 0,
	Big, // Normal
	Little, // Swapped
	Variable // :(
};

// The C++ standard defines special behavior for null pointers that sometimes
//  acts fundamentally different than normal pointers, by design. There are
//  cases where this is undesirable, but the situation is such that it is
//  impossible or impractical to acquire a real pointer to an actual object.
//  This value can act as a stand-in that is obviously invalid, so memory
//  access should cause an obvious segmentation fault.
// NOTE: Not garaunteed to be invalid, very compiler and platform specific, but
//  conventionally invalid in modern memory management systems
// NOTE: This is a fundamental standards violation and rife with undefined
//  behavior, so be very cautious when using it
static void const* const kInvalidNonNullPointer = reinterpret_cast<void const*>( 0x1 );

#ifdef _MSC_VER
	#define RF_PLATFORM_MSVC
	constexpr Compiler kCompiler = Compiler::MSVC;

	#pragma intrinsic(__debugbreak)
	#define RF_SOFTWARE_INTERRUPT() __debugbreak()

	#define RF_NO_INLINE __declspec(noinline)
	#define RF_ACK_ANY_PADDING __pragma(warning(suppress:4324))

	#ifdef _M_AMD64
		#define RF_PLATFORM_X86_64
		#define RF_PLATFORM_STRONG_MEMORY_MODEL
		#define RF_PLATFORM_LITTLE_ENDIAN
		#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
		#define RF_PLATFORM_POINTER_BYTES 8u
		constexpr Architecture kArchitecture = Architecture::x86_64;
		constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
		constexpr Endianness kEndianness = Endianness::Little;
		constexpr bool kAlignedModificationsAreAtomic = true;
		#define RF_ACK_64BIT_PADDING __pragma(warning(suppress:4324))
		#define RF_ACK_32BIT_PADDING
	#elif defined(_M_IX86)
		#define RF_PLATFORM_X86_32
		#define RF_PLATFORM_STRONG_MEMORY_MODEL
		#define RF_PLATFORM_LITTLE_ENDIAN
		#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
		#define RF_PLATFORM_POINTER_BYTES 4u
		constexpr Architecture kArchitecture = Architecture::x86_32;
		constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
		constexpr Endianness kEndianness = Endianness::Little;
		constexpr bool kAlignedModificationsAreAtomic = true;
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING __pragma(warning(suppress:4324))
	#elif defined(_M_ARM64)
		#error Verify and add support
		#define RF_PLATFORM_ARM_64
		#define RF_PLATFORM_VARIABLE_ENDIAN
		#define RF_PLATFORM_POINTER_BYTES 8u
		constexpr Architecture kArchitecture = Architecture::Invalid;
		constexpr MemoryModel kMemoryModel = MemoryModel::Invalid;
		constexpr Endianness kEndianness = Endianness::Variable;
		constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
		#define RF_ACK_64BIT_PADDING __pragma(warning(suppress:4324))
		#define RF_ACK_32BIT_PADDING
	#elif defined(_M_ARM)
		#error Verify and add support
		#define RF_PLATFORM_ARM_32
		#define RF_PLATFORM_VARIABLE_ENDIAN
		#define RF_PLATFORM_POINTER_BYTES 4u
		constexpr Architecture kArchitecture = Architecture::Invalid;
		constexpr MemoryModel kMemoryModel = MemoryModel::Invalid;
		constexpr Endianness kEndianness = Endianness::Variable;
		constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING __pragma(warning(suppress:4324))
	#else
		#error Undefined architecture
	#endif
#else
	#error Undefined platform
#endif

#ifdef RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	#define RF_ALIGN_ATOMIC_POINTER alignas(RF_PLATFORM_POINTER_BYTES)
#else
	// Intentionally undefined due to it being unsafe
#endif

///////////////////////////////////////////////////////////////////////////////
}}

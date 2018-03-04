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

#ifdef _MSC_VER
	#define RF_PLATFORM_MSVC
	constexpr Compiler kCompiler = Compiler::MSVC;

	#pragma intrinsic(__debugbreak)
	#define RF_SOFTWARE_INTERRUPT() __debugbreak()

	#define RF_NO_INLINE __declspec(noinline)

	#ifdef _M_AMD64
		#define RF_PLATFORM_X86_64
		#define RF_PLATFORM_STRONG_MEMORY_MODEL
		#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
		#define RF_PLATFORM_POINTER_BYTES 8u
		constexpr Architecture kArchitecture = Architecture::x86_64;
		constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
		constexpr bool kAlignedModificationsAreAtomic = true;
	#elif defined(_M_IX86)
		#define RF_PLATFORM_X86_32
		#define RF_PLATFORM_STRONG_MEMORY_MODEL
		#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
		#define RF_PLATFORM_POINTER_BYTES 4u
		constexpr Architecture kArchitecture = Architecture::x86_32;
		constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
		constexpr bool kAlignedModificationsAreAtomic = true;
	#elif defined(_M_ARM64)
		#error Verify and add support
		#define RF_PLATFORM_ARM_64
		#define RF_PLATFORM_POINTER_BYTES 8u
		constexpr Architecture kArchitecture = Architecture::Invalid;
		constexpr MemoryModel kMemoryModel = MemoryModel::Invalid;
		constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
	#elif defined(_M_ARM)
		#error Verify and add support
		#define RF_PLATFORM_ARM_32
		#define RF_PLATFORM_POINTER_BYTES 4u
		constexpr Architecture kArchitecture = Architecture::Invalid;
		constexpr MemoryModel kMemoryModel = MemoryModel::Invalid;
		constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
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

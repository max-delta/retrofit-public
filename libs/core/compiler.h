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
	#define RF_ACK_UNSAFE_INHERITANCE __pragma(warning(suppress:4435))
	#define RF_ACK_CONSTEXPR_SIGN_MISMATCH __pragma(warning(suppress:4287))

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

// Atomic ordering quick-reference
// ASSUME:
//  std::atomic<uint64_t> val; <- used for the function case
//  uint64_t val; <- used for the '=' case
#if defined( RF_PLATFORM_X86_64 )
// val;
// val.load(relaxed)
// val.load(acquire)
// val.load(sequence)
//   mov rax, [val]
// val = 7
// val.store(7, relaxed)
// val.store(7, release)
//   mov [val], 7
// val.store(7, sequence)
//   mov [val], 7
//   mfence
// val.exchange(7, relaxed)
// val.exchange(7, release)
// val.exchange(7, acquire)
// val.exchange(7, sequence)
//   mov rax, 7
//   xchg rax, [val]
#elif defined( RF_PLATFORM_ARM_64 )
// val;
//   mov r0, val
//   ldr r0, [r0]
// val.load(relaxed)
//   mov r0, val
//   ldr r0, [r0]
// val.load(acquire)
//   mov r0, val
//   ldr r0, [r0]
//   dmb ish
// val.load(sequence)
//   mov r0, val
//   dmb ish
//   ldr r0, [r0]
//   dmb ish
// val = 7
//   mov r2, 7
//   ldr r3, val
//   str r2, [r3]
// val.store(7, relaxed)
//   mov r1, 7
//   mov r0, val
//   str r1, [r0]
// val.store(7, release)
//   mov r1, 7
//   mov r0, val
//   dmb ish
//   str r1, [r0]
// val.store(7, sequence)
//   mov r1, 7
//   mov r0, val
//   dmb ish
//   str r1, [r0]
//   dmb ish
// val.exchange(7, relaxed)
//   mov r1, 7
//   mov r0, val
//   <jmp target>
//   ldrex r2, [r0]
//   strex r2, r1, [r0]
//   cmp r2, 0
//   bne <jmp target>
// val.exchange(7, release)
//   mov r1, 7
//   mov r0, val
//   <jmp target>
//   ldrex r2, [r0]
//   strex r2, r1, [r0]
//   cmp r2, 0
//   bne <jmp target>
//   dmb ish
// val.exchange(7, acquire)
//   mov r1, 7
//   mov r0, val
//   dmb ish
//   <jmp target>
//   ldrex r2, [r0]
//   strex r2, r1, [r0]
//   cmp r2, 0
//   bne <jmp target>
// val.exchange(7, sequence)
//   mov r1, 7
//   mov r0, val
//   dmb ish
//   <jmp target>
//   ldrex r2, [r0]
//   strex r2, r1, [r0]
//   cmp r2, 0
//   bne <jmp target>
//   dmb ish
#endif

///////////////////////////////////////////////////////////////////////////////
}}

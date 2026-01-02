#pragma once

#include "core/macros.h"


namespace RF::compiler {
///////////////////////////////////////////////////////////////////////////////

enum class Compiler
{
	Invalid = 0,
	MSVC,
	Clang
};

enum class OperatingSystem
{
	Invalid = 0,
	Windows
};

enum class Architecture
{
	Invalid = 0,
	x86_32,
	x86_64,
	ARM_32,
	ARM_64
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



#if defined(_WIN32)
	#define RF_PLATFORM_WINDOWS
	static constexpr OperatingSystem kOperatingSystem = OperatingSystem::Windows;
#else
	#error Undefined platform
#endif


#if defined( _MSC_VER ) && !defined( __clang__ ) // NOTE: clang-cl pretends to be MSVC
	#define RF_PLATFORM_MSVC
	static constexpr Compiler kCompiler = Compiler::MSVC;

	#define RF_TOOL_DESC "MSVC_" RF_STRINGIFY( _MSC_FULL_VER )
	static constexpr char const kToolDesc[] = RF_TOOL_DESC;

	#pragma intrinsic( __debugbreak )
	#define RF_SOFTWARE_INTERRUPT() __debugbreak()
	#define RF_SOFTWARE_FASTFAIL() __fastfail( 0xFFFFFFFF )

	#define RF_MSVC_INLINE_SUPPRESS( x ) __pragma( warning( suppress : x ) )

	#define RF_CLANG_PRAGMA( x )
	#define RF_CLANG_PUSH()
	#define RF_CLANG_POP()
	#define RF_CLANG_IGNORE( WARNING )

	#define RF_NOP() __nop()
	#define RF_NO_INLINE __declspec( noinline )
	#define RF_NO_LINK_STRIP __declspec( dllexport )
	#define RF_ACK_LINK_STRIP RF_MSVC_INLINE_SUPPRESS( 5245 )
	#define RF_ACK_ANY_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#define RF_ACK_UNSAFE_INHERITANCE RF_MSVC_INLINE_SUPPRESS( 4435 )
	#define RF_ACK_CONSTEXPR_SIGN_MISMATCH RF_MSVC_INLINE_SUPPRESS( 4287 )

	#define RF_BUILD_MESSAGE( MESSAGE ) __pragma( message( "" MESSAGE "" ) )

	#define RF_DECLARE_SECTION_RW( NAME ) __pragma( section( NAME, read, write ) )
	#define RF_ASSIGN_TO_SECTION( NAME ) __declspec( allocate( NAME ) )

	#ifdef _M_AMD64
		#define RF_PLATFORM_X86_64
		#define RF_ACK_64BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
		#define RF_ACK_32BIT_PADDING
	#elif defined( _M_IX86 )
		#define RF_PLATFORM_X86_32
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#elif defined( _M_ARM64 )
		#define RF_PLATFORM_ARM_64
		#define RF_ACK_64BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
		#define RF_ACK_32BIT_PADDING
	#elif defined( _M_ARM )
		#define RF_PLATFORM_ARM_32
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#else
		#error Undefined architecture
	#endif

	// TODO: Figure out a technique for aggregate initialization that
	//  is warning-free in C++20, since the standards made a mess of
	//  the whole thing
	#define RF_ACK_AGGREGATE_NOCOPY() RF_MSVC_INLINE_SUPPRESS( 4623 4626 ); // Implicit deletion

#elif defined( __clang__ )
	#define RF_PLATFORM_CLANG
	static constexpr Compiler kCompiler = Compiler::Clang;

	#ifndef _MSC_VER
		#error Normal clang not supported yet
	#endif

	#define RF_TOOL_DESC "ClangCL_" __clang_version__ RF_STRINGIFY( _MSC_FULL_VER )
	static constexpr char const kToolDesc[] = RF_TOOL_DESC;

	#pragma intrinsic( __debugbreak )
	#define RF_SOFTWARE_INTERRUPT() __debugbreak()
	#define RF_SOFTWARE_FASTFAIL() __fastfail( 0xFFFFFFFF )

	#define RF_MSVC_INLINE_SUPPRESS( x ) __pragma( warning( suppress : x ) )

	#define RF_CLANG_PRAGMA( x ) _Pragma( RF_STRINGIFY( x ) )
	#define RF_CLANG_PUSH() RF_CLANG_PRAGMA( clang diagnostic push )
	#define RF_CLANG_POP() RF_CLANG_PRAGMA( clang diagnostic pop )
	#define RF_CLANG_IGNORE( WARNING ) RF_CLANG_PRAGMA( clang diagnostic ignored WARNING )

	#define RF_NOP() asm volatile("nop")
	#define RF_NO_INLINE __declspec( noinline )
	#define RF_NO_LINK_STRIP __declspec( dllexport )
	#define RF_ACK_LINK_STRIP RF_MSVC_INLINE_SUPPRESS( 5245 )
	#define RF_ACK_ANY_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#define RF_ACK_UNSAFE_INHERITANCE RF_MSVC_INLINE_SUPPRESS( 4435 )
	#define RF_ACK_CONSTEXPR_SIGN_MISMATCH RF_MSVC_INLINE_SUPPRESS( 4287 )

	#define RF_BUILD_MESSAGE( MESSAGE )

	#define RF_DECLARE_SECTION_RW( NAME ) __pragma( section( NAME, read, write ) )
	#define RF_ASSIGN_TO_SECTION( NAME ) __declspec( allocate( NAME ) )

	#ifdef _M_AMD64
		#define RF_PLATFORM_X86_64
		#define RF_ACK_64BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
		#define RF_ACK_32BIT_PADDING
	#elif defined( _M_IX86 )
		#define RF_PLATFORM_X86_32
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#elif defined( _M_ARM64 )
		#error Verify and add support
		#define RF_PLATFORM_ARM_64
		#define RF_ACK_64BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
		#define RF_ACK_32BIT_PADDING
	#elif defined( _M_ARM )
		#error Verify and add support
		#define RF_PLATFORM_ARM_32
		#define RF_ACK_64BIT_PADDING
		#define RF_ACK_32BIT_PADDING RF_MSVC_INLINE_SUPPRESS( 4324 )
	#else
		#error Undefined architecture
	#endif

	// TODO: Figure out a technique for aggregate initialization that
	//  is warning-free in C++20, since the standards made a mess of
	//  the whole thing
	#define RF_ACK_AGGREGATE_NOCOPY() RF_MSVC_INLINE_SUPPRESS( 4623 4626 ); // Implicit deletion

#else
	#error Undefined platform
#endif



#if defined( RF_PLATFORM_X86_64 )
	static constexpr Architecture kArchitecture = Architecture::x86_64;
	#define RF_PLATFORM_STRONG_MEMORY_MODEL
	static constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
	#define RF_PLATFORM_LITTLE_ENDIAN
	static constexpr Endianness kEndianness = Endianness::Little;
	#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	static constexpr bool kAlignedModificationsAreAtomic = true;
	#define RF_PLATFORM_POINTER_BYTES 8u
	static constexpr size_t kPointerBytes = RF_PLATFORM_POINTER_BYTES;
	#define RF_MIN_PAGE_SIZE 4096u
	static constexpr unsigned long kMinPageSize = 4096u;
#elif defined( RF_PLATFORM_X86_32 )
	static constexpr Architecture kArchitecture = Architecture::x86_32;
	#define RF_PLATFORM_STRONG_MEMORY_MODEL
	static constexpr MemoryModel kMemoryModel = MemoryModel::Strong;
	#define RF_PLATFORM_LITTLE_ENDIAN
	static constexpr Endianness kEndianness = Endianness::Little;
	#define RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	static constexpr bool kAlignedModificationsAreAtomic = true;
	#define RF_PLATFORM_POINTER_BYTES 4u
	static constexpr size_t kPointerBytes = RF_PLATFORM_POINTER_BYTES;
	#define RF_MIN_PAGE_SIZE 4096u
	static constexpr unsigned long kMinPageSize = 4096u;
#elif defined( RF_PLATFORM_ARM_64 )
	// TODO: Verify all of this
	// NOTE: May be toolchain-specific depending on target OS
	static constexpr Architecture kArchitecture = Architecture::ARM_64;
	static constexpr MemoryModel kMemoryModel = MemoryModel::Weak; // Verify?
	#if defined( RF_PLATFORM_WINDOWS )
		// Microsoft formally declares Windows on ARM64 to use little-endian
		//  mode only
		// SEE: Microsoft ARM64 ABI Conventions
		#define RF_PLATFORM_LITTLE_ENDIAN
		static constexpr Endianness kEndianness = Endianness::Little;
	#else
		#define RF_PLATFORM_VARIABLE_ENDIAN
		static constexpr Endianness kEndianness = Endianness::Variable;
	#endif
	static constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
	#define RF_PLATFORM_POINTER_BYTES 8u
	static constexpr size_t kPointerBytes = RF_PLATFORM_POINTER_BYTES;
	#define RF_MIN_PAGE_SIZE 4096u /* Verify? */
	static constexpr unsigned long kMinPageSize = 4096u; // Verify?
#elif defined( RF_PLATFORM_ARM_32 )
	// TODO: Verify all of this
	// NOTE: May be toolchain-specific depending on target OS
	static constexpr Architecture kArchitecture = Architecture::ARM_32;
	static constexpr MemoryModel kMemoryModel = MemoryModel::Weak; // Verify?
	#if defined( RF_PLATFORM_WINDOWS )
		// Microsoft formally declares Windows on ARM32 to use little-endian
		//  mode only
		// SEE: Microsoft ARM32 ABI Conventions
		#define RF_PLATFORM_LITTLE_ENDIAN
		static constexpr Endianness kEndianness = Endianness::Little;
	#else
		#define RF_PLATFORM_VARIABLE_ENDIAN
		static constexpr Endianness kEndianness = Endianness::Variable;
	#endif
	static constexpr bool kAlignedModificationsAreAtomic = false; // Verify?
	#define RF_PLATFORM_POINTER_BYTES 4u
	static constexpr size_t kPointerBytes = RF_PLATFORM_POINTER_BYTES;
	#define RF_MIN_PAGE_SIZE 4096u /* Verify? */
	static constexpr unsigned long kMinPageSize = 4096u; // Verify?
#else
	#error Undefined platform
#endif


static_assert( sizeof( void* ) == kPointerBytes, "Pointer size mismatch" );
static_assert( alignof( void* ) == sizeof( void* ), "Unexpected pointer alignment, verify this is correct" );
#ifdef RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	#define RF_ALIGN_ATOMIC_POINTER alignas( RF_PLATFORM_POINTER_BYTES )
#else
	// Intentionally undefined due to it being unsafe
#endif



// Atomic ordering quick-reference
// ASSUME:
//  rftl::atomic<uint64_t> val; <- used for the function case
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
}

// MSVC needs some intrinsics declared before they can be invoked
#if defined( RF_PLATFORM_MSVC )
extern "C" void __nop();
__pragma( intrinsic( __nop ) );
extern "C" __declspec( noreturn ) void __fastfail( unsigned int );
__pragma( intrinsic( __fastfail ) );
	#if defined( RF_PLATFORM_X86_64 )
extern "C" void __cpuidex( int[4], int, int );
__pragma( intrinsic( __cpuidex ) );
	#endif
#endif

// Clang needs some intrinsics declared before they can be invoked
#if defined( RF_PLATFORM_CLANG )
	#if defined( RF_PLATFORM_X86_64 )
extern "C" void __cpuidex( int[4], int, int );
	#endif
#endif

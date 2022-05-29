#include "stdafx.h"

#include "core/compiler.h"


namespace RF::compiler {
///////////////////////////////////////////////////////////////////////////////

TEST( Compiler, Constants )
{
	// Mostly just making sure the variables were defined
	static_assert( kCompiler != Compiler::Invalid, "Unsupported" );
	static_assert( kOperatingSystem != OperatingSystem::Invalid, "Unsupported" );
	static_assert( kArchitecture != Architecture::Invalid, "Unsupported" );
	static_assert( kMemoryModel != MemoryModel::Invalid, "Unsupported" );
	static_assert( kEndianness != Endianness::Invalid, "Unsupported" );

	static_assert( RF_PLATFORM_POINTER_BYTES == sizeof( void* ), "Wrong size" );
}



TEST( Compiler, Endian )
{
	char const bytes[4] = { 1, 2, 3, 4 };
#ifdef RF_PLATFORM_LITTLE_ENDIAN
	ASSERT_EQ( *reinterpret_cast<uint32_t const*>( &bytes[0] ), 0x04030201 );
#else
	ASSERT_EQ( *reinterpret_cast<uint32_t const*>( &bytes[0] ), 0x01020304 );
#endif
}



TEST( Compiler, AtomicPointerStackAlignment )
{
#ifdef RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	char padding[3];
	(void)padding;
	RF_ALIGN_ATOMIC_POINTER volatile void* pointer;
	ASSERT_EQ( ( reinterpret_cast<size_t>( &pointer ) % RF_PLATFORM_POINTER_BYTES ), 0 );
#endif
}



TEST( Compiler, AtomicPointerStaticAlignment )
{
#ifdef RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC
	RF_ALIGN_ATOMIC_POINTER volatile static void* pointer;
	ASSERT_EQ( ( reinterpret_cast<size_t>( &pointer ) % RF_PLATFORM_POINTER_BYTES ), 0 );
#endif
}

///////////////////////////////////////////////////////////////////////////////
}

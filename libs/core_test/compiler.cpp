#include "stdafx.h"

#include "core/compiler.h"


namespace RF { namespace compiler {
///////////////////////////////////////////////////////////////////////////////

TEST(Compiler, Constants)
{
	// Mostly just making sure the variables were defined
	static_assert( kCompiler != Compiler::Invalid, "Unsupported" );
	static_assert( kArchitecture != Architecture::Invalid, "Unsupported" );
	static_assert( kMemoryModel != MemoryModel::Invalid, "Unsupported" );

	static_assert( RF_PLATFORM_POINTER_BYTES == sizeof( void* ), "Wrong size" );
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
}}

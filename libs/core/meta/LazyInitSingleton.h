#pragma once

#include "core/compiler.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// NOTE: This is dangerous to use directly, and should be wrapped in a helper
//  function where it can appropriately control cross-DLL behavior as needed
template<typename SINGLETON>
SINGLETON & GetOrInitFunctionStaticScopedSingleton()
{
	#if defined(RF_PLATFORM_ALIGNED_MODIFICATIONS_ARE_ATOMIC) && defined(RF_PLATFORM_STRONG_MEMORY_MODEL)
	{
		// What is all this weird shit? Why not just return a reference to the
		//  static variable directly? Well... you need to look at the assembly, but
		//  this reduces some of the heavy-weight function-static thread-safety
		//  code that gets emitted behind the scenes, atleast in VS2017. Not
		//  advisable to do this pattern often, and should be reviewed occasionally
		//  in the assembly to confirm it's working as expected.
		// Some keywords you can look at for some idea about what's going on here:
		//  * Function-local static initialization concurrency
		//  * BSS segment
		//   * Zero page list
		//  * Strong memory model
		//  * L1 cache-line
		//   * Aligned memory access
		//    * Atomic read/write
		RF_ALIGN_ATOMIC_POINTER volatile static SINGLETON* singletonPointer = nullptr;
		if( singletonPointer == nullptr )
		{
			static SINGLETON singleton = {};

			// This isn't obviously thread-safe since multiple threads could enter
			//  the if, but only one would create the object, and then they would
			//  all write the same value anyways since they're just writing the
			//  address of a link-time-known variable to another link-time-known
			//  variable
			singletonPointer = &singleton;
		}
		return const_cast<SINGLETON &>( *singletonPointer );
	}
	#else
	{
		static SINGLETON singleton = SINGLETON();
		return singleton;
	}
	#endif
}

///////////////////////////////////////////////////////////////////////////////
}

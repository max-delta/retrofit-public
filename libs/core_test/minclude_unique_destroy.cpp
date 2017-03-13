#include "stdafx.h"

// Do not need type to be defined to destroy it
#include "core/ptr/unique_ptr.h"
namespace {
	class ForwardedClass;
	void test(RF::UniquePtr<ForwardedClass> && uptr)
	{
		uptr = nullptr;
	}
}

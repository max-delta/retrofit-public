#include "stdafx.h"

// Do not need type to be defined to destroy it
#include "core/ptr/weak_ptr.h"
namespace {

RF_CLANG_IGNORE( "-Wunused-function" );

class ForwardedClass;
void test( RF::WeakPtr<ForwardedClass>&& wptr )
{
	wptr = nullptr;
}

}

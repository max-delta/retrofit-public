#include "stdafx.h"

#include "core/ptr/weak_ptr.h"
namespace {

RF_CLANG_IGNORE( "-Wunused-function" );

class ForwardedClass;

// Do not need type to be defined to destroy it
RF_ACK_LINK_STRIP void test_destroy( RF::WeakPtr<ForwardedClass>&& wptr )
{
	wptr = nullptr;
}

// Do not need type to be defined to assign it
RF_ACK_LINK_STRIP void test_assign( RF::WeakPtr<ForwardedClass>&& wptr )
{
	RF::WeakPtr<ForwardedClass> same{ wptr };
	same = wptr;

	RF::WeakPtr<ForwardedClass const> cv{ wptr };
	cv = wptr;
}

}

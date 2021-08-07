#include "stdafx.h"

#include "core/ptr/shared_ptr.h"
namespace {

RF_CLANG_IGNORE( "-Wunused-function" );

class ForwardedClass;

// Do not need type to be defined to destroy it
void test_destroy( RF::SharedPtr<ForwardedClass>&& sptr )
{
	sptr = nullptr;
}

// Do not need type to be defined to assign it
void test_assign( RF::SharedPtr<ForwardedClass>&& sptr )
{
	RF::SharedPtr<ForwardedClass> same{ sptr };
	same = sptr;

	RF::SharedPtr<ForwardedClass const> cv{ sptr };
	cv = sptr;
}

}

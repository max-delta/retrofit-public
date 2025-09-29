#include "stdafx.h"

#include "core/ptr/unique_ptr.h"
namespace {

RF_CLANG_IGNORE( "-Wunused-function" );

class ForwardedClass;

// Do not need type to be defined to destroy it
RF_ACK_LINK_STRIP void test_destroy( RF::UniquePtr<ForwardedClass>&& uptr )
{
	uptr = nullptr;
}

// Do not need type to be defined to assign it
RF_ACK_LINK_STRIP void test_assign( RF::UniquePtr<ForwardedClass>&& uptr )
{
	RF::UniquePtr<ForwardedClass> same{ rftl::move( uptr ) };
	same = rftl::move( uptr );

	RF::UniquePtr<ForwardedClass const> cv{ rftl::move( uptr ) };
	cv = rftl::move( uptr );
}

}

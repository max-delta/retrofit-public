#include "stdafx.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/cstdlib"


RF_CLANG_IGNORE( "-Wself-move" );

namespace RF {
///////////////////////////////////////////////////////////////////////////////

namespace {

struct HugeStruct
{
	uint64_t member[100];
};

}
static_assert(
	sizeof( UniquePtr<HugeStruct> ) < sizeof( HugeStruct ),
	"UniquePtr is embedding storage" );



TEST( UniquePtr, Defaults )
{
	{
		UniquePtr<int> uptr;
		ASSERT_TRUE( uptr == nullptr );
	}
	{
		UniquePtr<int> uptr = UniquePtr<int>();
		ASSERT_TRUE( uptr == nullptr );
	}
	{
		UniquePtr<int> uptr = nullptr;
		ASSERT_TRUE( uptr == nullptr );
	}
}



TEST( UniquePtr, SoloLifecycle )
{
	// Normal
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( uptr != nullptr );
		ASSERT_TRUE( *uptr == 47 );
		*uptr = 53;
		ASSERT_TRUE( *uptr == 53 );
	}

	// Deferred create
	{
		auto creation = DefaultCreator<int>::Create( 47 );
		{
			UniquePtr<int> uptr = rftl::move( creation );
			ASSERT_TRUE( uptr != nullptr );
			ASSERT_TRUE( *uptr == 47 );
			*uptr = 53;
			ASSERT_TRUE( *uptr == 53 );
		}
	}

	// Pre-mature termination
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( uptr != nullptr );
		uptr = nullptr;
		ASSERT_TRUE( uptr == nullptr );
	}
}



TEST( UniquePtr, WeakLifecycle )
{
	// Weak outlives
	{
		WeakPtr<int> wptr;
		ASSERT_TRUE( wptr == nullptr );
		{
			UniquePtr<int> uptr = DefaultCreator<int>::Create( 47 );
			wptr = uptr;
			ASSERT_TRUE( wptr != nullptr );
		}
		ASSERT_TRUE( wptr == nullptr );
	}

	// Unique outlives
	{
		UniquePtr<int> uptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( uptr != nullptr );
		{
			WeakPtr<int> wptr = uptr;
			ASSERT_TRUE( wptr != nullptr );
		}
		ASSERT_TRUE( uptr != nullptr );
	}

	// Moves to an already assigned pointer should be destructive
	{
		UniquePtr<int> uptr1 = DefaultCreator<int>::Create( 83 );
		ASSERT_TRUE( uptr1 != nullptr );
		UniquePtr<int> uptr2 = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( uptr2 != nullptr );
		{
			WeakPtr<int> wptr1 = uptr1;
			ASSERT_TRUE( wptr1 != nullptr );
			ASSERT_TRUE( *wptr1 == 83 );
			WeakPtr<int> wptr2 = uptr2;
			ASSERT_TRUE( wptr2 != nullptr );
			ASSERT_TRUE( *wptr2 == 47 );
			wptr1 = rftl::move( wptr2 );
			ASSERT_TRUE( wptr1 != nullptr );
			ASSERT_TRUE( *wptr1 == 47 );
			ASSERT_TRUE( wptr2 == nullptr );
		}
		ASSERT_TRUE( uptr1 != nullptr );
		ASSERT_TRUE( uptr2 != nullptr );
	}
}



TEST( UniquePtr, Move )
{
	// Move to longer-lived unique
	{
		UniquePtr<int> uptr2;
		ASSERT_TRUE( uptr2 == nullptr );
		{
			UniquePtr<int> uptr1 = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( uptr1 != nullptr );
			ASSERT_TRUE( *uptr1 == 47 );
			uptr2 = rftl::move( uptr1 );
			ASSERT_TRUE( uptr1 == nullptr );
			ASSERT_TRUE( uptr2 != nullptr );
			ASSERT_TRUE( *uptr2 == 47 );
		}
		ASSERT_TRUE( uptr2 != nullptr );
		ASSERT_TRUE( *uptr2 == 47 );
	}

	// Moves to an already assigned pointer should be destructive
	{
		UniquePtr<int> uptr2 = DefaultCreator<int>::Create( 83 );
		ASSERT_TRUE( uptr2 != nullptr );
		ASSERT_TRUE( *uptr2 == 83 );
		{
			UniquePtr<int> uptr1 = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( uptr1 != nullptr );
			ASSERT_TRUE( *uptr1 == 47 );
			uptr2 = rftl::move( uptr1 );
			ASSERT_TRUE( uptr1 == nullptr );
			ASSERT_TRUE( uptr2 != nullptr );
			ASSERT_TRUE( *uptr2 == 47 );
		}
		ASSERT_TRUE( uptr2 != nullptr );
		ASSERT_TRUE( *uptr2 == 47 );
	}
}



TEST( UniquePtr, MoveDerived )
{
	struct Base
	{
		virtual ~Base() = default;
		int base;
	};
	struct Derived : public Base
	{
		int derived;
	};

	// Construct into move
	{
		UniquePtr<Derived> uptr1 = DefaultCreator<Derived>::Create();
		ASSERT_TRUE( uptr1 != nullptr );
		UniquePtr<Base> uptr2( rftl::move( uptr1 ) );
		ASSERT_TRUE( uptr1 == nullptr );
		ASSERT_TRUE( uptr2 != nullptr );
	}
}



TEST( UniquePtr, MoveIntoTrash )
{
	UniquePtr<int> uptr1 = DefaultCreator<int>::Create( 5 );
	{
		void* alloc = ::operator new( sizeof( UniquePtr<int> ) );
		ASSERT_NE( alloc, nullptr );
		if( alloc == nullptr )
		{
			rftl::abort();
		}
		memset( alloc, 0xcc, sizeof( UniquePtr<int> ) );
		UniquePtr<int>* newAlloc = new( alloc ) UniquePtr<int>( rftl::move( uptr1 ) );
		newAlloc->~UniquePtr();
		::operator delete( alloc );
	}
}



TEST( UniquePtr, Arrow )
{
	struct Test
	{
		int test;
	};

	UniquePtr<Test> uptr = DefaultCreator<Test>::Create();
	( *uptr ).test = 9;
	ASSERT_TRUE( ( *uptr ).test == 9 );
	ASSERT_TRUE( uptr->test == 9 );
}



TEST( UniquePtr, CastToBase )
{
	struct Base
	{
		virtual ~Base() = default;
		int base;
	};
	struct Derived : public Base
	{
		int derived;
	};
	struct Unrelated
	{
		int unrelated;
	};

	UniquePtr<Derived> uptr_d = nullptr;
	UniquePtr<Base> uptr_b = nullptr;
	UniquePtr<Unrelated> uptr_u = nullptr;
	WeakPtr<Derived> wptr_d = nullptr;
	WeakPtr<Base> wptr_b = nullptr;
	WeakPtr<Unrelated> wptr_u = nullptr;

	// Formula:
	//xptr_b = xptr_b;
	//xptr_b = xptr_d;
	//xptr_b = xptr_u; // SHOULD FAIL
	//xptr_d = xptr_b; // SHOULD FAIL
	//xptr_d = xptr_d;
	//xptr_d = xptr_u; // SHOULD FAIL
	//xptr_u = xptr_b; // SHOULD FAIL
	//xptr_u = xptr_d; // SHOULD FAIL
	//xptr_u = xptr_u;

	// Unique->Shared
	uptr_b = rftl::move( uptr_b );
	uptr_b = rftl::move( uptr_d );
	//uptr_b = rftl::move(uptr_u); // SHOULD FAIL
	//uptr_d = rftl::move(uptr_b); // SHOULD FAIL
	uptr_d = rftl::move( uptr_d );
	//uptr_d = rftl::move(uptr_u); // SHOULD FAIL
	//uptr_u = rftl::move(uptr_b); // SHOULD FAIL
	//uptr_u = rftl::move(uptr_d); // SHOULD FAIL
	uptr_u = rftl::move( uptr_u );

	// Unique->Weak:
	wptr_b = uptr_b;
	wptr_b = uptr_d;
	//wptr_b = uptr_u; // SHOULD FAIL
	//wptr_d = uptr_b; // SHOULD FAIL
	wptr_d = uptr_d;
	//wptr_d = uptr_u; // SHOULD FAIL
	//wptr_u = uptr_b; // SHOULD FAIL
	//wptr_u = uptr_d; // SHOULD FAIL
	wptr_u = uptr_u;

	uptr_b = DefaultCreator<Base>::Create();
	uptr_b = DefaultCreator<Derived>::Create();
	//uptr_b = DefaultCreator<Unrelated>::Create(); // SHOULD FAIL
	//uptr_d = DefaultCreator<Base>::Create(); // SHOULD FAIL
	uptr_d = DefaultCreator<Derived>::Create();
	//uptr_d = DefaultCreator<Unrelated>::Create(); // SHOULD FAIL
	//uptr_u = DefaultCreator<Base>::Create(); // SHOULD FAIL
	//uptr_u = DefaultCreator<Derived>::Create(); // SHOULD FAIL
	uptr_u = DefaultCreator<Unrelated>::Create();
}



TEST( UniquePtr, CastToConst )
{
	UniquePtr<int> uptr = nullptr;
	UniquePtr<int const> uptr_c = rftl::move( uptr );
	WeakPtr<int const> wptr_c = uptr;
}



TEST( UniquePtr, CastToVoid )
{
	UniquePtr<int> uptr = nullptr;
	UniquePtr<int const> uptr_c = nullptr;

	UniquePtr<void> uptr_v;
	UniquePtr<void const> uptr_vc;
	WeakPtr<void> wptr_v;
	WeakPtr<void const> wptr_vc;

	uptr_v = rftl::move( uptr );
	//uptr_v = rftl::move( uptr_c ); // SHOULD FAIL
	uptr_vc = rftl::move( uptr );
	uptr_vc = rftl::move( uptr_c );
	wptr_v = uptr;
	//wptr_v = uptr_c; // SHOULD FAIL
	wptr_vc = uptr;
	wptr_vc = uptr_c;
}

///////////////////////////////////////////////////////////////////////////////
}

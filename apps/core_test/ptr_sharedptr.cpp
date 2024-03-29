#include "stdafx.h"

#include "core/ptr/shared_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/cstdlib"


RF_CLANG_IGNORE( "-Wself-assign-overloaded" );

namespace RF {
///////////////////////////////////////////////////////////////////////////////

namespace {

struct HugeStruct
{
	uint64_t member[100];
};

}
static_assert(
	sizeof( SharedPtr<HugeStruct> ) < sizeof( HugeStruct ),
	"SharedPtr is embedding storage" );



TEST( SharedPtr, Defaults )
{
	{
		SharedPtr<int> sptr;
		ASSERT_TRUE( sptr == nullptr );
	}
	{
		SharedPtr<int> sptr = SharedPtr<int>();
		ASSERT_TRUE( sptr == nullptr );
	}
	{
		SharedPtr<int> sptr = nullptr;
		ASSERT_TRUE( sptr == nullptr );
	}
}



TEST( SharedPtr, SoloLifecycle )
{
	// Normal
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( sptr != nullptr );
		ASSERT_TRUE( *sptr == 47 );
		*sptr = 53;
		ASSERT_TRUE( *sptr == 53 );
	}

	// Deferred create
	{
		auto creation = DefaultCreator<int>::Create( 47 );
		{
			SharedPtr<int> sptr = rftl::move( creation );
			ASSERT_TRUE( sptr != nullptr );
			ASSERT_TRUE( *sptr == 47 );
			*sptr = 53;
			ASSERT_TRUE( *sptr == 53 );
		}
	}

	// Pre-mature termination
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( sptr != nullptr );
		sptr = nullptr;
		ASSERT_TRUE( sptr == nullptr );
	}
}



TEST( SharedPtr, WeakLifecycle )
{
	// Weak outlives
	{
		WeakPtr<int> wptr;
		ASSERT_TRUE( wptr == nullptr );
		{
			SharedPtr<int> sptr = DefaultCreator<int>::Create( 47 );
			wptr = sptr;
			ASSERT_TRUE( wptr != nullptr );
		}
		ASSERT_TRUE( wptr == nullptr );
	}

	// Shared outlives
	{
		SharedPtr<int> sptr = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( sptr != nullptr );
		{
			WeakPtr<int> wptr = sptr;
			ASSERT_TRUE( wptr != nullptr );
		}
		ASSERT_TRUE( sptr != nullptr );
	}

	// Moves to an already assigned pointer should be destructive
	{
		SharedPtr<int> sptr1 = DefaultCreator<int>::Create( 83 );
		ASSERT_TRUE( sptr1 != nullptr );
		SharedPtr<int> sptr2 = DefaultCreator<int>::Create( 47 );
		ASSERT_TRUE( sptr2 != nullptr );
		{
			WeakPtr<int> wptr1 = sptr1;
			ASSERT_TRUE( wptr1 != nullptr );
			ASSERT_TRUE( *wptr1 == 83 );
			WeakPtr<int> wptr2 = sptr2;
			ASSERT_TRUE( wptr2 != nullptr );
			ASSERT_TRUE( *wptr2 == 47 );
			wptr1 = rftl::move( wptr2 );
			ASSERT_TRUE( wptr1 != nullptr );
			ASSERT_TRUE( *wptr1 == 47 );
			ASSERT_TRUE( wptr2 == nullptr );
		}
		ASSERT_TRUE( sptr1 != nullptr );
		ASSERT_TRUE( sptr2 != nullptr );
	}
}



TEST( SharedPtr, Move )
{
	// Move to longer-lived shared
	{
		SharedPtr<int> sptr2;
		ASSERT_TRUE( sptr2 == nullptr );
		{
			SharedPtr<int> sptr1 = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( sptr1 != nullptr );
			ASSERT_TRUE( *sptr1 == 47 );
			sptr2 = rftl::move( sptr1 );
			ASSERT_TRUE( sptr1 == nullptr );
			ASSERT_TRUE( sptr2 != nullptr );
			ASSERT_TRUE( *sptr2 == 47 );
		}
		ASSERT_TRUE( sptr2 != nullptr );
		ASSERT_TRUE( *sptr2 == 47 );
	}

	// Moves to an already assigned pointer should be destructive
	{
		SharedPtr<int> sptr2 = DefaultCreator<int>::Create( 83 );
		ASSERT_TRUE( sptr2 != nullptr );
		ASSERT_TRUE( *sptr2 == 83 );
		{
			SharedPtr<int> sptr1 = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( sptr1 != nullptr );
			ASSERT_TRUE( *sptr1 == 47 );
			sptr2 = rftl::move( sptr1 );
			ASSERT_TRUE( sptr1 == nullptr );
			ASSERT_TRUE( sptr2 != nullptr );
			ASSERT_TRUE( *sptr2 == 47 );
		}
		ASSERT_TRUE( sptr2 != nullptr );
		ASSERT_TRUE( *sptr2 == 47 );
	}
}



TEST( SharedPtr, MoveDerived )
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
		SharedPtr<Derived> sptr1 = DefaultCreator<Derived>::Create();
		ASSERT_TRUE( sptr1 != nullptr );
		SharedPtr<Base> sptr2( rftl::move( sptr1 ) );
		ASSERT_TRUE( sptr1 == nullptr );
		ASSERT_TRUE( sptr2 != nullptr );
	}
}



TEST( SharedPtr, MoveIntoTrash )
{
	SharedPtr<int> sptr1 = DefaultCreator<int>::Create( 5 );
	{
		void* alloc = ::operator new( sizeof( SharedPtr<int> ) );
		ASSERT_NE( alloc, nullptr );
		if( alloc == nullptr )
		{
			rftl::abort();
		}
		memset( alloc, 0xcc, sizeof( SharedPtr<int> ) );
		SharedPtr<int>* newAlloc = new( alloc ) SharedPtr<int>( rftl::move( sptr1 ) );
		newAlloc->~SharedPtr();
		::operator delete( alloc );
	}
}



TEST( SharedPtr, Copy )
{
	// Copy to longer-lived shared
	{
		SharedPtr<int> sptr2;
		ASSERT_TRUE( sptr2 == nullptr );
		{
			SharedPtr<int> sptr1 = DefaultCreator<int>::Create( 47 );
			ASSERT_TRUE( sptr1 != nullptr );
			ASSERT_TRUE( *sptr1 == 47 );
			sptr2 = sptr1;
			ASSERT_TRUE( sptr1 != nullptr );
			ASSERT_TRUE( *sptr1 == 47 );
			ASSERT_TRUE( sptr2 != nullptr );
			ASSERT_TRUE( *sptr2 == 47 );
		}
		ASSERT_TRUE( sptr2 != nullptr );
		ASSERT_TRUE( *sptr2 == 47 );
	}
}



TEST( SharedPtr, Arrow )
{
	struct Test
	{
		int test;
	};

	SharedPtr<Test> sptr = DefaultCreator<Test>::Create();
	WeakPtr<Test> wptr = sptr;
	( *sptr ).test = 9;
	ASSERT_TRUE( ( *sptr ).test == 9 );
	ASSERT_TRUE( ( *wptr ).test == 9 );
	ASSERT_TRUE( sptr->test == 9 );
	ASSERT_TRUE( wptr->test == 9 );
}



TEST( SharedPtr, CastToBase )
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

	SharedPtr<Derived> sptr_d = nullptr;
	SharedPtr<Base> sptr_b = nullptr;
	SharedPtr<Unrelated> sptr_u = nullptr;
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

	// Shared->Shared
	sptr_b = sptr_b;
	sptr_b = sptr_d;
	//sptr_b = sptr_u; // SHOULD FAIL
	//sptr_d = sptr_b; // SHOULD FAIL
	sptr_d = sptr_d;
	//sptr_d = sptr_u; // SHOULD FAIL
	//sptr_u = sptr_b; // SHOULD FAIL
	//sptr_u = sptr_d; // SHOULD FAIL
	sptr_u = sptr_u;

	// Weak->Weak
	wptr_b = wptr_b;
	wptr_b = wptr_d;
	//wptr_b = wptr_u; // SHOULD FAIL
	//wptr_d = wptr_b; // SHOULD FAIL
	wptr_d = wptr_d;
	//wptr_d = wptr_u; // SHOULD FAIL
	//wptr_u = wptr_b; // SHOULD FAIL
	//wptr_u = wptr_d; // SHOULD FAIL
	wptr_u = wptr_u;

	// Shared->Weak:
	wptr_b = sptr_b;
	wptr_b = sptr_d;
	//wptr_b = sptr_u; // SHOULD FAIL
	//wptr_d = sptr_b; // SHOULD FAIL
	wptr_d = sptr_d;
	//wptr_d = sptr_u; // SHOULD FAIL
	//wptr_u = sptr_b; // SHOULD FAIL
	//wptr_u = sptr_d; // SHOULD FAIL
	wptr_u = sptr_u;

	sptr_b = DefaultCreator<Base>::Create();
	sptr_b = DefaultCreator<Derived>::Create();
	//sptr_b = DefaultCreator<Unrelated>::Create(); // SHOULD FAIL
	//sptr_d = DefaultCreator<Base>::Create(); // SHOULD FAIL
	sptr_d = DefaultCreator<Derived>::Create();
	//sptr_d = DefaultCreator<Unrelated>::Create(); // SHOULD FAIL
	//sptr_u = DefaultCreator<Base>::Create(); // SHOULD FAIL
	//sptr_u = DefaultCreator<Derived>::Create(); // SHOULD FAIL
	sptr_u = DefaultCreator<Unrelated>::Create();
}



TEST( SharedPtr, CastToConst )
{
	SharedPtr<int> sptr = nullptr;
	SharedPtr<int const> sptr_c = sptr;
	WeakPtr<int const> wptr_c = sptr;
}



TEST( SharedPtr, CastToVoid )
{
	SharedPtr<int> sptr = nullptr;
	SharedPtr<int const> sptr_c = nullptr;

	SharedPtr<void> sptr_v;
	SharedPtr<void const> sptr_vc;
	WeakPtr<void> wptr_v;
	WeakPtr<void const> wptr_vc;

	sptr_v = sptr;
	//sptr_v = sptr_c; // SHOULD FAIL
	sptr_vc = sptr;
	sptr_vc = sptr_c;
	wptr_v = sptr;
	//wptr_v = sptr_c; // SHOULD FAIL
	wptr_vc = sptr;
	wptr_vc = sptr_c;
}

///////////////////////////////////////////////////////////////////////////////
}

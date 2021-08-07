#pragma once
#include "core/ptr/unique_ptr.h"
#include "core/ptr/shared_ptr.h"
#include "core/ptr/weak_ptr.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Potentially dangerous pointer transformations, that should be used very
//  carefully, since they could introduce ownership transfer failures that have
//  no non-terminal resolution
// NOTE: Yes, it's intentionally cumbersome to use these helper methods. If you
//  don't like it, then maybe you shouldn't be using them...
template<typename T>
struct PtrTransformer
{
	RF_NO_INSTANCE( PtrTransformer );

	// NOTE: Should be safe in all cases
	static void PerformTransformation( UniquePtr<T>&& in, SharedPtr<T>& out )
	{
		out = SharedPtr<T>{ in.CreateTransferPayloadAndWipeSelf() };
	}

	// NOTE: Very dangerous, likely to crater hard or corrupt memory if the
	//  shared pointer doesn't have unique access. This includes lurking
	//  shared-weak pointers, which could sneak in and steal the strong count.
	// NOTE: At time of writing, unique pointers will check on creation or
	//  destruction to make sure they're the only strong owners, and will hard
	//  fatal the entire application if they detect otherwise. This won't catch
	//  all mis-uses, but hopefully should prevent the most brazen stupidity.
	static void PerformUncheckedTransformation( SharedPtr<T>&& in, UniquePtr<T>& out )
	{
		out = UniquePtr<T>{ in.CreateTransferPayloadAndWipeSelf() };
	}

	// NOTE: Very limited use in safe cases, as it effectively just locks you
	//  out of being able to meaningfully interact with the allocation, unless
	//  you extract the pointer and cast it to what you think it is, at which
	//  point you're in the same problem-cases that wanton reintepret_cast
	//  normally puts you in
	// NOTE: Here for historical reference, but is now available as an implicit
	//  conversion on valid pointer types
	[[deprecated]]
	static void PerformVoidTransformation( UniquePtr<T>&& in, UniquePtr<void>& out )
	{
		CreationPayload<T> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<void> voidPayload(
			static_cast<decltype( CreationPayload<void>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<void>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = UniquePtr<void>{ rftl::move( voidPayload ) };
	}
	[[deprecated]]
	static void PerformVoidTransformation( WeakPtr<T>&& in, WeakPtr<void>& out )
	{
		CreationPayload<T> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<void> voidPayload(
			static_cast<decltype( CreationPayload<void>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<void>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = WeakPtr<void>{ voidPayload.mTarget, voidPayload.mRef };
		voidPayload.Clean();
	}

	// NOTE: This is basically a reinterpret_cast, which means you're
	//  completely on your own for type-safety
	static void PerformNonTypesafeTransformation( UniquePtr<void>&& in, UniquePtr<T>& out )
	{
		CreationPayload<void> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<T> voidPayload(
			static_cast<decltype( CreationPayload<T>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<T>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = UniquePtr<T>{ rftl::move( voidPayload ) };
	}
	static void PerformNonTypesafeTransformation( UniquePtr<void const>&& in, UniquePtr<T const>& out )
	{
		CreationPayload<void const> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<T const> voidPayload(
			static_cast<decltype( CreationPayload<T const>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<T const>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = UniquePtr<T const>{ rftl::move( voidPayload ) };
	}
	static void PerformNonTypesafeTransformation( WeakPtr<void>&& in, WeakPtr<T>& out )
	{
		CreationPayload<void> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<T> voidPayload(
			static_cast<decltype( CreationPayload<T>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<T>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = WeakPtr<T>{ voidPayload.mTarget, voidPayload.mRef };
		voidPayload.Clean();
	}
	static void PerformNonTypesafeTransformation( WeakPtr<void const>&& in, WeakPtr<T const>& out )
	{
		CreationPayload<void const> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<T const> voidPayload(
			static_cast<decltype( CreationPayload<T const>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<T const>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = WeakPtr<T const>{ voidPayload.mTarget, voidPayload.mRef };
		voidPayload.Clean();
	}
};

///////////////////////////////////////////////////////////////////////////////
}

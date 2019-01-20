#pragma once
#include "core/ptr/unique_ptr.h"
#include "core/ptr/shared_ptr.h"


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
	PtrTransformer() = delete;

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
	static void PerformVoidTransformation( UniquePtr<T>&& in, UniquePtr<void>& out )
	{
		CreationPayload<T> normalPayload = in.CreateTransferPayloadAndWipeSelf();
		CreationPayload<void> voidPayload(
			static_cast<decltype( CreationPayload<void>::mTarget )>( normalPayload.mTarget ),
			reinterpret_cast<decltype( CreationPayload<void>::mRef )>( normalPayload.mRef ) );
		normalPayload.Clean();
		out = UniquePtr<void>{ rftl::move( voidPayload ) };
	}
};

///////////////////////////////////////////////////////////////////////////////
}

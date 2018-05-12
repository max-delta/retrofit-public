#pragma once
#include "NonIncrementalTask.h"

#include "rftl/type_traits"


namespace RF { namespace scheduling {
///////////////////////////////////////////////////////////////////////////////

// Template helper to determine functor type, for use in cases such as lambdas,
//  where the type can't be expressed, and must rely on 'auto'
// EXAMPLE:
//  auto myLambda = [](){};
//  auto myFunctor = RF::scheduling::CreateFunctorTask( rftl::move( myLambda ) );
//  using FunctorType = decltype( myFunctor );
template<typename Functor>
FunctorTask<Functor> CreateFunctorTask( Functor && functor )
{
	return FunctorTask<Functor>( rftl::move( functor ) );
}


template<typename Functor>
class FunctorTask final : public NonIncrementalTask
{
	RF_NO_COPY( FunctorTask );

	static_assert(
		rftl::is_invocable<Functor>::value,
		"Functor must be callable as a void function" );

	//
	// Public methods
public:
	FunctorTask( Functor && functor )
		: mFunctor( rftl::move( functor ) )
	{
		//
	}

	FunctorTask( FunctorTask&& rhs )
		: mFunctor( rftl::move( rhs.mFunctor ) )
	{
		//
	}

	virtual void Run() override final
	{
		mFunctor();
	}


	//
	// Private data
private:
	Functor const mFunctor;
};

///////////////////////////////////////////////////////////////////////////////
}}

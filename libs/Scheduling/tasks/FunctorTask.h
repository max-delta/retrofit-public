#pragma once
#include "NonIncrementalTask.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"

#include "rftl/type_traits"


namespace RF::scheduling {
///////////////////////////////////////////////////////////////////////////////

// Template helper to determine functor type, for use in cases such as lambdas,
//  where the type can't be expressed, and must rely on 'auto'
// EXAMPLE:
//  auto myLambda = [](){};
//  auto myFunctor = RF::scheduling::CreateFunctorTask( rftl::move( myLambda ) );
//  using FunctorType = decltype( myFunctor );
template<typename Functor>
FunctorTask<typename rftl::decay<Functor>::type> CreateFunctorTask( Functor&& functor )
{
	return FunctorTask<typename rftl::decay<Functor>::type>( rftl::move( functor ) );
}

template<typename Functor>
CloneableFunctorTask<typename rftl::decay<Functor>::type> CreateCloneableFunctorTask( Functor&& functor )
{
	return CloneableFunctorTask<typename rftl::decay<Functor>::type>( rftl::move( functor ) );
}

///////////////////////////////////////////////////////////////////////////////

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
	FunctorTask( Functor&& functor )
		: mFunctor( rftl::move( functor ) )
	{
		//
	}

	FunctorTask( FunctorTask&& rhs )
		: mFunctor( rftl::move( rhs.mFunctor ) )
	{
		//
	}

	virtual TaskPtr Clone() const override final
	{
		return nullptr;
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

template<typename Functor>
class CloneableFunctorTask final : public NonIncrementalTask
{
	RF_NO_COPY( CloneableFunctorTask );

public:
	static_assert(
		rftl::is_invocable<Functor>::value,
		"Functor must be callable as a void function" );

	//
	// Public methods
public:
	CloneableFunctorTask( Functor&& functor )
		: mFunctor( rftl::move( functor ) )
	{
		//
	}

	CloneableFunctorTask( CloneableFunctorTask&& rhs )
		: mFunctor( rftl::move( rhs.mFunctor ) )
	{
		//
	}

	virtual TaskPtr Clone() const override final
	{
		Functor cloneFunc = mFunctor;
		CloneableFunctorTask clone = CreateCloneableFunctorTask( rftl::move( cloneFunc ) );
		return DefaultCreator<CloneableFunctorTask>::Create( rftl::move( clone ) );
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
}

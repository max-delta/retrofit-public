#pragma once
#include "core/macros.h"


namespace RF { namespace alloc {
///////////////////////////////////////////////////////////////////////////////

// Virtual interface for talking to allocators
class Allocator
{
	RF_NO_COPY( Allocator );

	//
	// Public methods
public:
	virtual ~Allocator() = default;

	virtual void* Allocate( size_t size ) = 0;
	virtual void* Allocate( size_t size, size_t align ) = 0;
	virtual void Delete( void* ptr ) = 0;

	virtual size_t GetMaxSize() const = 0;
	virtual size_t GetCurrentSize() const = 0;
	virtual size_t GetCurrentCount() const = 0;

	virtual void RelinquishAllAllocations() = 0;


	//
	// Protected methods
protected:
	Allocator() = default;
};

///////////////////////////////////////////////////////////////////////////////

// Template interface for exposing allocators via virtuals
template<typename A>
class AllocatorT final : public Allocator
{
	RF_NO_COPY( AllocatorT );

	//
	// Public methods
public:
	template<typename... Args>
	AllocatorT( Args... args )
		: mAllocator( args... )
	{
		//
	}

	virtual void* Allocate( size_t size ) override final
	{
		return mAllocator.Allocate( size );
	}

	virtual void* Allocate( size_t size, size_t align ) override final
	{
		return mAllocator.Allocate( size, align );
	}

	virtual void Delete( void* ptr ) override final
	{
		mAllocator.Delete( ptr );
	}

	virtual size_t GetMaxSize() const override final
	{
		return mAllocator.GetMaxSize();
	}

	virtual size_t GetCurrentSize() const override final
	{
		return mAllocator.GetCurrentSize();
	}

	virtual size_t GetCurrentCount() const override final
	{
		return mAllocator.GetCurrentCount();
	}

	virtual void RelinquishAllAllocations() override final
	{
		mAllocator.RelinquishAllAllocations();
	}


	//
	// Private data
private:
	A mAllocator;
};

///////////////////////////////////////////////////////////////////////////////
}}

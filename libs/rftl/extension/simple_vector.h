#pragma once
#include "core/compiler.h"
#include "core/macros.h"

#include "rftl/algorithm"
#include "rftl/memory"
#include "rftl/span"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Vector class where the element data cannot resize quickly, allowing the heap
//  allocation to have the capacity always match the size, and thus to have
//  less member storage in the class itself
template<typename ElementT>
class simple_vector
{
	RF_NO_ASSIGN_VIA_COPY( simple_vector );

	//
	// Types
public:
	using view_type = span<ElementT>;
	using const_view_type = span<ElementT const>;
private:
	using backing_type = scoped_ptr<ElementT[]>;


	//
	// Public methods
public:
	simple_vector() = default;
	simple_vector( const_view_type value )
		: mStorage( create( value ) )
		, mSize( value.size() )
	{
		//
	}
	simple_vector( ElementT const* value, size_t count )
		: simple_vector( const_view_type( value, count ) )
	{
		//
	}
	simple_vector( simple_vector const& value )
		: simple_vector( const_view_type( value ) )
	{
		//
	}

	simple_vector& operator=( simple_vector&& rhs )
	{
		RF_MOVE_ASSIGN_CLEAR( mStorage );
		RF_MOVE_ASSIGN_CLEAR( mSize );
		return *this;
	}

	operator view_type()
	{
		return view();
	}

	operator const_view_type() const
	{
		return view();
	}

	view_type view()
	{
		return view_type( mStorage.get(), mSize );
	}

	const_view_type view() const
	{
		return const_view_type( mStorage.get(), mSize );
	}

	ElementT& at( size_t index )
	{
		return rftl::span_at( view(), index );
	}

	ElementT const& at( size_t index ) const
	{
		return rftl::span_at( view(), index );
	}

	ElementT* data()
	{
		return mStorage.get();
	}

	ElementT const* data() const
	{
		return mStorage.get();
	}

	size_t size() const
	{
		return mSize;
	}

	bool empty() const
	{
		return mSize == 0;
	}

	void clear()
	{
		*this = {};
	}

	void resize( size_t size )
	{
		// Keep old around briefly
		backing_type const oldStorage = rftl::move( mStorage );
		size_t const oldSize = mSize;

		// Make new
		mStorage = create_default( size );
		mSize = size;

		// Transfer shared
		size_t const sharedSize = rftl::min( oldSize, mSize );
		for( size_t i = 0; i < sharedSize; i++ )
		{
			mStorage[i] = oldStorage[i];
		}
	}


	//
	// Private methods
private:
	static backing_type create( const_view_type value )
	{
		size_t const size = value.size();
		backing_type ptr = create_default( size );
		for( size_t i = 0; i < size; i++ )
		{
			ptr[i] = rftl::span_at( value, i );
		}
		return ptr;
	}

	static backing_type create_default( size_t size )
	{
		return rftl::make_scoped<ElementT[]>( size );
	}


	//
	// Private data
private:
	backing_type mStorage = {};
	size_t mSize = 0;
};

///////////////////////////////////////////////////////////////////////////////
}

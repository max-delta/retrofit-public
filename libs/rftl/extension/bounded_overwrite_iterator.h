#pragma once
#include "rftl/iterator"
#include "rftl/cstddef"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename IterT>
struct bounded_forward_overwrite_iterator
{
	using value_type = typename rftl::iterator_traits<IterT>::value_type;
	using difference_type = rftl::ptrdiff_t;

	bounded_forward_overwrite_iterator() = default;
	bounded_forward_overwrite_iterator( IterT const& begin, IterT const& end )
		: mEnd( end ), mIter( begin )
	{
		//
	}
	template<typename ContainerT>
	explicit bounded_forward_overwrite_iterator( ContainerT& container )
		: bounded_forward_overwrite_iterator( container.begin(), container.end() )
	{
		//
	}

	bounded_forward_overwrite_iterator& operator=( value_type const& element )
	{
		if( mIter != mEnd )
		{
			*mIter = element;
			mIter++;
		}
		return *this;
	}

	// No-ops, needed to satisfy interator contraints
	bounded_forward_overwrite_iterator& operator*()
	{
		return *this;
	}
	bounded_forward_overwrite_iterator& operator++()
	{
		return *this;
	}
	bounded_forward_overwrite_iterator operator++( int )
	{
		return *this;
	}

private:
	IterT mEnd;
	IterT mIter;
};

///////////////////////////////////////////////////////////////////////////////

// Deduction helper
// EXAMPLE:
//	rftl::array<int, 5> buf;
//  rftl::bounded_forward_overwrite_iterator iter( buf );
template<typename ContainerT>
bounded_forward_overwrite_iterator( ContainerT& c ) -> bounded_forward_overwrite_iterator<typename ContainerT::iterator>;

///////////////////////////////////////////////////////////////////////////////
}

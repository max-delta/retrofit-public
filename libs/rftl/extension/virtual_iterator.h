#pragma once
#include "rftl/iterator"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename ElementT>
struct virtual_iterator
{
	virtual ~virtual_iterator() = default;
	virtual void operator()( ElementT const& element ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

template<typename ElementT, typename ContainerT>
struct virtual_back_inserter_iterator : public virtual_iterator<ElementT>
{
	virtual_back_inserter_iterator( ContainerT& containerRef )
		: mInserter( containerRef )
	{
		//
	}

	virtual void operator()( ElementT const& element ) override
	{
		mInserter = element;
	}

	rftl::back_insert_iterator<ContainerT> mInserter;
};

///////////////////////////////////////////////////////////////////////////////
}

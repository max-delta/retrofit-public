#pragma once
#include "rftl/iterator"


namespace std {
///////////////////////////////////////////////////////////////////////////////

template<>
struct iterator_traits<void*>
{
	using iterator_category = random_access_iterator_tag;
	using value_type = void;
	using difference_type = ptrdiff_t;
	using pointer = void*;
	using reference = void;
};



template<>
struct iterator_traits<void const*>
{
	using iterator_category = random_access_iterator_tag;
	using value_type = void;
	using difference_type = ptrdiff_t;
	using pointer = void const*;
	using reference = void;
};

///////////////////////////////////////////////////////////////////////////////
}

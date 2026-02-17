#pragma once
#include "stretch_vector.h"

#include "core/macros.h"

#include "rftl/limits"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector() = default;



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( size_type count )
{
	grow( count );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( size_type count, value_type const& value )
	: stretch_vector()
{
	grow( count, value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( InputIterator first, InputIterator term )
	: stretch_vector()
{
	append( first, term );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( stretch_vector const& other )
	: stretch_vector()
{
	operator=( other );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( stretch_vector<value_type, OtherCapacity> const& other )
	: stretch_vector()
{
	operator=( other );
}




template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( stretch_vector&& other )
	: stretch_vector()
{
	operator=( std::move( other ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( stretch_vector<value_type, OtherCapacity>&& other )
	: stretch_vector()
{
	operator=( std::move( other ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( rftl::initializer_list<value_type> init )
	: stretch_vector()
{
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<typename Convertible>
inline stretch_vector<Element, ElementCapacity, Alignment>::stretch_vector( rftl::initializer_list<Convertible> init )
	: stretch_vector()
{
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::~stretch_vector()
{
	clear();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
stretch_vector<Element, ElementCapacity, Alignment>&
stretch_vector<Element, ElementCapacity, Alignment>::operator=( stretch_vector const& other )
{
	assign( other.begin(), other.end() );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
stretch_vector<Element, ElementCapacity, Alignment>&
stretch_vector<Element, ElementCapacity, Alignment>::operator=( stretch_vector&& other )
{
	clear();
	extract( other.begin(), other.end() );
	other.clear();
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
stretch_vector<Element, ElementCapacity, Alignment>&
stretch_vector<Element, ElementCapacity, Alignment>::operator=( stretch_vector<value_type, OtherCapacity> const& other )
{
	assign( other.begin(), other.end() );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
stretch_vector<Element, ElementCapacity, Alignment>&
stretch_vector<Element, ElementCapacity, Alignment>::operator=( stretch_vector<value_type, OtherCapacity>&& other )
{
	clear();
	extract( other.begin(), other.end() );
	other.clear();
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
stretch_vector<Element, ElementCapacity, Alignment>&
stretch_vector<Element, ElementCapacity, Alignment>::operator=( rftl::initializer_list<value_type> init )
{
	assign( init );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::assign( size_type count, value_type const& value )
{
	clear();
	grow( count, value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline void stretch_vector<Element, ElementCapacity, Alignment>::assign( InputIterator first, InputIterator term )
{
	clear();
	append( first, term );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::assign( rftl::initializer_list<value_type> init )
{
	clear();
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::at( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reference stretch_vector<Element, ElementCapacity, Alignment>::at( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::operator[]( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	if( mStretch.has_value() )
	{
		return ( *mStretch )[pos];
	}
	return mFixed[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reference stretch_vector<Element, ElementCapacity, Alignment>::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	if( mStretch.has_value() )
	{
		return ( *mStretch )[pos];
	}
	return mFixed[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::front()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reference stretch_vector<Element, ElementCapacity, Alignment>::front() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reference stretch_vector<Element, ElementCapacity, Alignment>::back() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::value_type* stretch_vector<Element, ElementCapacity, Alignment>::data()
{
	if( mStretch.has_value() )
	{
		return mStretch->data();
	}
	return mFixed.data();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::value_type const* stretch_vector<Element, ElementCapacity, Alignment>::data() const
{
	if( mStretch.has_value() )
	{
		return mStretch->data();
	}
	return mFixed.data();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::begin()
{
	if( mStretch.has_value() )
	{
		return make_iter( *mStretch->begin() );
	}
	return make_iter( *mFixed.begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_iterator stretch_vector<Element, ElementCapacity, Alignment>::begin() const
{
	if( mStretch.has_value() )
	{
		return make_const_iter( *mStretch->begin() );
	}
	return make_const_iter( *mFixed.begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_iterator stretch_vector<Element, ElementCapacity, Alignment>::cbegin() const
{
	return begin();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::end()
{
	if( mStretch.has_value() )
	{
		// Would do '*mStretch->end()', but that would cause checked iterator
		//  debugging in MSVC to call us out on the bad dereference, even
		//  though the generated assembly wouldn't ACTUALLY perform the
		//  dereference from that
		return make_iter( *( mStretch->data() + mStretch->size() ) );
	}
	return make_iter( *mFixed.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_iterator stretch_vector<Element, ElementCapacity, Alignment>::end() const
{
	if( mStretch.has_value() )
	{
		// Would do '*mStretch->end()', see comment on non-const end() version
		//  in this class for details
		return make_const_iter( *( mStretch->data() + mStretch->size() ) );
	}
	return make_const_iter( *mFixed.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_iterator stretch_vector<Element, ElementCapacity, Alignment>::cend() const
{
	return end();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::rbegin()
{
	return reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::rbegin() const
{
	return const_reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::crbegin() const
{
	return const_reverse_iterator( cend() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::rend()
{
	return reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::rend() const
{
	return const_reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator stretch_vector<Element, ElementCapacity, Alignment>::crend() const
{
	return const_reverse_iterator( cbegin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline bool stretch_vector<Element, ElementCapacity, Alignment>::empty() const
{
	return size() == 0;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::size_type stretch_vector<Element, ElementCapacity, Alignment>::size() const
{
	if( mStretch.has_value() )
	{
		return mStretch->size();
	}
	return mFixed.size();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::size_type stretch_vector<Element, ElementCapacity, Alignment>::max_size() const
{
	return rftl::numeric_limits<size_t>::max();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::reserve( size_type newCap ) const
{
	RF_ASSERT_MSG( newCap <= max_size(), "Cannot grow beyond max size" );
	return;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::size_type stretch_vector<Element, ElementCapacity, Alignment>::capacity() const
{
	if( mStretch.has_value() )
	{
		return mStretch->capacity();
	}
	return mFixed.capacity();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::shrink_to_fit()
{
	// No...
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::clear()
{
	while( empty() == false )
	{
		pop_back();
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::push_back( value_type const& value )
{
	emplace_back( value_type( value ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::push_back( value_type&& value )
{
	emplace_back( rftl::move( value ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::emplace_back( value_type&& value )
{
	if( mStretch.has_value() )
	{
		return mStretch->emplace_back( rftl::move( value ) );
	}

	if( mFixed.size() + 1 > mFixed.max_size() )
	{
		stretch();
		return mStretch->emplace_back( rftl::move( value ) );
	}

	return mFixed.emplace_back( rftl::move( value ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class... Args>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::reference stretch_vector<Element, ElementCapacity, Alignment>::emplace_back( Args&&... args )
{
	if( mStretch.has_value() )
	{
		return mStretch->emplace_back( args... );
	}

	if( mFixed.size() + 1 > mFixed.max_size() )
	{
		stretch();
		return mStretch->emplace_back( args... );
	}

	return mFixed.emplace_back( args... );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::pop_back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting pop_back on empty container" );
	if( mStretch.has_value() )
	{
		return mStretch->pop_back();
	}
	return mFixed.pop_back();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::insert( const_iterator pos, const value_type& value )
{
	value_type copy = value;
	return insert( pos, rftl::move( copy ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::insert( const_iterator pos, value_type&& value )
{
	ptrdiff_t const distance = pos - cbegin();

	if( mStretch.has_value() )
	{
		return make_iter( *mStretch->insert( mStretch->begin() + distance, rftl::move( value ) ) );
	}

	if( mFixed.size() + 1 > mFixed.max_size() )
	{
		stretch();
		return make_iter( *mStretch->insert( mStretch->begin() + distance, rftl::move( value ) ) );
	}

	return make_iter( *mFixed.insert( mFixed.begin() + distance, rftl::move( value ) ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::erase( const_iterator pos )
{
	ptrdiff_t const distance = pos - begin();

	if( mStretch.has_value() )
	{
		return make_iter( *mStretch->erase( mStretch->begin() + distance ) );
	}

	return make_iter( *mFixed.erase( mFixed.begin() + distance ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::erase( const_iterator first, const_iterator last )
{
	RF_ASSERT( first >= begin() );
	RF_ASSERT( last <= end() );
	RF_ASSERT( first <= last );
	ptrdiff_t const distance = first - cbegin();
	difference_type const numToErase = last - first;
	for( difference_type i = 0; i < numToErase; i++ )
	{
		erase( first );
	}
	return begin() + distance;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::resize( size_type count )
{
	while( size() > count )
	{
		pop_back();
	}
	while( size() < count )
	{
		emplace_back();
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::resize( size_type count, value_type const& value )
{
	while( size() > count )
	{
		pop_back();
	}
	while( size() < count )
	{
		emplace_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline void stretch_vector<Element, ElementCapacity, Alignment>::append( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		emplace_back( rftl::move( value_type( *iter ) ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::append( rftl::initializer_list<value_type> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<typename Convertible>
inline void stretch_vector<Element, ElementCapacity, Alignment>::append( rftl::initializer_list<Convertible> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline void stretch_vector<Element, ElementCapacity, Alignment>::extract( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		emplace_back( rftl::move( *iter ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::grow( size_type growthAmount )
{
	if( mStretch.has_value() )
	{
		mStretch->resize( mStretch->size() + growthAmount );
		return;
	}

	size_t const new_size = mFixed.size() + growthAmount;
	if( new_size > mFixed.max_size() )
	{
		stretch();
		mStretch->resize( new_size );
		return;
	}

	mFixed.resize( new_size );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::grow( size_type growthAmount, value_type const& value )
{
	if( mStretch.has_value() )
	{
		mStretch->resize( mStretch->size() + growthAmount, value );
		return;
	}

	size_t const new_size = mFixed.size() + growthAmount;
	if( new_size > mFixed.max_size() )
	{
		stretch();
		mStretch->resize( new_size, value );
		return;
	}

	mFixed.resize( new_size, value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::stretch()
{
	RF_ASSERT( mStretch.has_value() == false );
	mStretch.emplace();
	mStretch->reserve( mFixed.size() );
	for( value_type& val : mFixed )
	{
		mStretch->emplace_back( rftl::move( val ) );
	}
	mFixed.clear();

	invalidate();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::iterator stretch_vector<Element, ElementCapacity, Alignment>::make_iter( value_type& value ) const
{
#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	return iterator( &value, mVersion );
#else
	return &value;
#endif
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline stretch_vector<Element, ElementCapacity, Alignment>::const_iterator stretch_vector<Element, ElementCapacity, Alignment>::make_const_iter( value_type const& value ) const
{
#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	return const_iterator( &value, mVersion );
#else
	return &value;
#endif
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void stretch_vector<Element, ElementCapacity, Alignment>::invalidate()
{
#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	mVersion.invalidate();
#endif
}

///////////////////////////////////////////////////////////////////////////////
}

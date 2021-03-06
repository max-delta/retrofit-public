#pragma once
#include "static_string.h"

#include "core/macros.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string()
{
	//
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( size_type count, value_type const& value )
	: static_basic_string()
{
	grow( count, value );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( InputIterator first, InputIterator term )
	: static_basic_string()
{
	append( first, term );
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( static_basic_string const& other )
	: static_basic_string( other.begin(), other.end() )
{
	//
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( static_basic_string<value_type, OtherCapacity> const& other )
	: static_basic_string( other.begin(), other.end() )
{
	//
}




template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( static_basic_string&& other )
	: static_basic_string()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( static_basic_string<value_type, OtherCapacity>&& other )
	: static_basic_string()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( rftl::initializer_list<value_type> init )
	: static_basic_string()
{
	append( init );
}



template<typename Element, size_t ElementCapacity>
template<typename Convertible>
inline static_basic_string<Element, ElementCapacity>::static_basic_string( rftl::initializer_list<Convertible> init )
	: static_basic_string()
{
	append( init );
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>::~static_basic_string()
{
	clear();
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>& static_basic_string<Element, ElementCapacity>::operator=( static_basic_string const& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
	}
	return *this;
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>& static_basic_string<Element, ElementCapacity>::operator=( static_basic_string const&& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
		other.clear();
	}
	return *this;
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_basic_string<Element, ElementCapacity>& static_basic_string<Element, ElementCapacity>::operator=( static_basic_string<value_type, OtherCapacity> const& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
	}
	return *this;
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_basic_string<Element, ElementCapacity>& static_basic_string<Element, ElementCapacity>::operator=( static_basic_string<value_type, OtherCapacity>&& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
		other.clear();
	}
	return *this;
}



template<typename Element, size_t ElementCapacity>
inline static_basic_string<Element, ElementCapacity>& rftl::static_basic_string<Element, ElementCapacity>::operator=( rftl::initializer_list<value_type> init )
{
	assign( init );
	return *this;
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::assign( size_type count, value_type const& value )
{
	clear();
	grow( count, value );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_basic_string<Element, ElementCapacity>::assign( InputIterator first, InputIterator term )
{
	clear();
	append( first, term );
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::assign( rftl::initializer_list<value_type> init )
{
	clear();
	append( init );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reference static_basic_string<Element, ElementCapacity>::at( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reference static_basic_string<Element, ElementCapacity>::at( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reference static_basic_string<Element, ElementCapacity>::operator[]( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reference static_basic_string<Element, ElementCapacity>::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reference static_basic_string<Element, ElementCapacity>::front()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reference static_basic_string<Element, ElementCapacity>::front() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reference static_basic_string<Element, ElementCapacity>::back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reference static_basic_string<Element, ElementCapacity>::back() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::value_type* static_basic_string<Element, ElementCapacity>::data()
{
	return &( m_Storage[0] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::value_type const* static_basic_string<Element, ElementCapacity>::data() const
{
	return &( m_Storage[0] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::value_type const* static_basic_string<Element, ElementCapacity>::c_str() const
{
	return data();
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::iterator static_basic_string<Element, ElementCapacity>::begin()
{
	return &( m_Storage[0] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_iterator static_basic_string<Element, ElementCapacity>::begin() const
{
	return &( m_Storage[0] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_iterator static_basic_string<Element, ElementCapacity>::cbegin() const
{
	return begin();
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::iterator static_basic_string<Element, ElementCapacity>::end()
{
	return &( m_Storage[size()] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_iterator static_basic_string<Element, ElementCapacity>::end() const
{
	return &( m_Storage[size()] );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_iterator static_basic_string<Element, ElementCapacity>::cend() const
{
	return end();
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reverse_iterator static_basic_string<Element, ElementCapacity>::rbegin()
{
	return reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reverse_iterator static_basic_string<Element, ElementCapacity>::rbegin() const
{
	return const_reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reverse_iterator static_basic_string<Element, ElementCapacity>::crbegin() const
{
	return const_reverse_iterator( cend() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::reverse_iterator static_basic_string<Element, ElementCapacity>::rend()
{
	return reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reverse_iterator static_basic_string<Element, ElementCapacity>::rend() const
{
	return const_reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::const_reverse_iterator static_basic_string<Element, ElementCapacity>::crend() const
{
	return const_reverse_iterator( cbegin() );
}



template<typename Element, size_t ElementCapacity>
inline bool static_basic_string<Element, ElementCapacity>::empty() const
{
	return size() == 0;
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::size_type static_basic_string<Element, ElementCapacity>::size() const
{
	for( size_t i = 0; i < capacity(); i++ )
	{
		if( m_Storage[i] == Element{} )
		{
			return i;
		}
	}
	return capacity();
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::size_type static_basic_string<Element, ElementCapacity>::length() const
{
	return size();
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::size_type static_basic_string<Element, ElementCapacity>::max_size() const
{
	return fixed_capacity;
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::reserve( size_type newCap ) const
{
	RF_ASSERT_MSG( newCap <= max_size(), "Cannot grow beyond max size" );
	return;
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::size_type static_basic_string<Element, ElementCapacity>::capacity() const
{
	return max_size();
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::shrink_to_fit()
{
	// No...
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::clear()
{
	while( empty() == false )
	{
		pop_back();
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::push_back( value_type const& value )
{
	value_type copy = value;
	push_back( rftl::move( copy ) );
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::push_back( value_type&& value )
{
	RF_ASSERT_MSG( value != Element{}, "Null characters not supported" );
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	m_Storage[size()] = rftl::move( value );
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::pop_back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting pop_back on empty container" );
	RF_ASSERT( size() > 0 );
	m_Storage[size() - 1] = {};
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::iterator static_basic_string<Element, ElementCapacity>::insert( const_iterator pos, const value_type& value )
{
	value_type copy = value;
	return insert( rftl::move( copy ) );
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::iterator static_basic_string<Element, ElementCapacity>::insert( const_iterator pos, value_type&& value )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( size() < fixed_capacity );
	for( iterator iter = end(); iter > pos; iter-- )
	{
		iterator const dest = iter;
		iterator const source = iter - 1;
		*dest = rftl::move( *source );
		*source = Element{};
	}
	iterator const mutablePos = const_cast<iterator>( pos );
	*mutablePos = rftl::move( value );
	return mutablePos;
}



template<typename Element, size_t ElementCapacity>
inline typename static_basic_string<Element, ElementCapacity>::iterator static_basic_string<Element, ElementCapacity>::erase( const_iterator pos )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( size() > 0 );
	iterator const mutablePos = const_cast<iterator>( pos );
	for( iterator iter = mutablePos; iter <= end() - 1; iter++ )
	{
		iterator dest = iter;
		iterator source = iter + 1;
		*dest = rftl::move( *source );
		*source = Element{};
	}
	return mutablePos;
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::resize( size_type count, value_type const& value )
{
	while( size() > count )
	{
		pop_back();
	}
	while( size() < count )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_basic_string<Element, ElementCapacity>::append( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		push_back( rftl::move( *iter ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::append( rftl::initializer_list<value_type> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity>
template<typename Convertible>
inline void static_basic_string<Element, ElementCapacity>::append( rftl::initializer_list<Convertible> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_basic_string<Element, ElementCapacity>::extract( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		push_back( rftl::move( *iter ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_basic_string<Element, ElementCapacity>::grow( size_type growthAmount, value_type const& value )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////
}

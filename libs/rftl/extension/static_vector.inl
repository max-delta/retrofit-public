#pragma once
#include "static_vector.h"

#include "core/macros.h"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity, size_t Alignment>
inline Element& static_vector<Element, ElementCapacity, Alignment>::Storage::Value()
{
	return *( reinterpret_cast<Element*>( mData ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline Element const& static_vector<Element, ElementCapacity, Alignment>::Storage::Value() const
{
	return *( reinterpret_cast<Element const*>( mData ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline Element* static_vector<Element, ElementCapacity, Alignment>::Storage::WriteableTarget()
{
	return reinterpret_cast<Element*>( mData );
}

///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector()
	: m_CurrentSize( 0 )
{
	//
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( size_type count )
	: static_vector()
{
	grow( count );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( size_type count, value_type const& value )
	: static_vector()
{
	grow( count, value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( InputIterator first, InputIterator term )
	: static_vector()
{
	append( first, term );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( static_vector const& other )
	: static_vector()
{
	operator=( other );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( static_vector<value_type, OtherCapacity> const& other )
	: static_vector()
{
	operator=( other );
}




template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( static_vector&& other )
	: static_vector()
{
	operator=( std::move( other ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( static_vector<value_type, OtherCapacity>&& other )
	: static_vector()
{
	operator=( std::move( other ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( rftl::initializer_list<value_type> init )
	: static_vector()
{
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<typename Convertible>
inline static_vector<Element, ElementCapacity, Alignment>::static_vector( rftl::initializer_list<Convertible> init )
	: static_vector()
{
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline static_vector<Element, ElementCapacity, Alignment>::~static_vector()
{
	clear();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
static_vector<Element, ElementCapacity, Alignment>&
static_vector<Element, ElementCapacity, Alignment>::operator=( static_vector const& other )
{
	append( other.begin(), other.end() );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
static_vector<Element, ElementCapacity, Alignment>&
static_vector<Element, ElementCapacity, Alignment>::operator=( static_vector&& other )
{
	extract( other.begin(), other.end() );
	other.clear();
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
static_vector<Element, ElementCapacity, Alignment>&
static_vector<Element, ElementCapacity, Alignment>::operator=( static_vector<value_type, OtherCapacity> const& other )
{
	append( other.begin(), other.end() );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<size_t OtherCapacity>
static_vector<Element, ElementCapacity, Alignment>&
static_vector<Element, ElementCapacity, Alignment>::operator=( static_vector<value_type, OtherCapacity>&& other )
{
	extract( other.begin(), other.end() );
	other.clear();
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
static_vector<Element, ElementCapacity, Alignment>&
static_vector<Element, ElementCapacity, Alignment>::operator=( rftl::initializer_list<value_type> init )
{
	append( init );
	return *this;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::assign( size_type count, value_type const& value )
{
	clear();
	grow( count, value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline void static_vector<Element, ElementCapacity, Alignment>::assign( InputIterator first, InputIterator term )
{
	clear();
	append( first, term );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::assign( rftl::initializer_list<value_type> init )
{
	clear();
	append( init );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::at( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reference static_vector<Element, ElementCapacity, Alignment>::at( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::operator[]( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos].Value();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reference static_vector<Element, ElementCapacity, Alignment>::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos].Value();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::front()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reference static_vector<Element, ElementCapacity, Alignment>::front() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reference static_vector<Element, ElementCapacity, Alignment>::back() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::value_type* static_vector<Element, ElementCapacity, Alignment>::data()
{
	return &( m_Storage[0].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::value_type const* static_vector<Element, ElementCapacity, Alignment>::data() const
{
	return &( m_Storage[0].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::begin()
{
	return &( m_Storage[0].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_iterator static_vector<Element, ElementCapacity, Alignment>::begin() const
{
	return &( m_Storage[0].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_iterator static_vector<Element, ElementCapacity, Alignment>::cbegin() const
{
	return begin();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::end()
{
	return &( m_Storage[size()].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_iterator static_vector<Element, ElementCapacity, Alignment>::end() const
{
	return &( m_Storage[size()].Value() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_iterator static_vector<Element, ElementCapacity, Alignment>::cend() const
{
	return end();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reverse_iterator static_vector<Element, ElementCapacity, Alignment>::rbegin()
{
	return reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator static_vector<Element, ElementCapacity, Alignment>::rbegin() const
{
	return const_reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator static_vector<Element, ElementCapacity, Alignment>::crbegin() const
{
	return const_reverse_iterator( cend() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reverse_iterator static_vector<Element, ElementCapacity, Alignment>::rend()
{
	return reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator static_vector<Element, ElementCapacity, Alignment>::rend() const
{
	return const_reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::const_reverse_iterator static_vector<Element, ElementCapacity, Alignment>::crend() const
{
	return const_reverse_iterator( cbegin() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline bool static_vector<Element, ElementCapacity, Alignment>::empty() const
{
	return size() == 0;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::size_type static_vector<Element, ElementCapacity, Alignment>::size() const
{
	RF_ASSERT( m_CurrentSize <= capacity() );
	return m_CurrentSize;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::size_type static_vector<Element, ElementCapacity, Alignment>::max_size() const
{
	return fixed_capacity;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::reserve( size_type newCap ) const
{
	RF_ASSERT_MSG( newCap <= max_size(), "Cannot grow beyond max size" );
	return;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::size_type static_vector<Element, ElementCapacity, Alignment>::capacity() const
{
	return max_size();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::shrink_to_fit()
{
	// No...
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::clear()
{
	while( empty() == false )
	{
		pop_back();
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::push_back( value_type const& value )
{
	emplace_back( value );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::push_back( value_type&& value )
{
	emplace_back( rftl::move( value ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::emplace_back( value_type&& value )
{
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	new( m_Storage[m_CurrentSize].WriteableTarget() ) Element( rftl::move( value ) );
	m_CurrentSize++;
	return back();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class... Args>
inline typename static_vector<Element, ElementCapacity, Alignment>::reference static_vector<Element, ElementCapacity, Alignment>::emplace_back( Args&&... args )
{
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	new( m_Storage[m_CurrentSize].WriteableTarget() ) Element( args... );
	m_CurrentSize++;
	return back();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::pop_back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting pop_back on empty container" );
	RF_ASSERT( m_CurrentSize > 0 );
	m_CurrentSize--;
	m_Storage[m_CurrentSize].Value().~Element();
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::insert( const_iterator pos, const value_type& value )
{
	value_type copy = value;
	return insert( pos, rftl::move( copy ) );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::insert( const_iterator pos, value_type&& value )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( m_CurrentSize < fixed_capacity );
	for( iterator iter = end(); iter > pos; iter-- )
	{
		iterator dest = iter;
		iterator source = iter - 1;
		if( dest != end() )
		{
			dest->~Element();
		}
		new( dest ) Element( rftl::move( *source ) );
		source->~Element();
	}
	iterator const mutablePos = const_cast<iterator>( pos );
	new( mutablePos ) Element( rftl::move( value ) );
	m_CurrentSize++;
	return mutablePos;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::erase( const_iterator pos )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( m_CurrentSize > 0 );
	iterator const mutablePos = const_cast<iterator>( pos );
	for( iterator iter = mutablePos; iter < end() - 1; iter++ )
	{
		iterator dest = iter;
		iterator source = iter + 1;
		if( dest != end() )
		{
			dest->~Element();
		}
		new( dest ) Element( rftl::move( *source ) );
		source->~Element();
	}
	m_CurrentSize--;
	return mutablePos;
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline typename static_vector<Element, ElementCapacity, Alignment>::iterator static_vector<Element, ElementCapacity, Alignment>::erase( const_iterator first, const_iterator last )
{
	RF_ASSERT( first >= begin() );
	RF_ASSERT( last <= end() );
	RF_ASSERT( first <= last );
	difference_type const numToErase = last - first;
	for( difference_type i = 0; i < numToErase; i++ )
	{
		erase( first );
	}
	return const_cast<iterator>( first );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::resize( size_type count )
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
inline void static_vector<Element, ElementCapacity, Alignment>::resize( size_type count, value_type const& value )
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
inline void static_vector<Element, ElementCapacity, Alignment>::append( InputIterator first, InputIterator term )
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
inline void static_vector<Element, ElementCapacity, Alignment>::append( rftl::initializer_list<value_type> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<typename Convertible>
inline void static_vector<Element, ElementCapacity, Alignment>::append( rftl::initializer_list<Convertible> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
template<class InputIterator>
inline void static_vector<Element, ElementCapacity, Alignment>::extract( InputIterator first, InputIterator term )
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
inline void static_vector<Element, ElementCapacity, Alignment>::grow( size_type growthAmount )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		emplace_back();
	}
}



template<typename Element, size_t ElementCapacity, size_t Alignment>
inline void static_vector<Element, ElementCapacity, Alignment>::grow( size_type growthAmount, value_type const& value )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
